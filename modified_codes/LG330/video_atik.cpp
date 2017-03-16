/*
 * video_atik.cpp
 *
 *  Created on: april 2014
 *      Author: Rumen G.Bogdanovski
 *
 *
 * This file is part of Lin_guider.
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#include <stdio.h>
#include <errno.h>
#include <assert.h>
#include <math.h>
#include <unistd.h>

#include "video_atik.h"
#include "timer.h"
#include "utils.h"
#include "filters.h"


// TODO: replace it with ctimer class
/*
long time_diff(struct timeval *start, struct timeval *end) {
	long msec;

	msec = (end->tv_sec - start->tv_sec) * 1000;
	msec += (end->tv_usec - start->tv_usec) / 1000;

	return msec;
}
*/

namespace video_drv
{


cvideo_atik::cvideo_atik() :
	m_expstart( 0 ),
	m_do_debayer( false )
{
	const atik_core::caps_s& caps = get_caps();
	if (caps.color_type == COLOUR_RGGB) m_do_debayer = true;
	device_type = DT_ATIK;
}


cvideo_atik::~cvideo_atik()
{
	stop();
}


time_fract_t cvideo_atik::set_fps( const time_fract &new_fps )
{
	time_fract_t set_fps = time_fract::mk_fps( 1, 1 );
	int frame_idx = get_frame_idx();

	if( frame_idx != -1 )
	{
		for( int i = 0;i < MAX_FMT;i++ )
		{
			if( new_fps != time_fract::mk_fps( 0, 0 ) &&
				device_formats[0].frame_table[frame_idx].fps_table[i] == new_fps )
			{
				set_fps = new_fps;
				break;
			}
		}
	}

	if( initialized )
		pthread_mutex_lock( &cv_mutex );

	capture_params.fps = set_fps;
	frame_delay = time_fract::to_msecs( capture_params.fps );

	if( initialized )
		pthread_mutex_unlock( &cv_mutex );

	return capture_params.fps;
}


int cvideo_atik::open_device( void )
{
	capture_params.ext_params.insert( std::make_pair( V4L2_CID_ATIK_DODEBAYER, m_do_debayer ) );
	m_do_debayer = capture_params.ext_params[ V4L2_CID_ATIK_DODEBAYER ];
	return open();
}


int cvideo_atik::close_device( void )
{
	return close();
}


int  cvideo_atik::get_vcaps( void )
{
	int i = 0;
	point_t pt;
	AtikCamera* camera = get_camera();
	const atik_core::caps_s& caps = get_caps();

	device_formats[0].format = get_pix_fmt();

	/*
	 * Modify to support iAstroHub, by Anat & Zachary.
	 * Remove all the exposure time greater than 4 sec.
	 * iAstruHub uses the exposure time 1s, 2s and 4s. 
	 */
	pt.x = caps.pixel_count_X;
	pt.y = caps.pixel_count_Y;
	device_formats[0].frame_table[ i ].size =  pt;
	device_formats[0].frame_table[ i ].fps_table[ 0 ] = time_fract::mk_fps( 4, 1 );
	device_formats[0].frame_table[ i ].fps_table[ 1 ] = time_fract::mk_fps( 2, 1 );
	device_formats[0].frame_table[ i ].fps_table[ 2 ] = time_fract::mk_fps( 1, 1 );
	device_formats[0].frame_table[ i ].fps_table[ 3 ] = time_fract::mk_fps( 1, 2 );
	device_formats[0].frame_table[ i ].fps_table[ 4 ] = time_fract::mk_fps( 1, 3 );
	device_formats[0].frame_table[ i ].fps_table[ 5 ] = time_fract::mk_fps( 1, 5 );
	device_formats[0].frame_table[ i ].fps_table[ 6 ] = time_fract::mk_fps( 1, 10 );
	i++;

	if (caps.max_bin_X >= 2 && caps.max_bin_Y >= 2) {
		pt.x = camera->imageWidth(caps.pixel_count_X, 2);
		pt.y = camera->imageHeight(caps.pixel_count_Y, 2);
		device_formats[0].frame_table[ i ].size =  pt;
		device_formats[0].frame_table[ i ].fps_table[ 0 ] = time_fract::mk_fps( 4, 1 );
		device_formats[0].frame_table[ i ].fps_table[ 1 ] = time_fract::mk_fps( 2, 1 );
		device_formats[0].frame_table[ i ].fps_table[ 2 ] = time_fract::mk_fps( 1, 1 );
		device_formats[0].frame_table[ i ].fps_table[ 3 ] = time_fract::mk_fps( 1, 2 );
		device_formats[0].frame_table[ i ].fps_table[ 4 ] = time_fract::mk_fps( 1, 3 );
		device_formats[0].frame_table[ i ].fps_table[ 5 ] = time_fract::mk_fps( 1, 5 );
		device_formats[0].frame_table[ i ].fps_table[ 6 ] = time_fract::mk_fps( 1, 10 );
		i++;
	}

	// enable bin 4x4 for cameras with width > 1300
	if (caps.pixel_count_X  > 1300 && caps.max_bin_X >= 4 && caps.max_bin_Y >= 4) {
		pt.x = camera->imageWidth(caps.pixel_count_X, 4);
		pt.y = camera->imageHeight(caps.pixel_count_Y, 4);
		device_formats[0].frame_table[ i ].size =  pt;
		device_formats[0].frame_table[ i ].fps_table[ 0 ] = time_fract::mk_fps( 4, 1 );
		device_formats[0].frame_table[ i ].fps_table[ 1 ] = time_fract::mk_fps( 2, 1 );
		device_formats[0].frame_table[ i ].fps_table[ 2 ] = time_fract::mk_fps( 1, 1 );
		device_formats[0].frame_table[ i ].fps_table[ 3 ] = time_fract::mk_fps( 1, 2 );
		device_formats[0].frame_table[ i ].fps_table[ 4 ] = time_fract::mk_fps( 1, 3 );
		device_formats[0].frame_table[ i ].fps_table[ 5 ] = time_fract::mk_fps( 1, 5 );
		device_formats[0].frame_table[ i ].fps_table[ 6 ] = time_fract::mk_fps( 1, 10 );
		i++;
	}

	// add empty tail
	pt.x = pt.y = 0;
	device_formats[0].frame_table[ i++ ].size = pt;

	if( enum_controls() ) {
		log_e("Unable to enumerate controls");
		return EXIT_FAILURE;
	}

	return 0;
}

unsigned int cvideo_atik::get_pix_fmt( void )
{
	if(!m_do_debayer) return V4L2_PIX_FMT_Y16;

	const atik_core::caps_s& caps = get_caps();
	if((caps.offsetX == 0) && (caps.offsetY == 0))
		return V4L2_PIX_FMT_SRGGB12;
	else if((caps.offsetX == 1) && (caps.offsetY == 0))
		return V4L2_PIX_FMT_SGRBG12;
	else if((caps.offsetX == 0) && (caps.offsetY == 1))
		return V4L2_PIX_FMT_SGBRG12;
	else if((caps.offsetX == 1) && (caps.offsetY == 1))
		return V4L2_PIX_FMT_SBGGR12;
	return 0;
}

int  cvideo_atik::set_control( unsigned int control_id, const param_val_t &val )
{
	switch( control_id ) {
	case V4L2_CID_EXPOSURE: {
		int v = val.values[0];
		if( v < 0 ) v = 0;
		if( v > 65536 ) v = 65536;
		int top = 65536 - v;
		if( top <= 0 ) {
			log_e( "cvideo_atik::set_control(): invalid exposure" );
			return -1;
		}
		init_lut_to8bit( top );

		capture_params.exposure = v;
		break;
	}
	case V4L2_CID_ATIK_DODEBAYER:
	{
		int v = val.values[0];
		v = v < 0 ? 0 : v;
		v = v > 1 ? 1 : v;
		capture_params.ext_params[ control_id ] = v;
		m_do_debayer = (v == 1);
		capture_params.pixel_format = get_pix_fmt();
		get_vcaps();
		log_i( "Debayering is %s", m_do_debayer ? "ON" : "OFF" );
		break;
	}
	default:
		return -1;
	}
	return 0;
}


int  cvideo_atik::get_control( unsigned int control_id, param_val_t *val )
{
	switch( control_id ) {
	case V4L2_CID_EXPOSURE:
		val->values[0] = capture_params.exposure;
		break;
	default:
		return -1;
	}
	return 0;
}


int cvideo_atik::init_device( void )
{
	int sizeimage = 0;

	// set desired size
	sizeimage = set_format();
	if( sizeimage <= 0 )
		return EXIT_FAILURE;

	set_fps( capture_params.fps );

	n_buffers = 1;
	buffers = (buffer *)calloc( n_buffers, sizeof(*buffers) );

	if( !buffers ) {
		log_e( "Out of memory %s, %s", __FUNCTION__, __LINE__ );
		return EXIT_FAILURE;
	}

	buffers[0].length = sizeimage;
	buffers[0].start.ptr = malloc( sizeimage );

	if( !buffers[0].start.ptr ) {
		log_e( "Out of memory %s, %s", __FUNCTION__, __LINE__ );
		free( buffers );
		return EXIT_FAILURE;
	}

	m_width = capture_params.width;
	m_height = capture_params.height;

	const atik_core::caps_s& caps = get_caps();

	// if the capture resolution is less than the physical,
	// read the full sensor but use bining
	AtikCamera* camera = get_camera();
	if (m_width == camera->imageWidth(caps.pixel_count_X, 2)) { // binX=2
		m_width *= 2;
		m_binX = 2;
	} else if (m_width == camera->imageWidth(caps.pixel_count_X, 4)) { //binX=4
		m_width *= 4;
		m_binX = 4;
	} else m_binX = 1; //binX=1

	if (m_height == camera->imageHeight(caps.pixel_count_Y, 2)) { //binY=2
		m_height *= 2;
		m_binY = 2;
	} else if(m_height == camera->imageHeight(caps.pixel_count_Y, 4)) { //binY=4
		m_height *= 4;
		m_binY = 4;
	} else m_binY = 1; //binY=1

	m_sensor_info = video_drv::sensor_info_s(
		caps.pixel_size_X * m_binX,
		caps.pixel_size_Y * m_binY,
		capture_params.width,
		capture_params.height
	);

	if( DBG_VERBOSITY ) {
		log_i("Image %dx%d binning %dx%d", m_width, m_height, m_binX, m_binY);
	}

	set_exposure( capture_params.exposure );
	get_exposure();

	return 0;
}


int cvideo_atik::uninit_device( void )
{
	if( buffers ) {
		for( int i = 0;i < (int)n_buffers;i++ ) {
			if( buffers[i].start.ptr16 )
				free( buffers[i].start.ptr16 );
		}
		free( buffers );
		buffers = NULL;
	}

	return 0;
}


int cvideo_atik::start_capturing( void )
{
	lock();
	bool success = get_camera()->startExposure(false);
	unlock();
	if( !success ) {
		log_e("startExposure(): failed");
		return 1;
	}
	if( DBG_VERBOSITY )
		log_i( "Exposure started" );

	m_expstart = exp_timer.gettime();

	return 0;
}


int cvideo_atik::stop_capturing( void )
{
	lock();
	get_camera()->abortExposure();
	unlock();

	return 0;
}


int cvideo_atik::read_frame( void )
{
	long time_left;
	bool success;
	data_ptr raw = buffers[0].start;

	time_left = m_expstart + (long)frame_delay - exp_timer.gettime();
	// if exposure time is not elapsed wait for some time to offload the CPU
	// and return. This way we do not have to wait for the long exposures to finish.
	if( time_left > 10 ) {
		usleep(10000);
		return 0; // too much time left, wait some more
	} else if( time_left > 0 )
		usleep(time_left * 1000);


	// If a guide pulse is trigerred between readCCD and getImage()
	// images can not be read and restart is needed. So syncronization with
	// the pusle driver is unavoidable. This may delay the guide pulse
	// by 200ms. This should not be a problem, because in autoguiding mode
	// usually exposures > 0.5s are used and the guiding pulse should be
	// finished by the time next exposure is being read. However this may
	// affect pressing RA+, RA-, DEC+ and DEC- buttons and shorter exposure
	// autoguiding.
	lock();
	success = get_camera()->readCCD(0, 0, m_width, m_height, m_binX, m_binY);
	if( !success )
		log_e("readCCD(): failed");
	if( DBG_VERBOSITY )
		log_i("Exposure finished. Reading %d bytes", buffers[0].length);

	success = get_camera()->getImage(raw.ptr16, capture_params.width * capture_params.height);
	unlock();
	if( !success )
		log_e("getImage(): failed");
	if( DBG_VERBOSITY )
		log_i( "Downloading finished. Read: %d bytes", buffers[0].length);

	lock();
	success = get_camera()->startExposure(false);
	unlock();
	if( !success ) {
		log_e("startExposure(): failed");
		return 1;
	}

	long prev = m_expstart;
	m_expstart = exp_timer.gettime();

	if( DBG_VERBOSITY ) {
		long exptime = m_expstart - prev;
		log_i( "Exposure started. Last frame took %d ms", exptime);
	}

	// synchronize data with GUI
	emit renderImage( buffers[ 0 ].start.ptr, buffers[0].length );

	pthread_mutex_lock( &cv_mutex );
	while( !data_thread_flag )
		pthread_cond_wait( &cv, &cv_mutex );
	data_thread_flag = 0;
	pthread_mutex_unlock( &cv_mutex );

	return 0;
}


int cvideo_atik::set_format( void )
{
	int i, j;
	point_t pt = {0, 0};

	capture_params.pixel_format = get_pix_fmt();
	for( i = 0; i < MAX_FMT && device_formats[i].format;i++ ) {
		if( device_formats[i].format != capture_params.pixel_format )
			continue;
		for( j = 0;j < MAX_FMT && device_formats[i].frame_table[j].size.x;j++ ) {
			if( device_formats[i].frame_table[j].size.x == (int)capture_params.width &&
					device_formats[i].frame_table[j].size.y == (int)capture_params.height ) {
				pt = device_formats[i].frame_table[j].size;
				break;
			}
		}
		if( pt.x == 0 && device_formats[i].frame_table[0].size.x )
			pt = device_formats[i].frame_table[0].size;

		break;
	}

	// set desired size
	capture_params.width  = pt.x;
	capture_params.height = pt.y;

	return capture_params.width * capture_params.height * 2;
}


int cvideo_atik::enum_controls( void )
{
	int n = 0;
	struct v4l2_queryctrl queryctrl;

	memset( &queryctrl, 0, sizeof(v4l2_queryctrl) );
	// create virtual control
	queryctrl.id = V4L2_CID_EXPOSURE;
	queryctrl.type = V4L2_CTRL_TYPE_INTEGER;
	snprintf( (char*)queryctrl.name, sizeof(queryctrl.name)-1, "exposure" );
	queryctrl.minimum = 0;
	queryctrl.maximum = 65535;
	queryctrl.step = 1;
	queryctrl.default_value = 0;
	queryctrl.flags = 0;
	// Add control to control list
	controls = add_control( -1, &queryctrl, controls, &n );

	// create virtual control (extended ctl)
	// Older colour Atiks do not report themseves as colour, however the pattern is RGGB
	// that is why enforce debayering is available even if the camera reports MONO
	queryctrl.id = V4L2_CID_ATIK_DODEBAYER;
	queryctrl.type = V4L2_CTRL_TYPE_BOOLEAN;
	snprintf( (char*)queryctrl.name, sizeof(queryctrl.name)-1, "Debayer RGB pattern" );
	queryctrl.minimum = 0;
	queryctrl.maximum = 1;
	queryctrl.step = 1;
	queryctrl.default_value = m_do_debayer;
	queryctrl.flags = 0;
	// Add control to control list
	controls = add_control( -1, &queryctrl, controls, &n, true );

	num_controls = n;

	return 0;
}

}
