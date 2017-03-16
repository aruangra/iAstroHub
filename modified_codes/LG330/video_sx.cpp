/*
 * video_sx.cpp
 *
 *  Created on: january 2015
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

#include "video_sx.h"
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


cvideo_sx::cvideo_sx()
{
	device_type = DT_SX;
}


cvideo_sx::~cvideo_sx()
{
	stop();
}


time_fract_t cvideo_sx::set_fps( const time_fract &new_fps )
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


int cvideo_sx::open_device( void )
{
	return open();
}


int cvideo_sx::close_device( void )
{
	return close();
}


int  cvideo_sx::get_vcaps( void )
{
	int i = 0;
	point_t pt;

	if (m_caps.bits_per_pixel == 16)
		device_formats[0].format = V4L2_PIX_FMT_Y16;
	else if (m_caps.bits_per_pixel == 8)
		device_formats[0].format = V4L2_PIX_FMT_GREY;
	else
		return 1;

	/*
	 * Modify to support iAstroHub, by Anat & Zachary.
	 * Remove all the exposure time greater than 4 sec.
	 * iAstruHub uses the exposure time 1s, 2s and 4s. 
	 */
	pt.x = m_caps.width;
	pt.y = m_caps.height;
	device_formats[0].frame_table[ i ].size =  pt;
	device_formats[0].frame_table[ i ].fps_table[ 0 ] = time_fract::mk_fps( 4, 1 );
	device_formats[0].frame_table[ i ].fps_table[ 1 ] = time_fract::mk_fps( 2, 1 );
	device_formats[0].frame_table[ i ].fps_table[ 2 ] = time_fract::mk_fps( 1, 1 );
	device_formats[0].frame_table[ i ].fps_table[ 3 ] = time_fract::mk_fps( 1, 2 );
	device_formats[0].frame_table[ i ].fps_table[ 4 ] = time_fract::mk_fps( 1, 3 );
	device_formats[0].frame_table[ i ].fps_table[ 5 ] = time_fract::mk_fps( 1, 5 );
	device_formats[0].frame_table[ i ].fps_table[ 6 ] = time_fract::mk_fps( 1, 10 );
	i++;

	pt.x = m_caps.width/2;
	pt.y = m_caps.height/2;
	device_formats[0].frame_table[ i ].size =  pt;
	device_formats[0].frame_table[ i ].fps_table[ 0 ] = time_fract::mk_fps( 4, 1 );
	device_formats[0].frame_table[ i ].fps_table[ 1 ] = time_fract::mk_fps( 2, 1 );
	device_formats[0].frame_table[ i ].fps_table[ 2 ] = time_fract::mk_fps( 1, 1 );
	device_formats[0].frame_table[ i ].fps_table[ 3 ] = time_fract::mk_fps( 1, 2 );
	device_formats[0].frame_table[ i ].fps_table[ 4 ] = time_fract::mk_fps( 1, 3 );
	device_formats[0].frame_table[ i ].fps_table[ 5 ] = time_fract::mk_fps( 1, 5 );
	device_formats[0].frame_table[ i ].fps_table[ 6 ] = time_fract::mk_fps( 1, 10 );
	i++;

	if (m_caps.width > 1300) {
		pt.x = m_caps.width/4;
		pt.y = m_caps.height/4;
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


int  cvideo_sx::set_control( unsigned int control_id, const param_val_t &val )
{
	switch( control_id ) {
	case V4L2_CID_EXPOSURE: {
		long wp_max = 255;
		if(m_caps.bits_per_pixel == 16) wp_max = 65536;
		int v = val.values[0];
		if( v < 0 ) v = 0;
		if( v > wp_max ) v = wp_max;
		int top = wp_max - v;
		if( top <= 0 ) {
			log_e( "cvideo_sx::set_control(): invalid exposure" );
			return -1;
		}
		init_lut_to8bit( top );

		capture_params.exposure = v;
		break;
	}
	default:
		return -1;
	}
	return 0;
}


int  cvideo_sx::get_control( unsigned int control_id, param_val_t *val )
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


int cvideo_sx::init_device( void )
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

	// if the capture resolution is half the physical,
	// read the full sensor but use bining
	if (m_width == m_caps.width/2) {
		m_width *= 2;
		m_binX = 2;
	} else if (m_width == m_caps.width/4) {
		m_width *= 4;
		m_binX = 4;
	} else m_binX = 1;

	if (m_height == m_caps.height/2) {
		m_height *= 2;
		m_binY = 2;
	} else if (m_height == m_caps.height/4) {
		m_height *= 4;
		m_binY = 4;
	} else m_binY = 1;

	m_sensor_info = video_drv::sensor_info_s(
		m_caps.pix_width * m_binX,
		m_caps.pix_height * m_binY,
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


int cvideo_sx::uninit_device( void )
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


int cvideo_sx::start_capturing( void )
{
	m_expstart = exp_timer.gettime();
	bool success = start_exposure();
	if( !success ) {
		log_e("startExposure(): failed");
		return 1;
	}
	if( DBG_VERBOSITY )
		log_i( "Exposure started" );

	return 0;
}


int cvideo_sx::stop_capturing( void )
{
	abort_exposure();
	return 0;
}


int cvideo_sx::read_frame( void )
{
	long time_left;
	bool success;
	data_ptr raw = buffers[0].start;
	int raw_size = buffers[0].length;

	time_left = m_expstart + (long)frame_delay - exp_timer.gettime();
	// if exposure time is not elapsed wait for some time to offload the CPU
	// and return. This way we do not have to wait for the long exposures to finish.
	if( time_left > 10 ) {
		usleep(10000);
		return 0; // too much time left, wait some more
	} else if( time_left > 0 )
		usleep(time_left * 1000);

	success = read_image((char *) raw.ptr16, raw_size);
	if( !success )
		log_e("read_image(): failed");
	if( DBG_VERBOSITY )
		log_i( "Exposure finished. Read: %d bytes", raw_size);

	long prev = m_expstart;
	m_expstart = exp_timer.gettime();

	success = start_exposure();
	if( !success ) {
		log_e("start_exposure(): failed");
		return 1;
	}

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


int cvideo_sx::set_format( void )
{
	int i, j;
	point_t pt = {0, 0};

	if (m_caps.bits_per_pixel == 16)
		capture_params.pixel_format = V4L2_PIX_FMT_Y16;
	else if (m_caps.bits_per_pixel == 8)
		capture_params.pixel_format = V4L2_PIX_FMT_GREY;
	else
		return 0;

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

	return capture_params.width * capture_params.height * m_caps.bits_per_pixel/8;
}


int cvideo_sx::enum_controls( void )
{
	int n = 0;
	struct v4l2_queryctrl queryctrl;

	memset( &queryctrl, 0, sizeof(v4l2_queryctrl) );

	long wp_max = 255;
	if(m_caps.bits_per_pixel == 16) wp_max = 65535;
	// create virtual control
	queryctrl.id = V4L2_CID_EXPOSURE;
	queryctrl.type = V4L2_CTRL_TYPE_INTEGER;
	snprintf( (char*)queryctrl.name, sizeof(queryctrl.name)-1, "exposure" );
	queryctrl.minimum = 0;
	queryctrl.maximum = wp_max;
	queryctrl.step = 1;
	queryctrl.default_value = 0;
	queryctrl.flags = 0;
	// Add control to control list
	controls = add_control( -1, &queryctrl, controls, &n );

	num_controls = n;

	return 0;
}

}
