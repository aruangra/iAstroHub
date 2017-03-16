/*
 * video_asi.cpp
 *
 *  Created on: february 2015
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

#include "video_asi.h"
#include "timer.h"
#include "utils.h"
#include "filters.h"
#include "bayer.h"

namespace video_drv
{

cvideo_asi::cvideo_asi()
{
	device_type = DT_ASI;
}


cvideo_asi::~cvideo_asi()
{
	stop();
}


time_fract_t cvideo_asi::set_fps( const time_fract &new_fps )
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
	abort_exposure();
	set_camera_exposure(frame_delay);
	start_exposure();

	if( initialized )
		pthread_mutex_unlock( &cv_mutex );

	return capture_params.fps;
}


int cvideo_asi::open_device( void )
{

	int result = open();
	if (result) return result;

	// Default Values
	m_transfer_bits = 8;
	m_bandwidth = m_has_bwidth ? m_bwidth_caps.DefaultValue : 0;
	m_clear_buffs =1;
	m_wb_r = m_has_wb_r ? m_wb_r_caps.DefaultValue : 0;
	m_wb_b = m_has_wb_b ? m_wb_b_caps.DefaultValue : 0;
	m_force_bw = 0;
	m_clear_buffs = 1;

	capture_params.ext_params.insert( std::make_pair( V4L2_CID_USER_ASI8BIT, m_transfer_bits == 8 ? 1 : 0 ) );
	m_transfer_bits = capture_params.ext_params[ V4L2_CID_USER_ASI8BIT ] == 0 ? 16 : 8;
	capture_params.ext_params.insert( std::make_pair( V4L2_CID_USER_BANDWIDTH, m_bandwidth ) );
	m_bandwidth = capture_params.ext_params[ V4L2_CID_USER_BANDWIDTH ];
	capture_params.ext_params.insert( std::make_pair( V4L2_CID_RED_BALANCE, m_wb_r ) );
	m_wb_r = capture_params.ext_params[ V4L2_CID_RED_BALANCE ];
	capture_params.ext_params.insert( std::make_pair( V4L2_CID_BLUE_BALANCE, m_wb_b ) );
	m_wb_b = capture_params.ext_params[ V4L2_CID_BLUE_BALANCE ];
	capture_params.ext_params.insert( std::make_pair( V4L2_CID_USER_CLEAR_BUFFS, m_clear_buffs ) );
	m_clear_buffs = capture_params.ext_params[ V4L2_CID_USER_CLEAR_BUFFS ];
	capture_params.ext_params.insert( std::make_pair( V4L2_CID_USER_FORCE_BW, m_force_bw ) );
	m_force_bw = capture_params.ext_params[ V4L2_CID_USER_FORCE_BW ];

	if (m_transfer_bits == 8) {
		if (m_force_bw) set_camera_image_type(ASI_IMG_Y8);
		else set_camera_image_type(ASI_IMG_RAW8);
	} else set_camera_image_type(ASI_IMG_RAW16);
	update_camera_image_type();

	capture_params.pixel_format = get_pix_fmt();

	return EXIT_SUCCESS;
}


int cvideo_asi::close_device( void )
{
	return close();
}


int  cvideo_asi::get_vcaps( void )
{
	int i = 0;
	point_t pt;

	device_formats[0].format = get_pix_fmt();

	/*
	 * Modify to support iAstroHub, by Anat & Zachary.
	 * Remove all the exposure time greater than 4 sec.
	 * iAstruHub uses the exposure time 1s, 2s and 4s. 
	 */
	 
	pt.x = m_cam_info.MaxWidth;
	pt.y = m_cam_info.MaxHeight;
	device_formats[0].frame_table[ i ].size =  pt;
	device_formats[0].frame_table[ i ].fps_table[ 0 ] = time_fract::mk_fps( 4, 1 );
	device_formats[0].frame_table[ i ].fps_table[ 1 ] = time_fract::mk_fps( 2, 1 );
	device_formats[0].frame_table[ i ].fps_table[ 2 ] = time_fract::mk_fps( 1, 1 );
	device_formats[0].frame_table[ i ].fps_table[ 3 ] = time_fract::mk_fps( 1, 2 );
	device_formats[0].frame_table[ i ].fps_table[ 4 ] = time_fract::mk_fps( 1, 3 );
	device_formats[0].frame_table[ i ].fps_table[ 5 ] = time_fract::mk_fps( 1, 5 );
	device_formats[0].frame_table[ i ].fps_table[ 6 ] = time_fract::mk_fps( 1, 10 );
	i++;

	pt.x = m_cam_info.MaxWidth/2;
	pt.y = m_cam_info.MaxHeight/2;
	device_formats[0].frame_table[ i ].size =  pt;
	device_formats[0].frame_table[ i ].fps_table[ 0 ] = time_fract::mk_fps( 4, 1 );
	device_formats[0].frame_table[ i ].fps_table[ 1 ] = time_fract::mk_fps( 2, 1 );
	device_formats[0].frame_table[ i ].fps_table[ 2 ] = time_fract::mk_fps( 1, 1 );
	device_formats[0].frame_table[ i ].fps_table[ 3 ] = time_fract::mk_fps( 1, 2 );
	device_formats[0].frame_table[ i ].fps_table[ 4 ] = time_fract::mk_fps( 1, 3 );
	device_formats[0].frame_table[ i ].fps_table[ 5 ] = time_fract::mk_fps( 1, 5 );
	device_formats[0].frame_table[ i ].fps_table[ 6 ] = time_fract::mk_fps( 1, 10 );
	i++;

	// add empty tail
	pt.x = pt.y = 0;
	device_formats[0].frame_table[ i++ ].size = pt;

	if( enum_controls() ) {
		log_e("Unable to enumerate controls");
		return EXIT_FAILURE;
	}

	return 0;
}


int  cvideo_asi::set_control( unsigned int control_id, const param_val_t &val )
{
	switch( control_id ) {
	case V4L2_CID_GAIN:
	{
		int v = val.values[0];
		if( v < m_gain_caps.MinValue ) v = m_gain_caps.MinValue;
		if( v > m_gain_caps.MaxValue ) v = m_gain_caps.MaxValue;
		bool success = set_camera_gain(v);
		if( !success ) {
			return -1;
		}
		capture_params.gain = v;
		break;
	}
	case V4L2_CID_EXPOSURE: {
		long wp_max = 255;
		if(m_bpp == 16) wp_max = 65535;
		int v = val.values[0];
		if( v < 0 ) v = 0;
		if( v > wp_max ) v = wp_max;
		int top = wp_max - v;
		if( top <= 0 ) {
			log_e( "cvideo_asi::set_control(): invalid exposure" );
			return -1;
		}
		init_lut_to8bit( top );
		capture_params.exposure = v;
		break;
	}
	case V4L2_CID_USER_BANDWIDTH:
	{
		//log_e( "USB Bandwidth = %d", m_bandwidth);
		int v = val.values[0];
		if( v < m_bwidth_caps.MinValue ) v = m_bwidth_caps.MinValue;
		if( v > m_bwidth_caps.MaxValue-10 ) v = m_bwidth_caps.MaxValue-10;
		capture_params.ext_params[ control_id ] = v;
		m_bandwidth = v;
		set_band_width(m_bandwidth);
		if (DBG_VERBOSITY)
			log_i( "USB Bandwidth = %d", m_bandwidth);
		break;
	}
	case V4L2_CID_RED_BALANCE:
	{
		//log_e( "USB Bandwidth = %d", m_bandwidth);
		int v = val.values[0];
		if( v < m_wb_r_caps.MinValue ) v = m_wb_r_caps.MinValue;
		if( v > m_wb_r_caps.MaxValue-10 ) v = m_wb_r_caps.MaxValue-10;
		capture_params.ext_params[ control_id ] = v;
		m_wb_r = v;
		set_wb_r(m_wb_r);
		if (DBG_VERBOSITY)
			log_i( "WB_R = %d", m_wb_r);
		break;
	}
	case V4L2_CID_BLUE_BALANCE:
	{
		//log_e( "USB Bandwidth = %d", m_bandwidth);
		int v = val.values[0];
		if( v < m_wb_b_caps.MinValue ) v = m_wb_b_caps.MinValue;
		if( v > m_wb_b_caps.MaxValue-10 ) v = m_wb_b_caps.MaxValue-10;
		capture_params.ext_params[ control_id ] = v;
		m_wb_b = v;
		set_wb_b(m_wb_b);
		if (DBG_VERBOSITY)
			log_i( "WB_B = %d", m_wb_b);
		break;
	}
	case V4L2_CID_USER_CLEAR_BUFFS:
	{
		int v = val.values[0];
		if( v < 0 || v > 1 )
			log_e( "%s: invalid value", __FUNCTION__ );
		v = v < 0 ? 0 : v;
		v = v > 1 ? 1 : v;
		capture_params.ext_params[ control_id ] = v;
		m_clear_buffs = v;
		if (DBG_VERBOSITY)
			log_i( "Clear buffs: %d", v );
		break;
	}
	case V4L2_CID_USER_FORCE_BW:
	{
		int v = val.values[0];
		v = v < 0 ? 0 : v;
		v = v > 1 ? 1 : v;
		capture_params.ext_params[ control_id ] = v;
		m_force_bw = (v == 1);
		if (m_transfer_bits == 8) {
			if (m_force_bw) set_camera_image_type(ASI_IMG_Y8);
			else set_camera_image_type(ASI_IMG_RAW8);
		} else set_camera_image_type(ASI_IMG_RAW16);
		capture_params.pixel_format = get_pix_fmt();
		log_i( "Forced BW mode is %s", m_force_bw ? "ON" : "OFF" );
		break;
	}
	case V4L2_CID_USER_ASI8BIT:
	{
		capture_params.ext_params[ control_id ] = val.values[0];
		log_i( "Ctrl 8-bit: %d, (restart is required)", val.values[0] );
		if( val.values[0] == 0 ) // mode 12-bit
			log_i( "NOTE: On some camera models 12-bit video mode disables guider port!" );
		break;
	}
	default:
		return -1;
	}
	return 0;
}


int  cvideo_asi::get_control( unsigned int control_id, param_val_t *val )
{
	switch( control_id ) {
	case V4L2_CID_GAIN:
	{
		val->values[0] = capture_params.gain;
		break;
	}
	case V4L2_CID_EXPOSURE:
		val->values[0] = capture_params.exposure;
		break;
	default:
		return -1;
	}
	return 0;
}


int cvideo_asi::init_device( void )
{
	int sizeimage = 0;
	ASI_ERROR_CODE result;

	set_fps( capture_params.fps );

	// set desired size
	sizeimage = set_format();
	if( sizeimage <= 0 )
		return EXIT_FAILURE;

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
	if (m_width == m_cam_info.MaxWidth/2) {
		m_width *= 2;
		m_binX = 2;
	} else if (m_width == m_cam_info.MaxHeight/4) {
		m_width *= 4;
		m_binX = 4;
	} else m_binX = 1;

	if (m_height == m_cam_info.MaxHeight/2) {
		m_height *= 2;
		m_binY = 2;
	} else if (m_height == m_cam_info.MaxHeight/4) {
		m_height *= 4;
		m_binY = 4;
	} else m_binY = 1;

	m_sensor_info = video_drv::sensor_info_s(
		m_cam_info.PixelSize * m_binX,
		m_cam_info.PixelSize * m_binY,
		capture_params.width,
		capture_params.height
	);

	if( DBG_VERBOSITY ) {
		log_i("Image %dx%d binning %dx%d", m_width, m_height, m_binX, m_binY);
	}

	set_exposure( capture_params.exposure );
	get_exposure();

	result = pASISetROIFormat(m_camera, capture_params.width, capture_params.height, m_binX, m_img_type);
	if(result != ASI_SUCCESS) {
		log_e("ASISetROIFormat() returned %d", result);
		return EXIT_FAILURE;
	}

	set_camera_gain(capture_params.gain);
	set_band_width(m_bandwidth);
	set_wb_r(m_wb_r);
	set_wb_b(m_wb_b);

	return 0;
}


int cvideo_asi::uninit_device( void )
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


int cvideo_asi::start_capturing( void )
{
	set_camera_exposure(frame_delay);
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


int cvideo_asi::stop_capturing( void )
{
	abort_exposure();
	return 0;
}


int cvideo_asi::read_frame( void )
{
	//long time_left;
	bool success;
	data_ptr raw = buffers[0].start;
	int raw_size = buffers[0].length;

	// DO NOT remove this code it will be utilized!!!!

	//time_left = m_expstart + (long)frame_delay - exp_timer.gettime();
	// if exposure time is not elapsed wait for some time to offload the CPU
	// and return. This way we do not have to wait for the long exposures to finish.
	/*
	if( time_left > 10 ) {
		usleep(10000);
		return 0; // too much time left, wait some more
	} else if( time_left > 0 )
		usleep(time_left * 1000);
	*/
	success = read_image((char *) raw.ptr8, raw_size, frame_delay);
	if( !success )
		log_e("read_image(): failed");
	if( DBG_VERBOSITY )
		log_i( "Exposure finished. Read: %d bytes", raw_size);

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

unsigned int cvideo_asi::get_pix_fmt( void ) {
	if (m_bpp == 16) {
		if (m_cam_info.IsColorCam) {
			switch (m_cam_info.BayerPattern) {
			case ASI_BAYER_RG:
				return V4L2_PIX_FMT_SRGGB12;
			case ASI_BAYER_BG:
				return V4L2_PIX_FMT_SBGGR12;
			case ASI_BAYER_GR:
				return V4L2_PIX_FMT_SGRBG12;
			case ASI_BAYER_GB:
				return V4L2_PIX_FMT_SGBRG12;
			default:
				return V4L2_PIX_FMT_Y16;
			}
		} else {
			return V4L2_PIX_FMT_Y16;
		}
	} else if (m_bpp == 8) {
		if (m_cam_info.IsColorCam) {
			switch (m_cam_info.BayerPattern) {
			case ASI_BAYER_RG:
				return PIX_FMT_SRGGB8;
			case ASI_BAYER_BG:
				return PIX_FMT_SBGGR8;
			case ASI_BAYER_GR:
				return PIX_FMT_SGRBG8;
			case ASI_BAYER_GB:
				return PIX_FMT_SGBRG8;
			default:
				return V4L2_PIX_FMT_GREY;
			}
		} else {
			return V4L2_PIX_FMT_GREY;
		}
	} else return 0;
}

int cvideo_asi::set_format( void )
{
	int i, j;
	point_t pt = {0, 0};

	device_formats[0].format = get_pix_fmt();
	capture_params.pixel_format = device_formats[0].format;

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

	return capture_params.width * capture_params.height * m_bpp / 8;
}


int cvideo_asi::enum_controls( void )
{
	int n = 0;
	struct v4l2_queryctrl queryctrl;

	memset( &queryctrl, 0, sizeof(v4l2_queryctrl) );
	if (m_has_gain) {
		// create virtual control
		queryctrl.id = V4L2_CID_GAIN;
		queryctrl.type = V4L2_CTRL_TYPE_INTEGER;
		snprintf( (char*)queryctrl.name, sizeof(queryctrl.name)-1, "gain" );
		queryctrl.minimum = m_gain_caps.MinValue;
		queryctrl.maximum = m_gain_caps.MaxValue;
		queryctrl.step = 1;
		queryctrl.default_value = m_gain_caps.DefaultValue;
		queryctrl.flags = 0;
		// Add control to control list
		controls = add_control( -1, &queryctrl, controls, &n );
	}

	long wp_max = 255;
	if(m_bpp == 16) wp_max = 65535;
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

	if(m_has_bwidth) {
		// create virtual control (extended ctl)
		queryctrl.id = V4L2_CID_USER_BANDWIDTH;
		queryctrl.type = V4L2_CTRL_TYPE_INTEGER;
		snprintf( (char*)queryctrl.name, sizeof(queryctrl.name)-1, "USB Bandwidth" );
		queryctrl.minimum = m_bwidth_caps.MinValue;
		queryctrl.maximum = m_bwidth_caps.MaxValue-5;
		queryctrl.step = 1;
		queryctrl.default_value = m_bandwidth;
		queryctrl.flags = 0;
		// Add control to control list
		controls = add_control( -1, &queryctrl, controls, &n, true );
	}

	if(m_has_wb_r) {
		// create virtual control (extended ctl)
		queryctrl.id = V4L2_CID_RED_BALANCE;
		queryctrl.type = V4L2_CTRL_TYPE_INTEGER;
		snprintf( (char*)queryctrl.name, sizeof(queryctrl.name)-1, "White Balance R" );
		queryctrl.minimum = m_wb_r_caps.MinValue;
		queryctrl.maximum = m_wb_r_caps.MaxValue;
		queryctrl.step = 1;
		queryctrl.default_value = m_wb_r;
		queryctrl.flags = 0;
		// Add control to control list
		controls = add_control( -1, &queryctrl, controls, &n, true );
	}

	if(m_has_wb_b) {
		// create virtual control (extended ctl)
		queryctrl.id = V4L2_CID_BLUE_BALANCE;
		queryctrl.type = V4L2_CTRL_TYPE_INTEGER;
		snprintf( (char*)queryctrl.name, sizeof(queryctrl.name)-1, "White Balance B" );
		queryctrl.minimum = m_wb_b_caps.MinValue;
		queryctrl.maximum = m_wb_b_caps.MaxValue;
		queryctrl.step = 1;
		queryctrl.default_value = m_wb_b;
		queryctrl.flags = 0;
		// Add control to control list
		controls = add_control( -1, &queryctrl, controls, &n, true );
	}

	// create virtual control
	queryctrl.id = V4L2_CID_USER_CLEAR_BUFFS;
	queryctrl.type = V4L2_CTRL_TYPE_BOOLEAN;
	snprintf( (char*)queryctrl.name, sizeof(queryctrl.name)-1, "Clear buffers" );
	queryctrl.minimum = 0;
	queryctrl.maximum = 1;
	queryctrl.step = 1;
	queryctrl.default_value = m_clear_buffs;
	queryctrl.flags = 0;
	// Add control to control list
	controls = add_control( -1, &queryctrl, controls, &n, true );

	if (m_transfer_bits == 8) { // Force SDK to render BW frames (abailable in 8 bits only)
		// create virtual control
		queryctrl.id = V4L2_CID_USER_FORCE_BW;
		queryctrl.type = V4L2_CTRL_TYPE_BOOLEAN;
		snprintf( (char*)queryctrl.name, sizeof(queryctrl.name)-1, "Force BW frames" );
		queryctrl.minimum = 0;
		queryctrl.maximum = 1;
		queryctrl.step = 1;
		queryctrl.default_value = m_force_bw;
		queryctrl.flags = 0;
		// Add control to control list
		controls = add_control( -1, &queryctrl, controls, &n, true );
	}

	// create virtual control
	queryctrl.id = V4L2_CID_USER_ASI8BIT;
	queryctrl.type = V4L2_CTRL_TYPE_BOOLEAN;
	snprintf( (char*)queryctrl.name, sizeof(queryctrl.name)-1, "8-bit mode" );
	queryctrl.minimum = 0;
	queryctrl.maximum = 1;
	queryctrl.step = 1;
	queryctrl.default_value = 1;
	queryctrl.flags = 0;
	// Add control to control list
	controls = add_control( -1, &queryctrl, controls, &n, true );

	num_controls = n;

	return 0;
}

}
