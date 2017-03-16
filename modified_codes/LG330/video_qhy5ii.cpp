/*
 * video_qhy5ii.cpp
 *
 *  Created on: 05.10.2013
 *      Author: gm
 *
 * Device access code is based on original QHY code from https://github.com/qhyccd-lzr
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
#include <unistd.h>
#include <errno.h>
#include <assert.h>
#include <math.h>
#include <netinet/in.h>

#include <algorithm>
#include <map>
#include <utility>

#include "video_qhy5ii.h"
#include "timer.h"
#include "utils.h"
#include "maindef.h"
#include "lusb.h"

namespace video_drv
{

int cvideo_qhy5ii::m_gain_lut[73] =
{
		0x004, 0x005, 0x006, 0x007, 0x008, 0x009, 0x00A, 0x00B, 0x00C, 0x00D, 0x00E,
		0x00F, 0x010, 0x011, 0x012, 0x013, 0x014, 0x015, 0x016, 0x017, 0x018, 0x019,
		0x01A, 0x01B, 0x01C, 0x01D, 0x01E, 0x01F, 0x051, 0x052, 0x053, 0x054, 0x055,
		0x056, 0x057, 0x058, 0x059, 0x05A, 0x05B, 0x05C, 0x05D, 0x05E, 0x05F, 0x6CE,
		0x6CF, 0x6D0, 0x6D1, 0x6D2, 0x6D3, 0x6D4, 0x6D5, 0x6D6, 0x6D7, 0x6D8, 0x6D9,
		0x6DA, 0x6DB, 0x6DC, 0x6DD, 0x6DE, 0x6DF, 0x6E0, 0x6E1, 0x6E2, 0x6E3, 0x6E4,
		0x6E5, 0x6E6, 0x6E7, 0x6FC, 0x6FD, 0x6FE, 0x6FF
};


//-------------------------------------------- QHY5II ---------------------------------------------------------
// QHY5II stuff
cvideo_qhy5ii::cvideo_qhy5ii() :
	m_data_size( 0 ),

	m_is_color( false ),
	m_dev_type( DEVICETYPE_UNKOWN ),
	m_width( 0 ),
	m_height( 0 ),
	m_transfer_bit( 8 ),
	m_bin( 11 ),
	m_transfer_speed( 0 ),
	m_usb_traf( 0 ),

	m_wbblue( 145 ),
	m_wbred( 115 ),
	m_do_debayer( false ),

	m_qhy5iiDeNoise( false )
{
	device_type = DT_QHY5II;

	capture_params.fps = time_fract::mk_fps( 1, 1 );
	capture_params.gain = 1;
	capture_params.exposure = 1;

	m_qhy5ii_obj = new qhy5ii_core_shared();
}


cvideo_qhy5ii::~cvideo_qhy5ii()
{
	//MUST BE called the first: stop thread and release resources
	stop();

	delete m_qhy5ii_obj;
}


int cvideo_qhy5ii::open_device( void )
{
	int ret = EXIT_SUCCESS;

	ret = m_qhy5ii_obj->open_device();
	if( ret != EXIT_SUCCESS )
		return ret;
	ret = m_qhy5ii_obj->get_dev_info( &m_dev_type, &m_is_color );

	capture_params.ext_params.insert( std::make_pair( V4L2_CID_USER_8BIT, m_transfer_bit == 8 ? 1 : 0 ) );
	m_transfer_bit = capture_params.ext_params[ V4L2_CID_USER_8BIT ] == 0 ? 16 : 8;

	m_usb_traf = 32;
	m_transfer_speed = 0;
	if( m_dev_type == DEVICETYPE_QHY5LII )
	{
		#ifdef __arm__
			log_i("System CPU is ARM");
			m_usb_traf = 40;
		#else
			log_i("System CPU is not ARM");
			m_usb_traf = 5;
		#endif
	}
	else
	if( m_dev_type == DEVICETYPE_QHY5II )
	{
		#ifdef __arm__
			log_i("System CPU is ARM");
			m_usb_traf = 150;
		#else
			log_i("System CPU is not ARM");
			m_usb_traf = 30;
		#endif
	}
	capture_params.ext_params.insert( std::make_pair( V4L2_CID_USER_USB_TRAF, m_usb_traf ) );
	m_usb_traf = capture_params.ext_params[ V4L2_CID_USER_USB_TRAF ];
	capture_params.ext_params.insert( std::make_pair( V4L2_CID_USER_DSPEED, m_transfer_speed ) );
	m_transfer_speed = capture_params.ext_params[ V4L2_CID_USER_DSPEED ];

	// disable color and 16bit for non-L as here's no code it
	if( m_dev_type == DEVICETYPE_QHY5II )
	{
		m_is_color = false;
		m_transfer_bit = 8;	// 16bit as default crashes QHY5-II (non-L) as set_transfer_bit() drops down m_transfer_bit to 8 and init_device() allocates too few mem
	}
	// for tests and debugging
	//m_is_color = true;

	if( m_is_color )
	{
		capture_params.ext_params.insert( std::make_pair( V4L2_CID_RED_BALANCE, m_wbred ) );
		capture_params.ext_params.insert( std::make_pair( V4L2_CID_BLUE_BALANCE, m_wbblue ) );
		capture_params.ext_params.insert( std::make_pair( V4L2_CID_USER_DODEBAYER, m_do_debayer ) );
		m_wbred = capture_params.ext_params[ V4L2_CID_RED_BALANCE ];
		m_wbblue = capture_params.ext_params[ V4L2_CID_BLUE_BALANCE ];
		m_do_debayer = capture_params.ext_params[ V4L2_CID_USER_DODEBAYER ];
	}

	if( m_dev_type == DEVICETYPE_QHY5LII ) {
		m_sensor_info = video_drv::sensor_info_s(
			3.75,
			3.75,
			QHY5LII_MATRIX_WIDTH,
			QHY5LII_MATRIX_HEIGHT
		);
	} else {
		m_sensor_info = video_drv::sensor_info_s(
			5.2,
			5.2,
			QHY5II_MATRIX_WIDTH,
			QHY5II_MATRIX_HEIGHT
		);
	}

	// warning
	if( m_dev_type == DEVICETYPE_QHY5LII && m_transfer_bit == 16 )
		log_i( "Note! 12-bit video mode disables guider port." );

	return ret;
}


int cvideo_qhy5ii::close_device( void )
{
    m_qhy5ii_obj->close_device();

    return EXIT_SUCCESS;
}


time_fract_t cvideo_qhy5ii::set_fps( const time_fract_t &new_fps )
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

	int ret = EXIT_FAILURE;
	unsigned frm_delay = time_fract::to_msecs( set_fps );
	stop_video_mode();

	//ret = set_exposure_time( frm_delay );
	// in this place I have to replace standard call with a strange chinese kludge below
	{
		capture_params.fps = set_fps;
		frame_delay = frm_delay;
		ret = qhy5_exposure_kludge( frm_delay );
	}

	if( ret != EXIT_SUCCESS )
	{
		log_e( "cvideo_qhy5ii::set_fps(): set_params() failed." );
	}
	else
	{
		//force_fps = true;
		capture_params.fps = set_fps;
		frame_delay = frm_delay;

		log_i( "  Frame rate:   %u/%u fps", set_fps.denominator, set_fps.numerator );
	}

	start_video_mode();

	if( initialized )
		pthread_mutex_unlock( &cv_mutex );

 return capture_params.fps;
}



int cvideo_qhy5ii::get_vcaps( void )
{
	int i = 0;
	point_t pt;

	device_formats[0].format = get_pix_fmt();

	if( m_dev_type == DEVICETYPE_QHY5LII )
	{
		/*
	 	 * Modify to support iAstroHub, by Anat & Zachary.
	 	 * Remove all the exposure time greater than 4 sec.
	 	 * iAstruHub uses the exposure time 1s, 2s and 4s. 
	 	 */
		pt.x = QHY5LII_WIDTH_B4;
		pt.y = QHY5LII_HEIGHT_B4;
		device_formats[0].frame_table[ i ].size =  pt;
		device_formats[0].frame_table[ i ].fps_table[ 0 ] = time_fract::mk_fps( 2, 1 );
		device_formats[0].frame_table[ i ].fps_table[ 1 ] = time_fract::mk_fps( 1, 1 );
		device_formats[0].frame_table[ i ].fps_table[ 2 ] = time_fract::mk_fps( 1, 2 );
		i++;

		pt.x = QHY5LII_WIDTH_B3;
		pt.y = QHY5LII_HEIGHT_B3;
		device_formats[0].frame_table[ i ].size =  pt;
		device_formats[0].frame_table[ i ].fps_table[ 0 ] = time_fract::mk_fps( 2, 1 );
		device_formats[0].frame_table[ i ].fps_table[ 1 ] = time_fract::mk_fps( 1, 1 );
		device_formats[0].frame_table[ i ].fps_table[ 2 ] = time_fract::mk_fps( 1, 2 );
		i++;

		pt.x = QHY5LII_WIDTH_B2;
		pt.y = QHY5LII_HEIGHT_B2;
		device_formats[0].frame_table[ i ].size =  pt;
		device_formats[0].frame_table[ i ].fps_table[ 0 ] = time_fract::mk_fps( 2, 1 );
		device_formats[0].frame_table[ i ].fps_table[ 1 ] = time_fract::mk_fps( 1, 1 );
		device_formats[0].frame_table[ i ].fps_table[ 2 ] = time_fract::mk_fps( 1, 2 );
		i++;

		pt.x = QHY5LII_WIDTH_B1;
		pt.y = QHY5LII_HEIGHT_B1;
		device_formats[0].frame_table[ i ].size =  pt;
		device_formats[0].frame_table[ i ].fps_table[ 0 ] = time_fract::mk_fps( 2, 1 );
		device_formats[0].frame_table[ i ].fps_table[ 1 ] = time_fract::mk_fps( 1, 1 );
		device_formats[0].frame_table[ i ].fps_table[ 2 ] = time_fract::mk_fps( 1, 2 );
		i++;
	}
	else
	if( m_dev_type == DEVICETYPE_QHY5II )
	{
		pt.x = QHY5II_WIDTH_B3;
		pt.y = QHY5II_HEIGHT_B3;
		device_formats[0].frame_table[ i ].size =  pt;
		device_formats[0].frame_table[ i ].fps_table[ 0 ] = time_fract::mk_fps( 4, 1 );
		device_formats[0].frame_table[ i ].fps_table[ 1 ] = time_fract::mk_fps( 2, 1 );
		device_formats[0].frame_table[ i ].fps_table[ 2 ] = time_fract::mk_fps( 1, 1 );
		device_formats[0].frame_table[ i ].fps_table[ 3 ] = time_fract::mk_fps( 1, 2 );
		device_formats[0].frame_table[ i ].fps_table[ 4 ] = time_fract::mk_fps( 1, 3 );
		device_formats[0].frame_table[ i ].fps_table[ 5 ] = time_fract::mk_fps( 1, 4 );
		device_formats[0].frame_table[ i ].fps_table[ 6 ] = time_fract::mk_fps( 1, 5 );
		device_formats[0].frame_table[ i ].fps_table[ 7 ] = time_fract::mk_fps( 1, 10 );
		i++;

		pt.x = QHY5II_WIDTH_B2;
		pt.y = QHY5II_HEIGHT_B2;
		device_formats[0].frame_table[ i ].size =  pt;
		device_formats[0].frame_table[ i ].fps_table[ 0 ] = time_fract::mk_fps( 4, 1 );
		device_formats[0].frame_table[ i ].fps_table[ 1 ] = time_fract::mk_fps( 2, 1 );
		device_formats[0].frame_table[ i ].fps_table[ 2 ] = time_fract::mk_fps( 1, 1 );
		device_formats[0].frame_table[ i ].fps_table[ 3 ] = time_fract::mk_fps( 1, 2 );
		device_formats[0].frame_table[ i ].fps_table[ 4 ] = time_fract::mk_fps( 1, 3 );
		device_formats[0].frame_table[ i ].fps_table[ 5 ] = time_fract::mk_fps( 1, 4 );
		device_formats[0].frame_table[ i ].fps_table[ 6 ] = time_fract::mk_fps( 1, 5 );
		device_formats[0].frame_table[ i ].fps_table[ 7 ] = time_fract::mk_fps( 1, 10 );
		i++;

		pt.x = QHY5II_WIDTH_B1;
		pt.y = QHY5II_HEIGHT_B1;
		device_formats[0].frame_table[ i ].size =  pt;
		device_formats[0].frame_table[ i ].fps_table[ 0 ] = time_fract::mk_fps( 4, 1 );
		device_formats[0].frame_table[ i ].fps_table[ 1 ] = time_fract::mk_fps( 2, 1 );
		device_formats[0].frame_table[ i ].fps_table[ 2 ] = time_fract::mk_fps( 1, 1 );
		device_formats[0].frame_table[ i ].fps_table[ 3 ] = time_fract::mk_fps( 1, 2 );
		device_formats[0].frame_table[ i ].fps_table[ 4 ] = time_fract::mk_fps( 1, 3 );
		device_formats[0].frame_table[ i ].fps_table[ 5 ] = time_fract::mk_fps( 1, 4 );
		device_formats[0].frame_table[ i ].fps_table[ 6 ] = time_fract::mk_fps( 1, 5 );
		device_formats[0].frame_table[ i ].fps_table[ 7 ] = time_fract::mk_fps( 1, 10 );
		i++;
	}

	// add empty tail
	pt.x = pt.y = 0;
	device_formats[0].frame_table[ i++ ].size = pt;

	if( enum_controls() )
	{
		log_e("Unable to enumerate controls");
		return EXIT_FAILURE;
	}

 return 0;
}


int cvideo_qhy5ii::set_control( unsigned int control_id, const param_val_t &val )
{
	int ret = EXIT_FAILURE;

	switch( control_id )
	{
	case V4L2_CID_GAIN:
	{
		int v = val.values[0];
		if( v < 0 ) v = 0;
		if( v > 100 ) v = 100;
		ret = set_gain( v );
		if( ret != EXIT_SUCCESS )
		{
			log_e( "cvideo_qhy5ii::set_control(): set_params() failed." );
			return -1;
		}
		capture_params.gain = v;
	}
		break;
	case V4L2_CID_EXPOSURE:
	{
		int max=255;
		if (m_transfer_bit == 16) max = 65535;
		int v = val.values[0];
		if( v < 0 ) v = 0;
		if( v > max ) v = max;
		int top = max - v;
		if( top <= 0 )
		{
			log_e( "cvideo_qhy5ii::set_control(): invalid exposure" );
			return -1;
		}
		init_lut_to8bit( top );

		capture_params.exposure = v;
	}
		break;
	case V4L2_CID_RED_BALANCE:
	{
		if( val.values[0] < 0 || val.values[0] > 255 )
		{
			log_e( "%s: invalid R-balance coeff.", __FUNCTION__ );
			return -1;
		}

		m_wbred = val.values[0];
		capture_params.ext_params[ control_id ] = val.values[0];
		ret = set_gain( capture_params.gain );
		if( ret != EXIT_SUCCESS )
		{
			log_e( "%s: set_gain(): failed.", __FUNCTION__ );
			return -1;
		} else {
			log_i( "R-balance changed to: %d", val.values[0] );
		}
	}
		break;
	case V4L2_CID_BLUE_BALANCE:
	{
		if( val.values[0] < 0 || val.values[0] > 255 )
		{
			log_e( "%s: invalid B-balance coeff.", __FUNCTION__ );
			return -1;
		}

		m_wbblue = val.values[0];
		capture_params.ext_params[ control_id ] = val.values[0];
		ret = set_gain( capture_params.gain );
		if( ret != EXIT_SUCCESS )
		{
			log_e( "%s: set_gain(): failed.",__FUNCTION__ );
			return -1;
		} else {
			log_i( "B-balance changed to: %d", val.values[0] );
		}
	}
		break;
	case V4L2_CID_USER_8BIT:
	{
		capture_params.ext_params[ control_id ] = val.values[0];
		log_i( "Ctrl 8-bit: %d, (restart is required)", val.values[0] );
		if( val.values[0] == 0 ) // mode 12-bit
			log_i( "Note! 12-bit video mode disables guider port." );
	}
		break;
	case V4L2_CID_USER_USB_TRAF:
	{
		int v = val.values[0];
		if( v < 1 || v > 255 )
			log_e( "%s: invalid USB traf value", __FUNCTION__ );
		v = v < 1 ? 1 : v;
		v = v > 255 ? 255 : v;
		capture_params.ext_params[ control_id ] = v;
		m_usb_traf = v;
		set_usb_traffic( m_usb_traf );
		log_i( "USB traf: %d", v );
	}
		break;
	case V4L2_CID_USER_DSPEED:
	{
		int v = val.values[0];
		if( v < 0 || v > 1 )
			log_e( "%s: invalid download speed value", __FUNCTION__ );
		v = v < 0 ? 0 : v;
		v = v > 1 ? 1 : v;
		capture_params.ext_params[ control_id ] = v;
		m_transfer_speed = v;
		set_speed( (m_transfer_speed != 0) );
		log_i( "Download speed: %d", v );
	}
		break;
	case V4L2_CID_USER_DODEBAYER:
	{
		int v = val.values[0];
		v = v < 0 ? 0 : v;
		v = v > 1 ? 1 : v;
		capture_params.ext_params[ control_id ] = v;
		m_do_debayer = (v == 1);
		capture_params.pixel_format = get_pix_fmt();
		log_i( "Debayering is %s", m_do_debayer ? "ON" : "OFF" );
	}
		break;
	default:
		return -1;
	}
	return 0;
}


int cvideo_qhy5ii::get_control( unsigned int control_id, param_val_t *val )
{
	switch( control_id )
	{
	case V4L2_CID_GAIN:
	{
		val->values[0] = capture_params.gain;
		break;
	}
	case V4L2_CID_EXPOSURE:
	{
		val->values[0] = capture_params.exposure;
		break;
	}
	case V4L2_CID_RED_BALANCE:
	case V4L2_CID_BLUE_BALANCE:
	case V4L2_CID_USER_8BIT:
	case V4L2_CID_USER_USB_TRAF:
	case V4L2_CID_USER_DSPEED:
	{
		val->values[0] = capture_params.ext_params[ control_id ];
		break;
	}
	default:
		return -1;
	}
	return 0;
}


int cvideo_qhy5ii::init_device( void )
{
	// set desired size
	int sizeimage = set_format();
	if( sizeimage <= 0 )
		return EXIT_FAILURE;

	/*
	m_usb_traf = 30;
	if( m_dev_type == DEVICETYPE_QHY5LII )
	{
		#ifdef __arm__
			log_i("System CPU is ARM");
			m_usb_traf = 40;
		#else
			log_i("System CPU is not ARM");
			m_usb_traf = 5;
		#endif
	}
	else
	if( m_dev_type == DEVICETYPE_QHY5II )
	{
		#ifdef __arm__
			log_i("System CPU is ARM");
			m_usb_traf = 150;
		#else
			log_i("System CPU is not ARM");
			m_usb_traf = 30;
		#endif
	}
	*/
	m_usb_traf = m_usb_traf < 1 ? 1 : m_usb_traf;
	m_usb_traf = m_usb_traf > 255 ? 255 : m_usb_traf;
	m_transfer_speed = m_transfer_speed < 0 ? 0 : m_transfer_speed;
	m_transfer_speed = m_transfer_speed > 1 ? 1 : m_transfer_speed;

	int tmp_transfer_bit = m_transfer_bit;
	set_transfer_bit( 8 );
	set_speed( (m_transfer_speed != 0) );
	/*
	#ifdef __arm__
		set_speed( false );
	#else
		set_speed( true );
	#endif
	*/
	SetQHY5LIIHDR(false);
	set_resolution( capture_params.width, capture_params.height );
	set_gain( capture_params.gain );
	set_usb_traffic( m_usb_traf );
	set_fps( capture_params.fps );
	start_video_mode();
	// The camera fails when it starts for the first time after connect in 12bit mode.
	// So intitialize it in 8bit and switch to 16 bit. This breaks the exposure times
	// and reinitialization of the exposure time is needed.
	if (tmp_transfer_bit == 16) {
		set_transfer_bit( 16 );
		log_i("Camera is in pseudo 16bit mode");
		stop_video_mode();
		set_fps( capture_params.fps );
		start_video_mode();
		set_exposure( capture_params.exposure );
	} else {
		set_exposure( capture_params.exposure );
		log_i("Camera is in 8bit mode");
	}

	m_data_size = m_width * m_height * (m_transfer_bit >> 3);

	n_buffers = 1;
	buffers = (buffer *)calloc( n_buffers, sizeof(*buffers) );

	if( !buffers )
	{
		log_e( "Out of memory %s, %s", __FUNCTION__, __LINE__ );
		return EXIT_FAILURE;
	}

	buffers[0].length = m_data_size;
	buffers[0].start.ptr = malloc( buffers[0].length + 4096 );
	if( !buffers[0].start.ptr )
	{
		log_e( "Out of memory %s, %s", __FUNCTION__, __LINE__ );
		free( buffers );
		return EXIT_FAILURE;
	}

	get_autogain();
	get_gain();
	get_exposure();

	return EXIT_SUCCESS;
}


int cvideo_qhy5ii::uninit_device( void )
{
	stop_video_mode();

	if( buffers )
	{
		for( int i = 0;i < (int)n_buffers;i++ )
		{
			if( buffers[i].start.ptr )
				free( buffers[i].start.ptr );
		}
		free( buffers );
		buffers = NULL;

	}

	return EXIT_SUCCESS;
}


int cvideo_qhy5ii::start_capturing( void )
{
	return EXIT_SUCCESS;
}


int cvideo_qhy5ii::stop_capturing( void )
{
	return EXIT_SUCCESS;
}


int cvideo_qhy5ii::read_frame( void )
{
	ctimer tm;
	tm.start();

	int ret = m_qhy5ii_obj->get_frame( buffers[0].start.ptr8, m_data_size, frame_delay );
	(void)ret;
	if( m_transfer_bit == 16 && m_dev_type == DEVICETYPE_QHY5LII )
		SWIFT_MSBLSBQHY5LII( buffers[0].start.ptr8 );

	if( DBG_VERBOSITY )
		log_i( "frame time: %ldms", tm.gettime() );

	// synchronize data with GUI
	void *ptr =  buffers[0].start.ptr8;

	//log_i( "DATA: %d %d %d", *((uint16_t*)ptr+10), *((uint16_t*)ptr+7000), *((uint16_t*)ptr+10000) );

	emit renderImage( ptr, buffers[0].length );

	pthread_mutex_lock( &cv_mutex );
	while( !data_thread_flag )
		pthread_cond_wait( &cv, &cv_mutex );
	data_thread_flag = 0;
	pthread_mutex_unlock( &cv_mutex );

	return EXIT_SUCCESS;
}


unsigned int cvideo_qhy5ii::get_pix_fmt( void )
{
	if (m_transfer_bit == 16) {
		if (m_dev_type == DEVICETYPE_QHY5LII && m_is_color && m_do_debayer) {
			return V4L2_PIX_FMT_SGRBG12;
		} else {
			return V4L2_PIX_FMT_Y16;
		}
	} else if (m_transfer_bit == 8){
		if (m_dev_type == DEVICETYPE_QHY5LII && m_is_color && m_do_debayer) {
			return V4L2_PIX_FMT_SGRBG8;
		} else {
			return V4L2_PIX_FMT_GREY;
		}
	}
	return 0;
}


int cvideo_qhy5ii::set_format( void )
{
	int i, j;
	point_t pt = {0, 0};

	capture_params.pixel_format = get_pix_fmt();

	for( i = 0; i < MAX_FMT && device_formats[i].format;i++ )
	{
		if( device_formats[i].format != capture_params.pixel_format )
			continue;
		for( j = 0;j < MAX_FMT && device_formats[i].frame_table[j].size.x;j++ )
		{
			if( device_formats[i].frame_table[j].size.x == (int)capture_params.width &&
					device_formats[i].frame_table[j].size.y == (int)capture_params.height )
			{
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

	return capture_params.width * capture_params.height * sizeof(unsigned short);
}


int cvideo_qhy5ii::enum_controls( void )
{
	int n = 0;
	struct v4l2_queryctrl queryctrl;

	memset( &queryctrl, 0, sizeof(v4l2_queryctrl) );

	// create virtual control
	queryctrl.id = V4L2_CID_GAIN;
	queryctrl.type = V4L2_CTRL_TYPE_INTEGER;
	snprintf( (char*)queryctrl.name, sizeof(queryctrl.name)-1, "gain" );
	queryctrl.minimum = 0;
	queryctrl.maximum = 100;
	queryctrl.step = 1;
	queryctrl.default_value = 1;
	queryctrl.flags = 0;
	// Add control to control list
	controls = add_control( -1, &queryctrl, controls, &n );

	// create virtual control
	queryctrl.id = V4L2_CID_EXPOSURE;
	queryctrl.type = V4L2_CTRL_TYPE_INTEGER;
	snprintf( (char*)queryctrl.name, sizeof(queryctrl.name)-1, "exposure" );
	queryctrl.minimum = 0;
	if (m_transfer_bit == 16)
		queryctrl.maximum = 65535;
	else
		queryctrl.maximum = 255;
	queryctrl.step = 1;
	queryctrl.default_value = 0;
	queryctrl.flags = 0;
	// Add control to control list
	controls = add_control( -1, &queryctrl, controls, &n );

	// create virtual control
	queryctrl.id = V4L2_CID_USER_8BIT;
	queryctrl.type = V4L2_CTRL_TYPE_BOOLEAN;
	snprintf( (char*)queryctrl.name, sizeof(queryctrl.name)-1, "8-bit" );
	queryctrl.minimum = 0;
	queryctrl.maximum = 1;
	queryctrl.step = 1;
	queryctrl.default_value = 0;
	queryctrl.flags = 0;
	// Add control to control list
	controls = add_control( -1, &queryctrl, controls, &n, true );

	// create virtual control
	queryctrl.id = V4L2_CID_USER_USB_TRAF;
	queryctrl.type = V4L2_CTRL_TYPE_INTEGER;
	snprintf( (char*)queryctrl.name, sizeof(queryctrl.name)-1, "USB traf" );
	queryctrl.minimum = 1;
	queryctrl.maximum = 255;
	queryctrl.step = 1;
	if( m_dev_type == DEVICETYPE_QHY5LII )
	{
		#ifdef __arm__
			log_i("System CPU is ARM");
			queryctrl.default_value = 40;
		#else
			log_i("System CPU is not ARM");
			queryctrl.default_value = 5;
		#endif
	}
	else
	if( m_dev_type == DEVICETYPE_QHY5II )
	{
		#ifdef __arm__
			log_i("System CPU is ARM");
			queryctrl.default_value = 150;
		#else
			log_i("System CPU is not ARM");
			queryctrl.default_value = 30;
		#endif
	}

	queryctrl.flags = 0;
	// Add control to control list
	controls = add_control( -1, &queryctrl, controls, &n, true );

	// create virtual control
	queryctrl.id = V4L2_CID_USER_DSPEED;
	queryctrl.type = V4L2_CTRL_TYPE_BOOLEAN;
	snprintf( (char*)queryctrl.name, sizeof(queryctrl.name)-1, "Speed" );
	queryctrl.minimum = 0;
	queryctrl.maximum = 1;
	queryctrl.step = 1;
	queryctrl.default_value = 0;
	queryctrl.flags = 0;
	// Add control to control list
	controls = add_control( -1, &queryctrl, controls, &n, true );

	if( m_is_color )
	{
		// create virtual control (extended ctl)
		queryctrl.id = V4L2_CID_RED_BALANCE;
		queryctrl.type = V4L2_CTRL_TYPE_INTEGER;
		snprintf( (char*)queryctrl.name, sizeof(queryctrl.name)-1, "red balance" );
		queryctrl.minimum = 0;
		queryctrl.maximum = 255; //100
		queryctrl.step = 1;
		queryctrl.default_value = m_wbred;
		queryctrl.flags = 0;
		// Add control to control list
		controls = add_control( -1, &queryctrl, controls, &n, true );

		// create virtual control (extended ctl)
		queryctrl.id = V4L2_CID_BLUE_BALANCE;
		queryctrl.type = V4L2_CTRL_TYPE_INTEGER;
		snprintf( (char*)queryctrl.name, sizeof(queryctrl.name)-1, "blue balance" );
		queryctrl.minimum = 0;
		queryctrl.maximum = 255; //100
		queryctrl.step = 1;
		queryctrl.default_value = m_wbblue;
		queryctrl.flags = 0;
		// Add control to control list
		controls = add_control( -1, &queryctrl, controls, &n, true );

		// create virtual control (extended ctl)
		queryctrl.id = V4L2_CID_USER_DODEBAYER;
		queryctrl.type = V4L2_CTRL_TYPE_BOOLEAN;
		snprintf( (char*)queryctrl.name, sizeof(queryctrl.name)-1, "do debayer" );
		queryctrl.minimum = 0;
		queryctrl.maximum = 1;
		queryctrl.step = 1;
		queryctrl.default_value = m_do_debayer;
		queryctrl.flags = 0;
		// Add control to control list
		controls = add_control( -1, &queryctrl, controls, &n, true );
	}

	num_controls = n;

	return 0;
}

//--------------------------------------------------------------------------------------

void cvideo_qhy5ii::set_resolution( int x, int y )
{
	m_qhy5ii_obj->lock();

    if( m_dev_type == DEVICETYPE_QHY5LII )
    	CorrectQHY5LIIWH( &x, &y );
    else
    if( m_dev_type == DEVICETYPE_QHY5II )
    	CorrectQHY5IIWH( &x, &y );

    m_qhy5ii_obj->unlock();

    m_width = x;
    m_height = y;

    capture_params.width = m_width;
    capture_params.height = m_height;

    set_exposure_time( time_fract::to_msecs(capture_params.fps) );
    set_gain( capture_params.gain );
    set_usb_traffic( m_usb_traf );
}


void cvideo_qhy5ii::set_speed( bool isHigh )
{
	m_qhy5ii_obj->lock();

	//isHigh ? transferspeed = 1 : transferspeed = 0;
	if( isHigh )
		m_transfer_speed = 1;
	else
		m_transfer_speed = 0;

	if( m_dev_type == DEVICETYPE_QHY5LII || m_dev_type == DEVICETYPE_QHY5II )
	{
		if( isHigh )
		{
			if( m_transfer_bit == 16 )
				SetSpeedQHY5LII(1);
			else
				SetSpeedQHY5LII(2);
		}
		else
		{
			if( m_transfer_bit == 16 )
				SetSpeedQHY5LII(0);
			else
				SetSpeedQHY5LII(1);
		}
	}

	m_qhy5ii_obj->unlock();
}


void cvideo_qhy5ii::set_usb_traffic( int i )
{
	m_qhy5ii_obj->lock();

	if( m_dev_type == DEVICETYPE_QHY5II )
		I2CTwoWrite(0x05, 0x0009 + i*50);
	else
	if( m_dev_type == DEVICETYPE_QHY5LII )
	{
		if( capture_params.width == 1280 )
			I2CTwoWrite(0x300c, 1650 + i*50);
		else
			I2CTwoWrite(0x300c, 1388 + i*50);
	}

	m_qhy5ii_obj->unlock();
}


void cvideo_qhy5ii::set_transfer_bit( int Bit )
{
	if( m_dev_type == DEVICETYPE_QHY5LII )
	{
		m_transfer_bit = Bit;
		if( Bit == 16 )
			Set14Bit(1);
		else
			Set14Bit(0);
		set_speed( (m_transfer_speed != 0) );
	}
	else
		m_transfer_bit = 8;
}


int cvideo_qhy5ii::set_exposure_time( double exptime )
{
	m_qhy5ii_obj->lock();

	bool err = false;
	if( m_dev_type == DEVICETYPE_QHY5LII )
        SetExposureTime_QHY5LII( exptime );
    else
    if( m_dev_type == DEVICETYPE_QHY5II )
        SetExposureTime_QHY5II( exptime );
    else
    	err = true;

	m_qhy5ii_obj->unlock();

	return err? EXIT_FAILURE : EXIT_SUCCESS;
}


int cvideo_qhy5ii::set_gain( unsigned short gain )
{
	/* Ugly fix: during the long exposures setting gain often freezes the camera.
	stopping video mode before gain setup seems to fix this */
	stop_video_mode();

	int ret = EXIT_FAILURE;
	/* TO BE CLARIFIED: set_gain strange cludge to fix XXXX issuses on ARM MK808,
	though this is not needed on x86. */
#ifdef __arm__
	if( m_dev_type == DEVICETYPE_QHY5LII )
	{
		m_qhy5ii_obj->lock();
		CorrectQHY5LIIWH( &m_width, &m_height );
		m_qhy5ii_obj->unlock();

		set_exposure_time( time_fract::to_msecs(capture_params.fps) );
		ret = set_gain_core( gain );
		set_usb_traffic( m_usb_traf );
	}
	else
	{
		ret = set_gain_core( gain );
	}
#else
	ret = set_gain_core( gain );
#endif
	start_video_mode();

	return ret;
}


double cvideo_qhy5ii::get_temperature( void )
{
	if( m_dev_type != DEVICETYPE_QHY5LII )
		return 0;

	double slope;
	double T0;
	uint16_t sensed, calib1, calib2;
	// start measuring
	I2CTwoWrite(0x30B4, 0x0011);
	// reading the calibration params gives just enough time
	calib1 = I2CTwoRead(0x30C6);
	calib2 = I2CTwoRead(0x30C8);
	slope = (70.0 - 55.0)/(calib1 - calib2);
	T0 = (slope*calib1 - 70.0);
	// stop measuring
	I2CTwoWrite(0x30B4, 0x0000);

	sensed = I2CTwoRead(0x30B2);

//	printf("calib1 = 0x%x\n", calib1);
//	printf("calib2 = 0x%x\n", calib2);
//	printf("sensed = 0x%x\n", sensed);
//	printf("slope %f\n", slope);
//	printf("T0 %f\n", T0);
	log_i( "Temperature %f", slope * sensed - T0 );

	return slope * sensed - T0;
}


void cvideo_qhy5ii::set_wb_blue( int blue )
{
    m_wbred = blue;
    if( m_dev_type == DEVICETYPE_QHY5LII )
        SetGainColorQHY5LII( capture_params.gain*10, m_wbred, m_wbblue );
}


void cvideo_qhy5ii::set_wb_green( int green )
{
	(void)green;
//    m_wbgreen = green;
//    if( m_dev_type == DEVICETYPE_QHY5LII )
//        SetGainColorQHY5LII( capture_params.gain*10, m_wbred, m_wbblue );
}


void cvideo_qhy5ii::set_wb_red( int red )
{
    m_wbred = red;
    if( m_dev_type == DEVICETYPE_QHY5LII )
        SetGainColorQHY5LII( capture_params.gain*10, m_wbred, m_wbblue );
}









//----------------------------------------------------------------------
void cvideo_qhy5ii::SetQHY5LIIGain( unsigned short gain )
{
	gain *= 10; // gain 0-100  => 0 - 1000

	if( m_QCam5LII.CheckBoxQHY5LIILoneExpMode )
	{
		SetExposureTime_QHY5LII( 1 );
		if( m_is_color )
		{
			double RG,BG;
			RG = (double)m_wbred / 100;
			BG = (double)m_wbblue / 100;
			SetGainColorQHY5LII( gain, RG, BG );
		}
		else
		{
			SetGainMonoQHY5LII( gain );
		}
		usleep(500000);
		SetExposureTime_QHY5LII( time_fract::to_msecs( capture_params.fps ) );
	}
	else
	{
		if( m_is_color )
		{
			double RG,BG;
			RG = (double)m_wbred / 100;
			BG = (double)m_wbblue / 100;
			SetGainColorQHY5LII( gain, RG, BG );
		}
		else
		{
			SetGainMonoQHY5LII( gain );
		}
	}
}


void cvideo_qhy5ii::SetSpeedQHY5LII( unsigned char i )
{
    // i=0,1,2    0=12M  1=24M  2=48M
    unsigned char buf[2] = { i, 0 };
    //buf[0] = i;

    m_qhy5ii_obj->ctrl_msg( QHYCCD_REQUEST_WRITE, 0xc8, 0x00, 0x00, buf, 1 );
}


void cvideo_qhy5ii::SWIFT_MSBLSBQHY5LII( unsigned char *ImgData )
{
	unsigned int i = 0, val = 0;
	while( i < capture_params.width*capture_params.height*2 )
	{
		val = ((ImgData[i + 1] + (ImgData[i] << 4)) << 4);
		ImgData[i + 1] = val >> 8;
		ImgData[i] = val - ImgData[i + 1];
		i += 2;
	}
}


double cvideo_qhy5ii::setQHY5LREG_PLL(unsigned char clk)
{
	double i = 0;

	if (clk == 0)
	{

		I2CTwoWrite(0x302A, 14); // DIV           14
		I2CTwoWrite(0x302C, 1); // DIV
		I2CTwoWrite(0x302E, 3); // DIV
		I2CTwoWrite(0x3030, 42); // MULTI          44

		I2CTwoWrite(0x3082, 0x0029);
		// OPERATION_MODE_CTRL

		if( m_QCam5LII.CheckBoxQHY5LIILoneExpMode )
		{
			I2CTwoWrite(0x30B0, 0x5330);
			// DIGITAL_TEST    5370: PLL BYPASS   1370  USE PLL
			i = 1.0;
		}
		else {
			I2CTwoWrite(0x30B0, 0x1330);
			i = 1.0;
		}
		// 5330
		I2CTwoWrite(0x305e, 0x00ff); // gain
		I2CTwoWrite(0x3012, 0x0020);
		// coarse integration time

		I2CTwoWrite(0x3064, 0x1802);
		// EMBEDDED_DATA_CTRL

		return i;

	}

	if (clk == 1) {

		I2CTwoWrite(0x302A, 14); // DIV           14
		I2CTwoWrite(0x302C, 1); // DIV
		I2CTwoWrite(0x302E, 3); // DIV
		I2CTwoWrite(0x3030, 65); // MULTI          44

		I2CTwoWrite(0x3082, 0x0029);
		// OPERATION_MODE_CTRL

		if (m_QCam5LII.CheckBoxQHY5LIILoneExpMode) {
			I2CTwoWrite(0x30B0, 0x5330);
			// DIGITAL_TEST    5370: PLL BYPASS   1370  USE PLL
			i = 1.0;
		}
		else {
			I2CTwoWrite(0x30B0, 0x1330);
			i = (double)65.0 / 14.0 / 3.0;
		}
		I2CTwoWrite(0x305e, 0x00ff); // gain
		I2CTwoWrite(0x3012, 0x0020);
		// coarse integration time

		I2CTwoWrite(0x3064, 0x1802);
		// EMBEDDED_DATA_CTRL

		return i;

	}

	if (clk == 2) {

		I2CTwoWrite(0x302A, 14); // DIV           14
		I2CTwoWrite(0x302C, 1); // DIV
		I2CTwoWrite(0x302E, 3); // DIV
		I2CTwoWrite(0x3030, 57); // MULTI          44

		I2CTwoWrite(0x3082, 0x0029);
		// OPERATION_MODE_CTRL

		if (m_QCam5LII.CheckBoxQHY5LIILoneExpMode) {
			I2CTwoWrite(0x30B0, 0x5330);
			// DIGITAL_TEST    5370: PLL BYPASS   1370  USE PLL
			i = 1.0;
		}
		else {
			I2CTwoWrite(0x30B0, 0x1330);
			i = (double)57.0 / 14.0 / 3.0;
		}

		I2CTwoWrite(0x305e, 0x00ff); // gain
		I2CTwoWrite(0x3012, 0x0020);
		// coarse integration time

		I2CTwoWrite(0x3064, 0x1802);
		// EMBEDDED_DATA_CTRL

		return i;

	}
	return i;
}


void cvideo_qhy5ii::SetQHY5LIIHDR(bool on)
{
	if (on)
		I2CTwoWrite(0x3082, 0x0028);
	else
		I2CTwoWrite(0x3082, 0x0001);
}


void cvideo_qhy5ii::SetGainMonoQHY5LII(double gain)
{
	// gain input range 0-1000
	int Gain_Min = 0;
	int Gain_Max = 796;

	gain = (Gain_Max - Gain_Min) * gain / 1000;

	gain = gain / 10; // range:0-39.8

	unsigned short REG30B0;

	if (m_QCam5LII.CheckBoxQHY5LIILoneExpMode)
		REG30B0 = 0X5330;
	else
		REG30B0 = 0X1330;

	unsigned short baseDGain;

	double C[8] = {10, 8, 5, 4, 2.5, 2, 1.25, 1};
	double S[8] = {0, 0, 0, 0, 0, 0, 0, 0};
	int A[8] = {0, 0, 0, 0, 0, 0, 0, 0};
	int B[8] = {0, 0, 0, 0, 0, 0, 0, 0};

	double Error[8];

        int i;
	for (i = 0; i < 8; i++)
        {
		S[i] = gain / C[i];
		A[i] = (int)(S[i]);
		B[i] = (int)((S[i] - A[i]) / 0.03125);
		if (A[i] > 7)
			A[i] = 10000;
		if (A[i] == 0)
			A[i] = 10000;
		Error[i] = fabs(((double)(A[i])+(double)(B[i]) * 0.03125) * C[i] - gain);
	}

	double minValue;
	int minValuePosition;

	minValue = Error[0];
	minValuePosition = 0;

	for (i = 0; i < 8; i++) {

		if (minValue > Error[i]) {
			minValue = Error[i];
			minValuePosition = i;
		}
	}
	int AA, BB, CC;
	double DD;
	double EE;
	(void)EE;

	AA = A[minValuePosition];
	BB = B[minValuePosition];
	if (minValuePosition == 0) {
		CC = 8;
		DD = 1.25;
		I2CTwoWrite(0x30B0, (REG30B0 &~0x0030) + 0x30);
		I2CTwoWrite(0x3EE4, 0XD308);
	}
	if (minValuePosition == 1) {
		CC = 8;
		DD = 1;
		I2CTwoWrite(0x30B0, (REG30B0 &~0x0030) + 0x30);
		I2CTwoWrite(0x3EE4, 0XD208);
	}
	if (minValuePosition == 2) {
		CC = 4;
		DD = 1.25;
		I2CTwoWrite(0x30B0, (REG30B0 &~0x0030) + 0x20);
		I2CTwoWrite(0x3EE4, 0XD308);
	}
	if (minValuePosition == 3) {
		CC = 4;
		DD = 1;
		I2CTwoWrite(0x30B0, (REG30B0 &~0x0030) + 0x20);
		I2CTwoWrite(0x3EE4, 0XD208);
	}
	if (minValuePosition == 4) {
		CC = 2;
		DD = 1.25;
		I2CTwoWrite(0x30B0, (REG30B0 &~0x0030) + 0x10);
		I2CTwoWrite(0x3EE4, 0XD308);
	}
	if (minValuePosition == 5) {
		CC = 2;
		DD = 1;
		I2CTwoWrite(0x30B0, (REG30B0 &~0x0030) + 0x10);
		I2CTwoWrite(0x3EE4, 0XD208);
	}
	if (minValuePosition == 6) {
		CC = 1;
		DD = 1.25;
		I2CTwoWrite(0x30B0, (REG30B0 &~0x0030) + 0x00);
		I2CTwoWrite(0x3EE4, 0XD308);
	}
	if (minValuePosition == 7) {
		CC = 1;
		DD = 1;
		I2CTwoWrite(0x30B0, (REG30B0 &~0x0030) + 0x00);
		I2CTwoWrite(0x3EE4, 0XD208);
	}

	EE = fabs(((double)(AA)+(double)(BB) * 0.03125) * CC * DD - gain);

	baseDGain = BB + AA * 32;
	I2CTwoWrite(0x305E, baseDGain);

}


void cvideo_qhy5ii::SetGainColorQHY5LII(double gain, double RG, double BG)
{
	// gain input range 0-1000
	int Gain_Min = 0;
	int Gain_Max = 398;

	if (gain < 26)
		gain = 26;

	gain = (Gain_Max - Gain_Min) * gain / 1000;

	gain = gain / 10; // range:0-39.8

	unsigned short REG30B0;

	if (m_QCam5LII.CheckBoxQHY5LIILoneExpMode)
		REG30B0 = 0x5330;
	else
		REG30B0 = 0x1330;

	unsigned short baseDGain;

	double C[8] = {10, 8, 5, 4, 2.5, 2, 1.25, 1};
	double S[8] = {0, 0, 0, 0, 0, 0, 0, 0};
	int A[8] = {0, 0, 0, 0, 0, 0, 0, 0};
	int B[8] = {0, 0, 0, 0, 0, 0, 0, 0};

	double Error[8];

        int i;
	for (i = 0; i < 8; i++)
	{
		S[i] = gain / C[i];
		A[i] = (int)(S[i]);
		B[i] = (int)((S[i] - A[i]) / 0.03125);
		if (A[i] > 3)
			A[i] = 10000;
		if (A[i] == 0)
			A[i] = 10000;
		Error[i] = fabs(((double)(A[i])+(double)(B[i]) * 0.03125) * C[i] - gain);
	}

	double minValue;
	int minValuePosition;

	minValue = Error[0];
	minValuePosition = 0;

	for (i = 0; i < 8; i++)
	{
		if (minValue > Error[i])
		{
			minValue = Error[i];
			minValuePosition = i;
		}
	}

	int AA, BB, CC;
	double DD;
	double EE;
	(void)EE;

	AA = A[minValuePosition];
	BB = B[minValuePosition];
	if (minValuePosition == 0) {
		CC = 8;
		DD = 1.25;
		I2CTwoWrite(0x30B0, (REG30B0 &~0x0030) + 0x30);
		I2CTwoWrite(0x3EE4, 0XD308);
	}
	if (minValuePosition == 1) {
		CC = 8;
		DD = 1;
		I2CTwoWrite(0x30B0, (REG30B0 &~0x0030) + 0x30);
		I2CTwoWrite(0x3EE4, 0XD208);
	}
	if (minValuePosition == 2) {
		CC = 4;
		DD = 1.25;
		I2CTwoWrite(0x30B0, (REG30B0 &~0x0030) + 0x20);
		I2CTwoWrite(0x3EE4, 0XD308);
	}
	if (minValuePosition == 3) {
		CC = 4;
		DD = 1;
		I2CTwoWrite(0x30B0, (REG30B0 &~0x0030) + 0x20);
		I2CTwoWrite(0x3EE4, 0XD208);
	}
	if (minValuePosition == 4) {
		CC = 2;
		DD = 1.25;
		I2CTwoWrite(0x30B0, (REG30B0 &~0x0030) + 0x10);
		I2CTwoWrite(0x3EE4, 0XD308);
	}
	if (minValuePosition == 5) {
		CC = 2;
		DD = 1;
		I2CTwoWrite(0x30B0, (REG30B0 &~0x0030) + 0x10);
		I2CTwoWrite(0x3EE4, 0XD208);
	}
	if (minValuePosition == 6) {
		CC = 1;
		DD = 1.25;
		I2CTwoWrite(0x30B0, (REG30B0 &~0x0030) + 0x00);
		I2CTwoWrite(0x3EE4, 0XD308);
	}
	if (minValuePosition == 7) {
		CC = 1;
		DD = 1;
		I2CTwoWrite(0x30B0, (REG30B0 &~0x0030) + 0x00);
		I2CTwoWrite(0x3EE4, 0XD208);
	}

	EE = fabs(((double)(AA)+(double)(BB) * 0.03125) * CC * DD - gain);

	baseDGain = BB + AA * 32;

	I2CTwoWrite(0x3058, (unsigned short)(baseDGain*BG));
	I2CTwoWrite(0x305a, (unsigned short)(baseDGain*RG));
	I2CTwoWrite(0x305c, baseDGain);
	I2CTwoWrite(0x3056, baseDGain);
}


void cvideo_qhy5ii::Set14Bit( unsigned char i )
{
	m_qhy5ii_obj->lock();

	unsigned char buf[2] = { i, 0 };
	//buf[0] = i;
	m_qhy5ii_obj->ctrl_msg( QHYCCD_REQUEST_WRITE, 0xcd, 0x00, 0x00, buf, 1 );

	m_qhy5ii_obj->unlock();
}


void cvideo_qhy5ii::initQHY5LII_XGA(void)
{
	log_i( "%s", __FUNCTION__ );

	int x = 1024;
	int y = 768;

	InitQHY5LIIRegs();
	m_QCam5LII.QHY5L_PLL_Ratio = (int)setQHY5LREG_PLL(0);

	int xstart = 4 + (1280 - 1024) / 2;
	int ystart = 4 + (960 - 768) / 2;
	int xsize = x - 1;
	int ysize = y - 1;

	I2CTwoWrite(0x3002, ystart); // y start
	I2CTwoWrite(0x3004, xstart); // x start
	I2CTwoWrite(0x3006, ystart + ysize); // y end
	I2CTwoWrite(0x3008, xstart + xsize); // x end
	I2CTwoWrite(0x300a, 795); // frame length
	I2CTwoWrite(0x300c, 1388); // line  length
	I2CTwoWrite(0x301A, 0x10DC); // RESET_REGISTER
}


void cvideo_qhy5ii::initQHY5LII_1280X960(void)
{
	log_i( "%s", __FUNCTION__ );

	int x = 1280;
	int y = 960;

	InitQHY5LIIRegs();
	m_QCam5LII.QHY5L_PLL_Ratio = (int)setQHY5LREG_PLL(0);

	int xstart = 4;
	int ystart = 4;
	int xsize = x - 1;
	int ysize = y - 1;

	I2CTwoWrite(0x3002, ystart); // y start
	I2CTwoWrite(0x3004, xstart); // x start
	I2CTwoWrite(0x3006, ystart + ysize); // y end
	I2CTwoWrite(0x3008, xstart + xsize); // x end
	I2CTwoWrite(0x300a, 990); // frame length
	I2CTwoWrite(0x300c, 1650); // line  length
	I2CTwoWrite(0x301A, 0x10DC); // RESET_REGISTER

}


void cvideo_qhy5ii::initQHY5LII_SVGA(void)
{
	log_i( "%s", __FUNCTION__ );

	int x = 800;
	int y = 600;

	InitQHY5LIIRegs();
    m_QCam5LII.QHY5L_PLL_Ratio = (int)setQHY5LREG_PLL(2);

	int xstart = 4 + (1280 - 800) / 2;
	int ystart = 4 + (960 - 600) / 2;
	int xsize = x - 1;
	int ysize = y - 1;

	I2CTwoWrite(0x3002, ystart); // y start
	I2CTwoWrite(0x3004, xstart); // x start
	I2CTwoWrite(0x3006, ystart + ysize); // y end
	I2CTwoWrite(0x3008, xstart + xsize); // x end
	I2CTwoWrite(0x300a, 626); // frame length
	I2CTwoWrite(0x300c, 1388); // line  length
	I2CTwoWrite(0x301A, 0x10DC); // RESET_REGISTER
}


void cvideo_qhy5ii::initQHY5LII_VGA(void)
{
	log_i( "%s", __FUNCTION__ );

	int x = 640;
	int y = 480;

	InitQHY5LIIRegs();
	m_QCam5LII.QHY5L_PLL_Ratio = (int)setQHY5LREG_PLL(1);

	int xstart = 4 + (1280 - 640) / 2;
	int ystart = 4 + (960 - 480) / 2;
	int xsize = x - 1;
	int ysize = y - 1;

	I2CTwoWrite(0x3002, ystart); // y start
	I2CTwoWrite(0x3004, xstart); // x start
	I2CTwoWrite(0x3006, ystart + ysize); // y end
	I2CTwoWrite(0x3008, xstart + xsize); // x end
	I2CTwoWrite(0x300a, 506); // frame length
	I2CTwoWrite(0x300c, 1388); // line  length
	I2CTwoWrite(0x301A, 0x10DC); // RESET_REGISTER
}


void cvideo_qhy5ii::initQHY5LII_QVGA(void)
{
	log_i( "%s", __FUNCTION__ );

	int x = 320;
	int y = 240;

	InitQHY5LIIRegs();
	m_QCam5LII.QHY5L_PLL_Ratio = (int)setQHY5LREG_PLL(1);

	int xstart = 4 + (1280 - 320) / 2; ;
	int ystart = 4 + (960 - 320) / 2; ;
	int xsize = x - 1;
	int ysize = y - 1;

	I2CTwoWrite(0x3002, ystart); // y start
	I2CTwoWrite(0x3004, xstart); // x start
	I2CTwoWrite(0x3006, ystart + ysize); // y end
	I2CTwoWrite(0x3008, xstart + xsize); // x end
	I2CTwoWrite(0x300a, 266); // frame length
	I2CTwoWrite(0x300c, 1388); // line  length
	I2CTwoWrite(0x301A, 0x10DC); // RESET_REGISTER
}


void cvideo_qhy5ii::CorrectQHY5LIIWH(int *w,int *h)
{
    if(*w <= 320 && *h <=240)
	{
		initQHY5LII_QVGA();
		*w = 320;
		*h = 240;
	}
	else if(*w <= 640 && *h <= 480)
	{
		initQHY5LII_VGA();
		*w = 640;
		*h = 480;
	}
	else if(*w <= 800 && *h <= 600)
	{
		initQHY5LII_SVGA();
		*w = 800;
		*h = 600;
	}
	else if(*w <= 1024 && *h <= 768)
	{
		initQHY5LII_XGA();
		*w = 1024;
		*h = 768;
	}
	else
	{
		initQHY5LII_1280X960();
		*w = 1280;
		*h = 960;
	}
}

void cvideo_qhy5ii::SetExposureTime_QHY5LII( uint32_t i )
{
	i *= 1000; // ms => us

	unsigned curr_expo = (unsigned)time_fract::to_msecs( capture_params.fps ) * 1000;
	if( (curr_expo < 4000 && i >= 4000) || (curr_expo >= 4000 && i < 4000) )
	{
		//CorrectQHY5LIIWH( &m_width, &m_height );
		//log_e( "Must not be called" );
	}

	if( i >= 1200000 )
	{
		m_QCam5LII.CheckBoxQHY5LIILoneExpMode = true;
	}
	else
	{
		m_QCam5LII.CheckBoxQHY5LIILoneExpMode = false;
	}
	// Required input parameters: CMOSCLK
	double CMOSCLK;

	if( m_transfer_speed == 1 )
	{
		if( m_transfer_bit == 16 )
			CMOSCLK = 24;
		else
			CMOSCLK = 48;
	}
	else
	{
		if( m_transfer_bit == 16 )
			CMOSCLK = 12;
		else
			CMOSCLK = 24;
	}

	double pixelPeriod;
	pixelPeriod = 1 / (CMOSCLK * m_QCam5LII.QHY5L_PLL_Ratio); // unit: us

	double RowTime;
	uint32_t ExpTime;
	unsigned short REG300C, REG3012;

	double MaxShortExpTime;

	if (!m_QCam5LII.CheckBoxQHY5LIILoneExpMode)
	{
		if (i > 2 * 1000 * 1000)
			i = 2 * 1000 * 1000;
	}

	REG300C = I2CTwoRead(0x300C);

	RowTime = REG300C * pixelPeriod;

	MaxShortExpTime = 65000 * RowTime;

	if( !m_QCam5LII.CheckBoxQHY5LIILoneExpMode )
	{
		if( (double)i > MaxShortExpTime )
			i = (uint32_t)MaxShortExpTime;
	}

	ExpTime = i;

	unsigned char buf[4];

	if (ExpTime > MaxShortExpTime)
	{
		I2CTwoWrite(0x3012, 65000);

		ExpTime = ExpTime - MaxShortExpTime;

		buf[0] = 0;
		buf[1] = (uint8_t)(((ExpTime / 1000) & (~0xff00ffff)) >> 16);
		buf[2] = (uint8_t)(((ExpTime / 1000) & (~0xffff00ff)) >> 8);
		buf[3] = (uint8_t)((ExpTime / 1000) & (~0xffffff00));
		m_qhy5ii_obj->ctrl_msg( QHYCCD_REQUEST_WRITE, 0xc1, 0x00, 0x00, buf, 4 );
		ExpTime = ExpTime + MaxShortExpTime;
		REG3012 = 65000;
		m_QCam5LII.longExpMode = true;
	}
	else
	{

		if( m_QCam5LII.longExpMode == true )
		{
			m_QCam5LII.longExpMode = false;
			// After switching over, it should be performed once more
			buf[0] = 0;
			buf[1] = 0;
			buf[2] = 0;
			buf[3] = 0;
			m_qhy5ii_obj->ctrl_msg( QHYCCD_REQUEST_WRITE, 0xc1, 0x00, 0x00, buf, 4 );
			usleep(100);
			REG3012 = (unsigned short)(ExpTime / RowTime);
			if (REG3012 < 1)
				REG3012 = 1;
			I2CTwoWrite(0x3012, REG3012);
			ExpTime = (uint32_t)REG3012 * RowTime;
			m_QCam5LII.longExpMode = false;

		}

		buf[0] = 0;
		buf[1] = 0;
		buf[2] = 0;
		buf[3] = 0;
		m_qhy5ii_obj->ctrl_msg( QHYCCD_REQUEST_WRITE, 0xc1, 0x00, 0x00, buf, 4 );
		usleep(100);
		REG3012 = (unsigned short)(ExpTime / RowTime);
		if (REG3012 < 1)
			REG3012 = 1;
		I2CTwoWrite(0x3012, REG3012);
		ExpTime = (uint32_t)REG3012 * RowTime;
		m_QCam5LII.longExpMode = false;
	}
}


void cvideo_qhy5ii::InitQHY5LIIRegs(void)
{
	// [720p, 25fps input27Mhz,output50Mhz, ]
	I2CTwoWrite(0x301A, 0x0001); // RESET_REGISTER
	I2CTwoWrite(0x301A, 0x10D8); // RESET_REGISTER
	usleep(100000);
	/////Linear sequencer
	I2CTwoWrite(0x3088, 0x8000); // SEQ_CTRL_PORT
	I2CTwoWrite(0x3086, 0x0025); // SEQ_DATA_PORT
	I2CTwoWrite(0x3086, 0x5050); // SEQ_DATA_PORT
	I2CTwoWrite(0x3086, 0x2D26); // SEQ_DATA_PORT
	I2CTwoWrite(0x3086, 0x0828); // SEQ_DATA_PORT
	I2CTwoWrite(0x3086, 0x0D17); // SEQ_DATA_PORT
	I2CTwoWrite(0x3086, 0x0926); // SEQ_DATA_PORT
	I2CTwoWrite(0x3086, 0x0028); // SEQ_DATA_PORT
	I2CTwoWrite(0x3086, 0x0526); // SEQ_DATA_PORT
	I2CTwoWrite(0x3086, 0xA728); // SEQ_DATA_PORT
	I2CTwoWrite(0x3086, 0x0725); // SEQ_DATA_PORT
	I2CTwoWrite(0x3086, 0x8080); // SEQ_DATA_PORT
	I2CTwoWrite(0x3086, 0x2925); // SEQ_DATA_PORT
	I2CTwoWrite(0x3086, 0x0040); // SEQ_DATA_PORT
	I2CTwoWrite(0x3086, 0x2702); // SEQ_DATA_PORT
	I2CTwoWrite(0x3086, 0x1616); // SEQ_DATA_PORT
	I2CTwoWrite(0x3086, 0x2706); // SEQ_DATA_PORT
	I2CTwoWrite(0x3086, 0x1F17); // SEQ_DATA_PORT
	I2CTwoWrite(0x3086, 0x3626); // SEQ_DATA_PORT
	I2CTwoWrite(0x3086, 0xA617); // SEQ_DATA_PORT
	I2CTwoWrite(0x3086, 0x0326); // SEQ_DATA_PORT
	I2CTwoWrite(0x3086, 0xA417); // SEQ_DATA_PORT
	I2CTwoWrite(0x3086, 0x1F28); // SEQ_DATA_PORT
	I2CTwoWrite(0x3086, 0x0526); // SEQ_DATA_PORT
	I2CTwoWrite(0x3086, 0x2028); // SEQ_DATA_PORT
	I2CTwoWrite(0x3086, 0x0425); // SEQ_DATA_PORT
	I2CTwoWrite(0x3086, 0x2020); // SEQ_DATA_PORT
	I2CTwoWrite(0x3086, 0x2700); // SEQ_DATA_PORT
	I2CTwoWrite(0x3086, 0x171D); // SEQ_DATA_PORT
	I2CTwoWrite(0x3086, 0x2500); // SEQ_DATA_PORT
	I2CTwoWrite(0x3086, 0x2017); // SEQ_DATA_PORT
	I2CTwoWrite(0x3086, 0x1028); // SEQ_DATA_PORT
	I2CTwoWrite(0x3086, 0x0519); // SEQ_DATA_PORT
	I2CTwoWrite(0x3086, 0x1703); // SEQ_DATA_PORT
	I2CTwoWrite(0x3086, 0x2706); // SEQ_DATA_PORT
	I2CTwoWrite(0x3086, 0x1703); // SEQ_DATA_PORT
	I2CTwoWrite(0x3086, 0x1741); // SEQ_DATA_PORT
	I2CTwoWrite(0x3086, 0x2660); // SEQ_DATA_PORT
	I2CTwoWrite(0x3086, 0x175A); // SEQ_DATA_PORT
	I2CTwoWrite(0x3086, 0x2317); // SEQ_DATA_PORT
	I2CTwoWrite(0x3086, 0x1122); // SEQ_DATA_PORT
	I2CTwoWrite(0x3086, 0x1741); // SEQ_DATA_PORT
	I2CTwoWrite(0x3086, 0x2500); // SEQ_DATA_PORT
	I2CTwoWrite(0x3086, 0x9027); // SEQ_DATA_PORT
	I2CTwoWrite(0x3086, 0x0026); // SEQ_DATA_PORT
	I2CTwoWrite(0x3086, 0x1828); // SEQ_DATA_PORT
	I2CTwoWrite(0x3086, 0x002E); // SEQ_DATA_PORT
	I2CTwoWrite(0x3086, 0x2A28); // SEQ_DATA_PORT
	I2CTwoWrite(0x3086, 0x081C); // SEQ_DATA_PORT
	I2CTwoWrite(0x3086, 0x1470); // SEQ_DATA_PORT
	I2CTwoWrite(0x3086, 0x7003); // SEQ_DATA_PORT
	I2CTwoWrite(0x3086, 0x1470); // SEQ_DATA_PORT
	I2CTwoWrite(0x3086, 0x7004); // SEQ_DATA_PORT
	I2CTwoWrite(0x3086, 0x1470); // SEQ_DATA_PORT
	I2CTwoWrite(0x3086, 0x7005); // SEQ_DATA_PORT
	I2CTwoWrite(0x3086, 0x1470); // SEQ_DATA_PORT
	I2CTwoWrite(0x3086, 0x7009); // SEQ_DATA_PORT
	I2CTwoWrite(0x3086, 0x170C); // SEQ_DATA_PORT
	I2CTwoWrite(0x3086, 0x0014); // SEQ_DATA_PORT
	I2CTwoWrite(0x3086, 0x0020); // SEQ_DATA_PORT
	I2CTwoWrite(0x3086, 0x0014); // SEQ_DATA_PORT
	I2CTwoWrite(0x3086, 0x0050); // SEQ_DATA_PORT
	I2CTwoWrite(0x3086, 0x0314); // SEQ_DATA_PORT
	I2CTwoWrite(0x3086, 0x0020); // SEQ_DATA_PORT
	I2CTwoWrite(0x3086, 0x0314); // SEQ_DATA_PORT
	I2CTwoWrite(0x3086, 0x0050); // SEQ_DATA_PORT
	I2CTwoWrite(0x3086, 0x0414); // SEQ_DATA_PORT
	I2CTwoWrite(0x3086, 0x0020); // SEQ_DATA_PORT
	I2CTwoWrite(0x3086, 0x0414); // SEQ_DATA_PORT
	I2CTwoWrite(0x3086, 0x0050); // SEQ_DATA_PORT
	I2CTwoWrite(0x3086, 0x0514); // SEQ_DATA_PORT
	I2CTwoWrite(0x3086, 0x0020); // SEQ_DATA_PORT
	I2CTwoWrite(0x3086, 0x2405); // SEQ_DATA_PORT
	I2CTwoWrite(0x3086, 0x1400); // SEQ_DATA_PORT
	I2CTwoWrite(0x3086, 0x5001); // SEQ_DATA_PORT
	I2CTwoWrite(0x3086, 0x2550); // SEQ_DATA_PORT
	I2CTwoWrite(0x3086, 0x502D); // SEQ_DATA_PORT
	I2CTwoWrite(0x3086, 0x2608); // SEQ_DATA_PORT
	I2CTwoWrite(0x3086, 0x280D); // SEQ_DATA_PORT
	I2CTwoWrite(0x3086, 0x1709); // SEQ_DATA_PORT
	I2CTwoWrite(0x3086, 0x2600); // SEQ_DATA_PORT
	I2CTwoWrite(0x3086, 0x2805); // SEQ_DATA_PORT
	I2CTwoWrite(0x3086, 0x26A7); // SEQ_DATA_PORT
	I2CTwoWrite(0x3086, 0x2807); // SEQ_DATA_PORT
	I2CTwoWrite(0x3086, 0x2580); // SEQ_DATA_PORT
	I2CTwoWrite(0x3086, 0x8029); // SEQ_DATA_PORT
	I2CTwoWrite(0x3086, 0x2500); // SEQ_DATA_PORT
	I2CTwoWrite(0x3086, 0x4027); // SEQ_DATA_PORT
	I2CTwoWrite(0x3086, 0x0216); // SEQ_DATA_PORT
	I2CTwoWrite(0x3086, 0x1627); // SEQ_DATA_PORT
	I2CTwoWrite(0x3086, 0x0620); // SEQ_DATA_PORT
	I2CTwoWrite(0x3086, 0x1736); // SEQ_DATA_PORT
	I2CTwoWrite(0x3086, 0x26A6); // SEQ_DATA_PORT
	I2CTwoWrite(0x3086, 0x1703); // SEQ_DATA_PORT
	I2CTwoWrite(0x3086, 0x26A4); // SEQ_DATA_PORT
	I2CTwoWrite(0x3086, 0x171F); // SEQ_DATA_PORT
	I2CTwoWrite(0x3086, 0x2805); // SEQ_DATA_PORT
	I2CTwoWrite(0x3086, 0x2620); // SEQ_DATA_PORT
	I2CTwoWrite(0x3086, 0x2804); // SEQ_DATA_PORT
	I2CTwoWrite(0x3086, 0x2520); // SEQ_DATA_PORT
	I2CTwoWrite(0x3086, 0x2027); // SEQ_DATA_PORT
	I2CTwoWrite(0x3086, 0x0017); // SEQ_DATA_PORT
	I2CTwoWrite(0x3086, 0x1D25); // SEQ_DATA_PORT
	I2CTwoWrite(0x3086, 0x0020); // SEQ_DATA_PORT
	I2CTwoWrite(0x3086, 0x1710); // SEQ_DATA_PORT
	I2CTwoWrite(0x3086, 0x2805); // SEQ_DATA_PORT
	I2CTwoWrite(0x3086, 0x1A17); // SEQ_DATA_PORT
	I2CTwoWrite(0x3086, 0x0327); // SEQ_DATA_PORT
	I2CTwoWrite(0x3086, 0x0617); // SEQ_DATA_PORT
	I2CTwoWrite(0x3086, 0x0317); // SEQ_DATA_PORT
	I2CTwoWrite(0x3086, 0x4126); // SEQ_DATA_PORT
	I2CTwoWrite(0x3086, 0x6017); // SEQ_DATA_PORT
	I2CTwoWrite(0x3086, 0xAE25); // SEQ_DATA_PORT
	I2CTwoWrite(0x3086, 0x0090); // SEQ_DATA_PORT
	I2CTwoWrite(0x3086, 0x2700); // SEQ_DATA_PORT
	I2CTwoWrite(0x3086, 0x2618); // SEQ_DATA_PORT
	I2CTwoWrite(0x3086, 0x2800); // SEQ_DATA_PORT
	I2CTwoWrite(0x3086, 0x2E2A); // SEQ_DATA_PORT
	I2CTwoWrite(0x3086, 0x2808); // SEQ_DATA_PORT
	I2CTwoWrite(0x3086, 0x1D05); // SEQ_DATA_PORT
	I2CTwoWrite(0x3086, 0x1470); // SEQ_DATA_PORT
	I2CTwoWrite(0x3086, 0x7009); // SEQ_DATA_PORT
	I2CTwoWrite(0x3086, 0x1720); // SEQ_DATA_PORT
	I2CTwoWrite(0x3086, 0x1400); // SEQ_DATA_PORT
	I2CTwoWrite(0x3086, 0x2024); // SEQ_DATA_PORT
	I2CTwoWrite(0x3086, 0x1400); // SEQ_DATA_PORT
	I2CTwoWrite(0x3086, 0x5002); // SEQ_DATA_PORT
	I2CTwoWrite(0x3086, 0x2550); // SEQ_DATA_PORT
	I2CTwoWrite(0x3086, 0x502D); // SEQ_DATA_PORT
	I2CTwoWrite(0x3086, 0x2608); // SEQ_DATA_PORT
	I2CTwoWrite(0x3086, 0x280D); // SEQ_DATA_PORT
	I2CTwoWrite(0x3086, 0x1709); // SEQ_DATA_PORT
	I2CTwoWrite(0x3086, 0x2600); // SEQ_DATA_PORT
	I2CTwoWrite(0x3086, 0x2805); // SEQ_DATA_PORT
	I2CTwoWrite(0x3086, 0x26A7); // SEQ_DATA_PORT
	I2CTwoWrite(0x3086, 0x2807); // SEQ_DATA_PORT
	I2CTwoWrite(0x3086, 0x2580); // SEQ_DATA_PORT
	I2CTwoWrite(0x3086, 0x8029); // SEQ_DATA_PORT
	I2CTwoWrite(0x3086, 0x2500); // SEQ_DATA_PORT
	I2CTwoWrite(0x3086, 0x4027); // SEQ_DATA_PORT
	I2CTwoWrite(0x3086, 0x0216); // SEQ_DATA_PORT
	I2CTwoWrite(0x3086, 0x1627); // SEQ_DATA_PORT
	I2CTwoWrite(0x3086, 0x0617); // SEQ_DATA_PORT
	I2CTwoWrite(0x3086, 0x3626); // SEQ_DATA_PORT
	I2CTwoWrite(0x3086, 0xA617); // SEQ_DATA_PORT
	I2CTwoWrite(0x3086, 0x0326); // SEQ_DATA_PORT
	I2CTwoWrite(0x3086, 0xA417); // SEQ_DATA_PORT
	I2CTwoWrite(0x3086, 0x1F28); // SEQ_DATA_PORT
	I2CTwoWrite(0x3086, 0x0526); // SEQ_DATA_PORT
	I2CTwoWrite(0x3086, 0x2028); // SEQ_DATA_PORT
	I2CTwoWrite(0x3086, 0x0425); // SEQ_DATA_PORT
	I2CTwoWrite(0x3086, 0x2020); // SEQ_DATA_PORT
	I2CTwoWrite(0x3086, 0x2700); // SEQ_DATA_PORT
	I2CTwoWrite(0x3086, 0x171D); // SEQ_DATA_PORT
	I2CTwoWrite(0x3086, 0x2500); // SEQ_DATA_PORT
	I2CTwoWrite(0x3086, 0x2021); // SEQ_DATA_PORT
	I2CTwoWrite(0x3086, 0x1710); // SEQ_DATA_PORT
	I2CTwoWrite(0x3086, 0x2805); // SEQ_DATA_PORT
	I2CTwoWrite(0x3086, 0x1B17); // SEQ_DATA_PORT
	I2CTwoWrite(0x3086, 0x0327); // SEQ_DATA_PORT
	I2CTwoWrite(0x3086, 0x0617); // SEQ_DATA_PORT
	I2CTwoWrite(0x3086, 0x0317); // SEQ_DATA_PORT
	I2CTwoWrite(0x3086, 0x4126); // SEQ_DATA_PORT
	I2CTwoWrite(0x3086, 0x6017); // SEQ_DATA_PORT
	I2CTwoWrite(0x3086, 0xAE25); // SEQ_DATA_PORT
	I2CTwoWrite(0x3086, 0x0090); // SEQ_DATA_PORT
	I2CTwoWrite(0x3086, 0x2700); // SEQ_DATA_PORT
	I2CTwoWrite(0x3086, 0x2618); // SEQ_DATA_PORT
	I2CTwoWrite(0x3086, 0x2800); // SEQ_DATA_PORT
	I2CTwoWrite(0x3086, 0x2E2A); // SEQ_DATA_PORT
	I2CTwoWrite(0x3086, 0x2808); // SEQ_DATA_PORT
	I2CTwoWrite(0x3086, 0x1E17); // SEQ_DATA_PORT
	I2CTwoWrite(0x3086, 0x0A05); // SEQ_DATA_PORT
	I2CTwoWrite(0x3086, 0x1470); // SEQ_DATA_PORT
	I2CTwoWrite(0x3086, 0x7009); // SEQ_DATA_PORT
	I2CTwoWrite(0x3086, 0x1616); // SEQ_DATA_PORT
	I2CTwoWrite(0x3086, 0x1616); // SEQ_DATA_PORT
	I2CTwoWrite(0x3086, 0x1616); // SEQ_DATA_PORT
	I2CTwoWrite(0x3086, 0x1616); // SEQ_DATA_PORT
	I2CTwoWrite(0x3086, 0x1616); // SEQ_DATA_PORT
	I2CTwoWrite(0x3086, 0x1616); // SEQ_DATA_PORT
	I2CTwoWrite(0x3086, 0x1616); // SEQ_DATA_PORT
	I2CTwoWrite(0x3086, 0x1616); // SEQ_DATA_PORT
	I2CTwoWrite(0x3086, 0x1616); // SEQ_DATA_PORT
	I2CTwoWrite(0x3086, 0x1616); // SEQ_DATA_PORT
	I2CTwoWrite(0x3086, 0x1616); // SEQ_DATA_PORT
	I2CTwoWrite(0x3086, 0x1616); // SEQ_DATA_PORT
	I2CTwoWrite(0x3086, 0x1616); // SEQ_DATA_PORT
	I2CTwoWrite(0x3086, 0x1616); // SEQ_DATA_PORT
	I2CTwoWrite(0x3086, 0x1616); // SEQ_DATA_PORT
	I2CTwoWrite(0x3086, 0x1616); // SEQ_DATA_PORT
	I2CTwoWrite(0x3086, 0x1400); // SEQ_DATA_PORT
	I2CTwoWrite(0x3086, 0x2024); // SEQ_DATA_PORT
	I2CTwoWrite(0x3086, 0x1400); // SEQ_DATA_PORT
	I2CTwoWrite(0x3086, 0x502B); // SEQ_DATA_PORT
	I2CTwoWrite(0x3086, 0x302C); // SEQ_DATA_PORT
	I2CTwoWrite(0x3086, 0x2C2C); // SEQ_DATA_PORT
	I2CTwoWrite(0x3086, 0x2C00); // SEQ_DATA_PORT
	I2CTwoWrite(0x3086, 0x0225); // SEQ_DATA_PORT
	I2CTwoWrite(0x3086, 0x5050); // SEQ_DATA_PORT
	I2CTwoWrite(0x3086, 0x2D26); // SEQ_DATA_PORT
	I2CTwoWrite(0x3086, 0x0828); // SEQ_DATA_PORT
	I2CTwoWrite(0x3086, 0x0D17); // SEQ_DATA_PORT
	I2CTwoWrite(0x3086, 0x0926); // SEQ_DATA_PORT
	I2CTwoWrite(0x3086, 0x0028); // SEQ_DATA_PORT
	I2CTwoWrite(0x3086, 0x0526); // SEQ_DATA_PORT
	I2CTwoWrite(0x3086, 0xA728); // SEQ_DATA_PORT
	I2CTwoWrite(0x3086, 0x0725); // SEQ_DATA_PORT
	I2CTwoWrite(0x3086, 0x8080); // SEQ_DATA_PORT
	I2CTwoWrite(0x3086, 0x2917); // SEQ_DATA_PORT
	I2CTwoWrite(0x3086, 0x0525); // SEQ_DATA_PORT
	I2CTwoWrite(0x3086, 0x0040); // SEQ_DATA_PORT
	I2CTwoWrite(0x3086, 0x2702); // SEQ_DATA_PORT
	I2CTwoWrite(0x3086, 0x1616); // SEQ_DATA_PORT
	I2CTwoWrite(0x3086, 0x2706); // SEQ_DATA_PORT
	I2CTwoWrite(0x3086, 0x1736); // SEQ_DATA_PORT
	I2CTwoWrite(0x3086, 0x26A6); // SEQ_DATA_PORT
	I2CTwoWrite(0x3086, 0x1703); // SEQ_DATA_PORT
	I2CTwoWrite(0x3086, 0x26A4); // SEQ_DATA_PORT
	I2CTwoWrite(0x3086, 0x171F); // SEQ_DATA_PORT
	I2CTwoWrite(0x3086, 0x2805); // SEQ_DATA_PORT
	I2CTwoWrite(0x3086, 0x2620); // SEQ_DATA_PORT
	I2CTwoWrite(0x3086, 0x2804); // SEQ_DATA_PORT
	I2CTwoWrite(0x3086, 0x2520); // SEQ_DATA_PORT
	I2CTwoWrite(0x3086, 0x2027); // SEQ_DATA_PORT
	I2CTwoWrite(0x3086, 0x0017); // SEQ_DATA_PORT
	I2CTwoWrite(0x3086, 0x1E25); // SEQ_DATA_PORT
	I2CTwoWrite(0x3086, 0x0020); // SEQ_DATA_PORT
	I2CTwoWrite(0x3086, 0x2117); // SEQ_DATA_PORT
	I2CTwoWrite(0x3086, 0x1028); // SEQ_DATA_PORT
	I2CTwoWrite(0x3086, 0x051B); // SEQ_DATA_PORT
	I2CTwoWrite(0x3086, 0x1703); // SEQ_DATA_PORT
	I2CTwoWrite(0x3086, 0x2706); // SEQ_DATA_PORT
	I2CTwoWrite(0x3086, 0x1703); // SEQ_DATA_PORT
	I2CTwoWrite(0x3086, 0x1747); // SEQ_DATA_PORT
	I2CTwoWrite(0x3086, 0x2660); // SEQ_DATA_PORT
	I2CTwoWrite(0x3086, 0x17AE); // SEQ_DATA_PORT
	I2CTwoWrite(0x3086, 0x2500); // SEQ_DATA_PORT
	I2CTwoWrite(0x3086, 0x9027); // SEQ_DATA_PORT
	I2CTwoWrite(0x3086, 0x0026); // SEQ_DATA_PORT
	I2CTwoWrite(0x3086, 0x1828); // SEQ_DATA_PORT
	I2CTwoWrite(0x3086, 0x002E); // SEQ_DATA_PORT
	I2CTwoWrite(0x3086, 0x2A28); // SEQ_DATA_PORT
	I2CTwoWrite(0x3086, 0x081E); // SEQ_DATA_PORT
	I2CTwoWrite(0x3086, 0x0831); // SEQ_DATA_PORT
	I2CTwoWrite(0x3086, 0x1440); // SEQ_DATA_PORT
	I2CTwoWrite(0x3086, 0x4014); // SEQ_DATA_PORT
	I2CTwoWrite(0x3086, 0x2020); // SEQ_DATA_PORT
	I2CTwoWrite(0x3086, 0x1410); // SEQ_DATA_PORT
	I2CTwoWrite(0x3086, 0x1034); // SEQ_DATA_PORT
	I2CTwoWrite(0x3086, 0x1400); // SEQ_DATA_PORT
	I2CTwoWrite(0x3086, 0x1014); // SEQ_DATA_PORT
	I2CTwoWrite(0x3086, 0x0020); // SEQ_DATA_PORT
	I2CTwoWrite(0x3086, 0x1400); // SEQ_DATA_PORT
	I2CTwoWrite(0x3086, 0x4013); // SEQ_DATA_PORT
	I2CTwoWrite(0x3086, 0x1802); // SEQ_DATA_PORT
	I2CTwoWrite(0x3086, 0x1470); // SEQ_DATA_PORT
	I2CTwoWrite(0x3086, 0x7004); // SEQ_DATA_PORT
	I2CTwoWrite(0x3086, 0x1470); // SEQ_DATA_PORT
	I2CTwoWrite(0x3086, 0x7003); // SEQ_DATA_PORT
	I2CTwoWrite(0x3086, 0x1470); // SEQ_DATA_PORT
	I2CTwoWrite(0x3086, 0x7017); // SEQ_DATA_PORT
	I2CTwoWrite(0x3086, 0x2002); // SEQ_DATA_PORT
	I2CTwoWrite(0x3086, 0x1400); // SEQ_DATA_PORT
	I2CTwoWrite(0x3086, 0x2002); // SEQ_DATA_PORT
	I2CTwoWrite(0x3086, 0x1400); // SEQ_DATA_PORT
	I2CTwoWrite(0x3086, 0x5004); // SEQ_DATA_PORT
	I2CTwoWrite(0x3086, 0x1400); // SEQ_DATA_PORT
	I2CTwoWrite(0x3086, 0x2004); // SEQ_DATA_PORT
	I2CTwoWrite(0x3086, 0x1400); // SEQ_DATA_PORT
	I2CTwoWrite(0x3086, 0x5022); // SEQ_DATA_PORT
	I2CTwoWrite(0x3086, 0x0314); // SEQ_DATA_PORT
	I2CTwoWrite(0x3086, 0x0020); // SEQ_DATA_PORT
	I2CTwoWrite(0x3086, 0x0314); // SEQ_DATA_PORT
	I2CTwoWrite(0x3086, 0x0050); // SEQ_DATA_PORT
	I2CTwoWrite(0x3086, 0x2C2C); // SEQ_DATA_PORT
	I2CTwoWrite(0x3086, 0x2C2C); // SEQ_DATA_PORT
	I2CTwoWrite(0x309E, 0x018A); // RESERVED_MFR_309E
	I2CTwoWrite(0x301A, 0x10D8); // RESET_REGISTER
	I2CTwoWrite(0x3082, 0x0029); // OPERATION_MODE_CTRL
	I2CTwoWrite(0x301E, 0x00C8); // DATA_PEDESTAL
	I2CTwoWrite(0x3EDA, 0x0F03); // RESERVED_MFR_3EDA
	I2CTwoWrite(0x3EDE, 0xC007); // RESERVED_MFR_3EDE
	I2CTwoWrite(0x3ED8, 0x01EF); // RESERVED_MFR_3ED8
	I2CTwoWrite(0x3EE2, 0xA46B); // RESERVED_MFR_3EE2
	I2CTwoWrite(0x3EE0, 0x067D); // RESERVED_MFR_3EE0
	I2CTwoWrite(0x3EDC, 0x0070); // RESERVED_MFR_3EDC
	I2CTwoWrite(0x3044, 0x0404); // DARK_CONTROL
	I2CTwoWrite(0x3EE6, 0x4303); // RESERVED_MFR_3EE6
	I2CTwoWrite(0x3EE4, 0xD208); // DAC_LD_24_25
	I2CTwoWrite(0x3ED6, 0x00BD); // RESERVED_MFR_3ED6
	I2CTwoWrite(0x3EE6, 0x8303); // RESERVED_MFR_3EE6
	I2CTwoWrite(0x30E4, 0x6372); // RESERVED_MFR_30E4
	I2CTwoWrite(0x30E2, 0x7253); // RESERVED_MFR_30E2
	I2CTwoWrite(0x30E0, 0x5470); // RESERVED_MFR_30E0
	I2CTwoWrite(0x30E6, 0xC4CC); // RESERVED_MFR_30E6
	I2CTwoWrite(0x30E8, 0x8050); // RESERVED_MFR_30E8
	usleep(200);
	I2CTwoWrite(0x302A, 14); // DIV           14
	I2CTwoWrite(0x302C, 1); // DIV
	I2CTwoWrite(0x302E, 3); // DIV
	I2CTwoWrite(0x3030, 65); // MULTI          44
	I2CTwoWrite(0x3082, 0x0029);
	// OPERATION_MODE_CTRL
	I2CTwoWrite(0x30B0, 0x5330);
	I2CTwoWrite(0x305e, 0x00ff); // gain
	I2CTwoWrite(0x3012, 0x0020);
	// coarse integration time
	I2CTwoWrite(0x3064, 0x1802);
}


//-----------------------------------------------------------------------------
unsigned char cvideo_qhy5ii::MSB(unsigned short i)
{
        unsigned short j;
        j=(i&~0x00ff)>>8;
        return j;
}

unsigned char cvideo_qhy5ii::LSB(unsigned short i)
{
        unsigned short j;
        j=i&~0xff00;
        return j;
}


void cvideo_qhy5ii::I2CTwoWrite( uint16_t addr, uint16_t value )
{
    unsigned char data[2];
    data[0] = MSB(value);
    data[1] = LSB(value);

    m_qhy5ii_obj->ctrl_msg( QHYCCD_REQUEST_WRITE, 0xbb, 0, addr, data, 2 );
}


uint16_t cvideo_qhy5ii::I2CTwoRead( uint16_t addr )
{
    unsigned char data[2];

    m_qhy5ii_obj->ctrl_msg( QHYCCD_REQUEST_READ, 0xb7, 0, addr, data, 2 );

    return (uint16_t)(data[0]) * 256 +  (uint16_t)data[1];
}





//----------------------------------- QHY5-II stuff -----------------------------------
void cvideo_qhy5ii::SetExposureTime_QHY5II( uint32_t i )
{
	i *= 1000; // ms => us

	// Required input parameters: CMOSCLK  REG04  REG05 REG0C REG09
	double CMOSCLK;

	if( m_transfer_speed == 1 )
		CMOSCLK = 48;
	else
		CMOSCLK = 24;

	double pixelPeriod;
	pixelPeriod = 1 / CMOSCLK; // unit: us

	double A, Q;
	double P1, P2;
	double RowTime; // unit: us
	uint32_t ExpTime; // unit: us
	unsigned short REG04, REG05, REG0C, REG09;
	double MaxShortExpTime;

	REG04 = I2CTwoRead(0x04);
	REG05 = I2CTwoRead(0x05);
	REG09 = I2CTwoRead(0x09);
	REG0C = I2CTwoRead(0x0C);
	ExpTime = i;

	A = REG04 + 1;
	P1 = 242;
	P2 = 2 + REG05 - 19;
	Q = P1 + P2;
	RowTime = (A + Q) * pixelPeriod;

	MaxShortExpTime = 15000 * RowTime - 180 * pixelPeriod - 4 * REG0C * pixelPeriod;

	unsigned char buf[4];
	if (ExpTime > MaxShortExpTime)
	{
		I2CTwoWrite(0x09, 15000);
		ExpTime = (uint32_t)(ExpTime - MaxShortExpTime);
		buf[0] = 0;
		buf[1] = (uint8_t)(((ExpTime / 1000) & (~0xff00ffff)) >> 16);
		buf[2] = (uint8_t)(((ExpTime / 1000) & (~0xffff00ff)) >> 8);
		buf[3] = (uint8_t)((ExpTime / 1000) & (~0xffffff00));

		m_qhy5ii_obj->ctrl_msg( QHYCCD_REQUEST_WRITE, 0xc1, 0x00, 0x00, buf, 4 );
		ExpTime = (uint32_t)(ExpTime + MaxShortExpTime);
		//longExpMode = true;
	}
	else
	{
		buf[0] = 0;
		buf[1] = 0;
		buf[2] = 0;
		buf[3] = 0;
		m_qhy5ii_obj->ctrl_msg( QHYCCD_REQUEST_WRITE, 0xc1, 0x00, 0x00, buf, 4 );
		usleep(100);
		REG09 = (unsigned short)((ExpTime + 180 * pixelPeriod + 4 * REG0C * pixelPeriod) / RowTime);
		if( REG09 < 1 )
			REG09 = 1;
		I2CTwoWrite( 0x09, REG09 );
		ExpTime = (uint32_t)(REG09 * RowTime - 180 * pixelPeriod - 4 * REG0C * pixelPeriod);
		//longExpMode = false;
	}
}


void cvideo_qhy5ii::QHY5IISetResolution( int x, int y )
{
	I2CTwoWrite(0x09, 200);
	I2CTwoWrite(0x01, 8 + (1024 - y) / 2); // y start
	I2CTwoWrite(0x02, 16 + (1280 - x) / 2); // x start
	I2CTwoWrite(0x03, (unsigned short)(y - 1)); // y size
	I2CTwoWrite(0x04, (unsigned short)(x - 1)); // x size
	I2CTwoWrite(0x22, 0x00); // normal bin
	I2CTwoWrite(0x23, 0x00); // normal bin
}


void cvideo_qhy5ii::CorrectQHY5IIWH( int *w, int *h )
{
	/*
    if(*w <= 320 && *h <=240)
    {
        *w = 320;
        *h = 240;
        initQHY5II_QVGA();
    }
    else if(*w <= 400 && *h <= 400)
    {
        *w = 400;
        *h = 400;
        initQHY5II_R400();
    }
    else
    */
    if( *w <= 640 && *h <= 480 )
    {
        *w = 640;
        *h = 480;
        initQHY5II_VGA();
    }
    else
    if( *w <= 800 && *h <= 600 )
    {
        *w = 800;
        *h = 600;
        initQHY5II_SVGA();
    }
    /*
    else
    if(*w <= 800 && *h <= 800)
    {
        *w = 800;
        *h = 800;
        initQHY5II_R800();
    }
    else if(*w < 960 && *h <= 720)
    {
        *w = 960;
        *h = 720;
        initQHY5II_960X720();
    }
    */
    else if(*w <= 1024 && *h <= 768)
    {
        *w = 1024;
        *h = 768;
        initQHY5II_XGA();
    }
    /*
    else if(*w <= 1280 && *h <= 720)
    {
        *w = 1280;
        *h = 720;
        initQHY5II_1280X720();
    }
    */
    else
    if( *w <= 1280 && *h <= 1024 )
    {
        *w = 1280;
        *h = 1024;
        initQHY5II_SXGA();
    }
}


void cvideo_qhy5ii::initQHY5II_SXGA( void )
{
    if( m_qhy5iiDeNoise )
    {
        QHY5IISetResolution( 1312, 1024 );
    }
    else
    {
        QHY5IISetResolution( 1280, 1024 );
    }
}

void cvideo_qhy5ii::initQHY5II_XGA( void )
{
    if( m_qhy5iiDeNoise )
    {
        QHY5IISetResolution(1312, 768);
    }
    else
    {
        QHY5IISetResolution(1024, 768);
    }
}

void cvideo_qhy5ii::initQHY5II_SVGA( void )
{
    if( m_qhy5iiDeNoise )
    {
        QHY5IISetResolution( 1312, 600 );
    }
    else
    {
        QHY5IISetResolution( 800, 600 );
    }
}


void cvideo_qhy5ii::initQHY5II_VGA( void )
{
    if( m_qhy5iiDeNoise )
    {
        QHY5IISetResolution( 1312, 480 );
    }
    else
    {
        QHY5IISetResolution( 640, 480 );
    }
}



void cvideo_qhy5ii::SetQHY5IIGain( unsigned short gain )
{
	gain *= 10; // gain 0-100  => 0 - 1000

    int i = 0;
    int Gain_Min = 0;
    int Gain_Max = 72;
    i = (Gain_Max - Gain_Min) * gain / 1000;
    I2CTwoWrite(0x35, m_gain_lut[i]);
}


int cvideo_qhy5ii::qhy5_exposure_kludge( unsigned tm )
{
	int ret = EXIT_FAILURE;

	if( m_dev_type == DEVICETYPE_QHY5LII )
	{
		m_qhy5ii_obj->lock();
		CorrectQHY5LIIWH( &m_width, &m_height );
		m_qhy5ii_obj->unlock();

		ret = set_exposure_time( tm );
		set_gain( capture_params.gain );
		set_usb_traffic( m_usb_traf );
	}
	else
	{
		ret = set_exposure_time( tm );
	}

	return ret;
}


int cvideo_qhy5ii::set_gain_core( unsigned short gain )
{
    // one more crutch
    gain = gain > 0 ? gain : 1;

    m_qhy5ii_obj->lock();

    bool err = false;
    if( m_dev_type == DEVICETYPE_QHY5LII )
        SetQHY5LIIGain( gain );
    else
    if( m_dev_type == DEVICETYPE_QHY5II )
        SetQHY5IIGain( gain );
    else
        err = true;

    m_qhy5ii_obj->unlock();

    return err? EXIT_FAILURE : EXIT_SUCCESS;
}


void cvideo_qhy5ii::start_video_mode( void )
{
	m_qhy5ii_obj->lock();

	unsigned char buf[1] = { 100 };
	m_qhy5ii_obj->ctrl_msg( QHYCCD_REQUEST_WRITE, 0xb3, 0, 0, buf, 1 );

	m_qhy5ii_obj->unlock();
}


void cvideo_qhy5ii::stop_video_mode( void )
{
	m_qhy5ii_obj->lock();

	unsigned char buf[4] = { 0, 0, 0, 0 };
	m_qhy5ii_obj->ctrl_msg( QHYCCD_REQUEST_WRITE, 0xc1, 0, 0, buf, 4 );

	m_qhy5ii_obj->unlock();
}

}
