/*
 * lin_guider.cpp
 *
 *      Author: gm
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

#include <stddef.h>

#include "lin_guider.h"
#include "params.h"
#include "utils.h"
#include "maindef.h"

#include "io_driver.h"
#include "io_lpt.h"
#include "io_ftdi.h"
#include "io_qhy5.h"
#include "io_null.h"
#include "io_qhy6.h"
#include "io_gpusb.h"
#include "io_gpio.h"
#include "io_qhy5ii.h"
#include "io_nexstar.h"
#include "io_atik.h"
#include "io_sx.h"
#include "io_asi.h"
#include "io_skywatcher.h"

#include "video.h"
#include "video_pwc.h"
#include "video_uvc.h"
#include "video_null.h"
#include "video_qhy5.h"
#include "video_dsi2pro.h"
#include "video_qhy6.h"
#include "video_qhy5ii.h"
#include "video_atik.h"
#include "video_sx.h"
#include "video_asi.h"


lin_guider::lin_guider(QWidget *parent)
    : QMainWindow(parent)
{
 bool res;

    log_i("Starting Lin_guider...");

	ui.setupUi(this);

	setWindowTitle( QString("Lin-guider") );
	setWindowIcon( QIcon(QString::fromUtf8(":/new/prefix1/lin_guider.png")) );

	m_hfd_info_label = new QLabel();
	m_hfd_info_label->setFrameShape( QFrame::Panel );
	m_hfd_info_label->setFrameShadow( QFrame::Sunken );
	m_hfd_info_label->setVisible( false );
	ui.statusbar->addWidget( m_hfd_info_label, 0 );

	m_video_name_label = new QLabel();
	m_video_name_label->setFrameShape( QFrame::Panel );
	m_video_name_label->setFrameShadow( QFrame::Sunken );
	ui.statusbar->addPermanentWidget( m_video_name_label, 0 );

	m_io_name_label = new QLabel();
	m_io_name_label->setFrameShape( QFrame::Panel );
	m_io_name_label->setFrameShadow( QFrame::Sunken );
	ui.statusbar->addPermanentWidget( m_io_name_label, 0 );

	// connect toolbar
	connect( ui.actionExit, 		SIGNAL(triggered()), this, SLOT(onActionExit()) );
	connect( ui.actionSetupGuider, 	SIGNAL(triggered()), this, SLOT(onShowSetupGuider()) );
	connect( ui.actionSetupDriver, 	SIGNAL(triggered()), this, SLOT(onShowSetupDriver()) );
	connect( ui.actionRecord, 		SIGNAL(triggered()), this, SLOT(onRecord()) );
	connect( ui.actionCalibration, 	SIGNAL(triggered()), this, SLOT(onShowCalibration()) );
	connect( ui.actionGuiding, 		SIGNAL(triggered()), this, SLOT(onShowGuiding()) );
	connect( ui.actionSettings, 	SIGNAL(triggered()), this, SLOT(onShowSettings()) );
	connect( ui.actionAbout, 		SIGNAL(triggered()), this, SLOT(onShowAbout()) );
	connect( ui.action_Toggle_Calibration_Guider, SIGNAL(triggered()), this, SLOT(onToggleCalibrationGuider()) );
	connect( ui.actionAdjust2fitCamera, SIGNAL(triggered()), this, SLOT(onAdjust2fitCamera()) );

	param_block = new params();

	param_block->load();

	// get params
	m_capture_params	 = param_block->get_capture_params();
	m_guider_params	     = param_block->get_guider_params();
	m_ui_params		     = param_block->get_ui_params();
	m_device_params      = param_block->get_device_params();
	m_calibration_params = param_block->get_calibration_params();
	param_block->get_video_dev( dev_name_video, sizeof(dev_name_video) );
	param_block->get_io_dev( dev_name_io, sizeof(dev_name_io) );
	m_net_params    = param_block->get_net_params();
	m_common_params = param_block->get_common_params();
	m_drift_view_params =  param_block->get_drift_view_params();

	// create devices...
	// io driver
	switch( m_device_params.type )
	{
	case io_drv::DT_LPT:
		m_driver = new io_drv::cio_driver_lpt();
		break;
	case io_drv::DT_FTDI:
		m_driver = new io_drv::cio_driver_ftdi();
		break;
	case io_drv::DT_QHY5:
		m_driver = new io_drv::cio_driver_qhy5();
		break;
	case io_drv::DT_NULL:
		m_driver = new io_drv::cio_driver_null();
		break;
	case io_drv::DT_QHY6:
		m_driver = new io_drv::cio_driver_qhy6();
		break;
	case io_drv::DT_GPUSB:
		m_driver = new io_drv::cio_driver_gpusb();
		break;
	case io_drv::DT_GPIO:
		m_driver = new io_drv::cio_driver_gpio();
		break;
	case io_drv::DT_QHY5II:
		m_driver = new io_drv::cio_driver_qhy5ii();
		break;
	case io_drv::DT_NEXSTAR:
		m_driver = new io_drv::cio_driver_nexstar();
		break;
	case io_drv::DT_ATIK:
		m_driver = new io_drv::cio_driver_atik();
		break;
	case io_drv::DT_SX:
		m_driver = new io_drv::cio_driver_sx();
		break;
	case io_drv::DT_ASI:
		m_driver = new io_drv::cio_driver_asi();
		break;
	case io_drv::DT_SKYWATCHER:
		m_driver = new io_drv::cio_driver_skywatcher();
		break;		
	default:
		m_driver = new io_drv::cio_driver_null( true );

	}
	res = m_driver->set_deviceparams( m_device_params );
	if( !res )
		QMessageBox::warning( this, tr("Warning"), tr("Pulse-driver direction map possibly incorrect.\nIt may be cause of guiding errors or device corruption!\n\n\
It's strongly recommended to fix this issue."), QMessageBox::Ok );
	m_driver->start( dev_name_io );

	// video device
	int cam_type =  video_drv::cvideo_base::detect_best_device( m_capture_params.type, dev_name_video );
	switch( cam_type )
	{
	case video_drv::DRV_UVC:
		m_video = new video_drv::cvideo_uvc();
		break;
	case video_drv::DRV_PWC:
		m_video = new video_drv::cvideo_pwc();
		break;
	case video_drv::DRV_NULL:
		m_video = new video_drv::cvideo_null();
		break;
	case video_drv::DRV_QHY5:
		m_video = new video_drv::cvideo_qhy5();
		break;
	case video_drv::DRV_DSI2PRO:
		m_video = new video_drv::cvideo_dsi2pro();
		break;
	case video_drv::DRV_QHY6:
		m_video = new video_drv::cvideo_qhy6();
		break;
	case video_drv::DRV_QHY5II:
		m_video = new video_drv::cvideo_qhy5ii();
		break;
	case video_drv::DRV_ATIK:
		m_video = new video_drv::cvideo_atik( );
		break;
	case video_drv::DRV_SX:
		m_video = new video_drv::cvideo_sx( );
		break;
	case video_drv::DRV_ASI:
		m_video = new video_drv::cvideo_asi( );
		break;
	default:
		m_video = new video_drv::cvideo_null( true );
	}

	connect( m_video, SIGNAL( renderImage(const void *, int) ), this, SLOT( onGetVideo(const void *, int) ) );

	m_video->set_capture_params( m_capture_params );	// try to set desired params
	res = m_video->start( dev_name_video );
	m_capture_params = m_video->get_capture_params();	// receive actual params
	{
		const struct video_drv::sensor_info_s &si = m_video->get_sensor_info();
		if( si.is_available && m_guider_params.auto_info )
		{
			m_guider_params.matrix_width     = si.matrix_width;
			m_guider_params.matrix_height    = si.matrix_height;
			m_guider_params.ccd_pixel_width  = si.pixel_width;
			m_guider_params.ccd_pixel_height = si.pixel_height;
		}
	}

	// create receiving video buffer
	m_v_buf = (u_char *)malloc( m_capture_params.width * m_capture_params.height * sizeof(uint32_t) );
	memset( m_v_buf, 0, m_capture_params.width * m_capture_params.height * sizeof(uint32_t) );


	// create server
	m_server = new server( m_net_params );
	connect( m_server, SIGNAL( do_command() ), this, SLOT( onRemoteCmd() ) );
	m_server->start();

	// guider and video setup dialog
	setup_video_wnd = new setup_video( this );

	// parport setup dialog
	setup_driver_wnd = new setup_driver( this );

	// calibration window
	reticle_wnd = new rcalibration( this );
	reticle_wnd->set_video_params( m_capture_params.width, m_capture_params.height );

	// guider dialog
	guider_wnd = new guider( this, m_driver, &m_drift_view_params, m_common_params );
	guider_wnd->set_half_refresh_rate( m_ui_params.half_refresh_rate );

	// movie recorder dialog
	recorder_wnd = new mrecorder( this );
	recorder_wnd->set_video_params( m_v_buf, m_capture_params.width, m_capture_params.height );

	// settings dialog
	settings_wnd = new settings( this, &m_net_params, &m_common_params, &m_ui_params, &m_drift_view_params );

	// about dialog
	about_wnd = new about( this );

	m_drawer_delegate = new drawer_delegate( this );


	// main window video widget...
	m_video_out = new custom_drawer( ui.videoFrame );
	m_video_out->move( ui.videoFrame->frameWidth(), ui.videoFrame->frameWidth() );
	m_video_out->setAttribute( Qt::WA_NoSystemBackground, true );

	m_video_buffer = new QImage( m_v_buf, m_capture_params.width, m_capture_params.height, QImage::Format_RGB32 );

	// set all sizes
	m_video_out->set_source( m_video_buffer, m_drawer_delegate );
	ui.videoFrame->resize( m_capture_params.width + 2*ui.videoFrame->frameWidth(), m_capture_params.height + 2*ui.videoFrame->frameWidth() );

	// Init scroller
	QScrollArea *scrollArea = new QScrollArea( centralWidget() );
	scrollArea->setWidget( ui.videoFrame );
	scrollArea->setSizePolicy( QSizePolicy::Expanding, QSizePolicy::Expanding );
	scrollArea->setFocusPolicy( Qt::NoFocus );
	QGridLayout *scrollLayout = new QGridLayout( centralWidget() );
	scrollLayout->setContentsMargins( 0, 0, 0, 0 );
	scrollLayout->addWidget( scrollArea );

	//math...
	m_math = new cgmath( m_common_params );
	m_math->set_video_params( m_capture_params.width, m_capture_params.height );
	m_math->set_guider_params( m_guider_params.ccd_pixel_width, m_guider_params.ccd_pixel_height, m_guider_params.aperture, m_guider_params.focal );
	m_math->set_in_params( param_block->get_math_in_params() );


	// attach math to all modules
	guider_wnd->set_math( m_math );
	reticle_wnd->set_math( m_math );


	// apply all permanent params
	m_video_name_label->setText( tr("Video:") + QString(m_video->get_name()) );
	m_io_name_label->setText( tr("IO:") + QString(m_driver->get_name()) );

	set_visible_overlays( ovr_params_t::OVR_SQUARE | ovr_params_t::OVR_RETICLE, true );

	memset( d_objs, 0, sizeof(d_objs) );
	d_objs[0].type = ovr_params_t::OVR_RETICLE;
	d_objs[1].type = ovr_params_t::OVR_SQUARE;

	SQR_OVL_COLOR  = QColor( DEF_SQR_OVL_COLOR[0], DEF_SQR_OVL_COLOR[1], DEF_SQR_OVL_COLOR[2]) ;
	RA_COLOR	   = QColor( DEF_RA_COLOR[0], DEF_RA_COLOR[1], DEF_RA_COLOR[2] );
	DEC_COLOR	   = QColor( DEF_DEC_COLOR[0], DEF_DEC_COLOR[1], DEF_DEC_COLOR[2] );
	RET_ORG_COLOR  = QColor( DEF_RET_ORG_COLOR[0], DEF_RET_ORG_COLOR[1], DEF_RET_ORG_COLOR[2] );

	update_sb_video_info();
	update_sb_io_info();

	set_ui_params();
	m_hfd_info_label->setVisible( m_common_params.hfd_on );

	// test
	m_long_task_conn = NULL;
	m_timer.setInterval( 5000 );
	m_timer.setSingleShot( true );
	connect( &m_timer, SIGNAL( timeout() ), this, SLOT( onCmdTimer() ) );

	// setup geometry
	{
		const std::pair< QByteArray, QByteArray >& wnd_gs = param_block->get_wnd_geometry_state( "main_wnd" );
		this->restoreGeometry( wnd_gs.first );
		this->restoreState( wnd_gs.second );
	}
	{
		const std::pair< QByteArray, QByteArray >& wnd_gs = param_block->get_wnd_geometry_state( "guider_wnd" );
		guider_wnd->restoreGeometry( wnd_gs.first );
	}

	log_i("Started successfully");
}


lin_guider::~lin_guider()
{
	delete setup_video_wnd;

	delete setup_driver_wnd;

	delete m_driver;

	delete m_video;

	if( m_long_task_conn )
		m_server->return_conn( m_long_task_conn );
	delete m_server;

	// release drawer
	delete m_video_out;

	delete m_drawer_delegate;

	delete m_video_buffer;
	free( m_v_buf );

	delete m_math;

	delete reticle_wnd;

	delete recorder_wnd;

	delete settings_wnd;

	delete about_wnd;

	// params must be deleted last
	if( param_block )
		delete param_block;

	log_i("Terminated successfully.");
}


void lin_guider::showEvent ( QShowEvent * event )
{
 QString err_str;
 bool full_quit = false;

	if( event->spontaneous() )
		return;

	if( !m_video->is_initialized() )
	{
		err_str += tr("Video input is not initialized.\n");
		full_quit = true;
	}
	if( !m_driver->is_initialized() )
	{
		err_str += tr("Pulse driver is not initialized.\n");
	}

	if( !err_str.isNull() )
	{
		err_str += QString( tr("\nGuiding is not available.") );
		QMessageBox::critical( this, tr("Error"), err_str, QMessageBox::Ok );
		if( full_quit )
			return;
	}

	// start video
	m_video->pause( false );
}


void lin_guider::closeEvent( QCloseEvent *event )
{
	if( !u_yes("Do you want to quit?") )
	{
		event->ignore();
		return;
	}

	if( setup_video_wnd->isVisible() )
		setup_video_wnd->close();
	if( setup_driver_wnd->isVisible() )
		setup_driver_wnd->close();
	if( guider_wnd->isVisible() )
		guider_wnd->close();
	if( reticle_wnd->isVisible() )
		reticle_wnd->close();
	if( recorder_wnd->isVisible() )
		recorder_wnd->close();
	if( about_wnd->isVisible() )
		about_wnd->close();

	// get the actual square_index before saving
	m_common_params.square_index = m_math->get_square_index();
	m_math->get_reticle_params( NULL, NULL, &m_common_params.reticle_angle );

	// save params
	param_block->set_capture_params( m_capture_params );
	param_block->set_capture_next_params( m_video->get_next_params() );
	param_block->set_device_params( m_driver->get_deviceparams() );
	param_block->save_device_cfg = m_driver->is_initialized();
	param_block->set_guider_params( m_guider_params );
	param_block->set_math_in_params( *m_math->get_in_params() );
	param_block->set_ui_params( m_ui_params );
	param_block->set_calibration_params( m_calibration_params );
	param_block->set_video_dev( dev_name_video );
	param_block->set_io_dev( dev_name_io );
	param_block->set_net_params( m_net_params );
	param_block->set_common_params( m_common_params );
	param_block->set_drift_view_params( m_drift_view_params );

	{
		std::pair< QByteArray, QByteArray > wnd_gs = std::make_pair( this->saveGeometry(), this->saveState() );
		param_block->set_wnd_geometry_state( "main_wnd", wnd_gs );
	}
	{
		std::pair< QByteArray, QByteArray > wnd_gs = std::make_pair( guider_wnd->saveGeometry(), QByteArray() );
		param_block->set_wnd_geometry_state( "guider_wnd", wnd_gs );
	}

	param_block->save();
}


void lin_guider::onShowSetupGuider()
{
	setup_video_wnd->exec();
}


void lin_guider::onRecord()
{
	recorder_wnd->exec();
}


void lin_guider::onShowSetupDriver()
{
	setup_driver_wnd->exec();
}


void lin_guider::onShowCalibration()
{
	reticle_wnd->show();
}


void lin_guider::onShowGuiding()
{
	guider_wnd->show();
}


void lin_guider::onShowSettings()
{
	net_params_t old_net_params = m_net_params;

	settings_wnd->exec();
	//check UI changes
	set_ui_params();
	m_hfd_info_label->setVisible( m_common_params.hfd_on );
	m_hfd_info_label->setText( QString() );
	// restart server if necessary
	if( old_net_params != m_net_params )
	{
		if( !restart_server() )
			m_net_params = old_net_params;
	}

}


void lin_guider::onShowAbout()
{
	about_wnd->show();
}


void lin_guider::onActionExit()
{
	close();
}


bool lin_guider::restart_server( void )
{
	size_t num_conns = m_server->get_num_active_connections();
	if( num_conns )
	{
		int answer = QMessageBox::question(
				this,
				tr("Reset Connections"),
				tr("There are %1 active connections to the server.\n \"Yes\" will close them and apply changes.\n \"No\" will keep them, but changes will be applied on restart.").
			arg(num_conns), QMessageBox::Yes, QMessageBox::No|QMessageBox::Default|QMessageBox::Escape);

		if( answer != QMessageBox::Yes )
			return false;
	}

	disconnect( m_server, 0, 0, 0 );
	delete(m_server);
	m_server = new server(m_net_params);
	connect( m_server, SIGNAL( do_command() ), this, SLOT( onRemoteCmd() ) );
	m_server->start();

	return true;
}


void lin_guider::onToggleCalibrationGuider()
{
	if( reticle_wnd->isVisible() )
	{
		reticle_wnd->close();
		guider_wnd->show();
	}
	else
	if( guider_wnd->isVisible() )
	{
		guider_wnd->close();
		reticle_wnd->show();
	}
}


void lin_guider::onAdjust2fitCamera()
{
	QRect fg = ui.videoFrame->frameGeometry();
	int sb_width = ui.videoFrame->style()->pixelMetric(QStyle::PM_ScrollBarExtent);
	(void)sb_width;
	QSize f = frameSize() - size();
	QPoint lt = centralWidget()->mapToParent( QPoint(0, 0) );
	resize( fg.width() + lt.x() + f.width() + 4/*- sb_width*/, fg.height() + lt.y() + f.height() + 4 /*- sb_width*/ /*+ ui.statusbar->height()*/ );
}


// NOTE!!! Don't add code in this method at all
void lin_guider::onGetVideo( const void *src, int len )
{
 static uint32_t tick  = 0;
 int math_buf_size  = 0;
 double *math_buf = NULL;

 	// get math buffer
 	math_buf = m_math->get_data_buffer( NULL, NULL, NULL, &math_buf_size );

 	assert( math_buf );

	// decode video frame
 	m_video->process_frame( (void *)m_v_buf,
			m_capture_params.width * m_capture_params.height * sizeof(uint32_t),
			math_buf,
			math_buf_size,
			src,
			m_guider_params.bw_video,
			len );

	// unfreeze video thread
 	m_video->continue_capture();

	// main GUIDER call
	guider_wnd->guide();

	// try to record movie
	recorder_wnd->add_frame();

	tick++;
	// skip half frames
	if( m_ui_params.half_refresh_rate && (tick & 1) )
		return;

	// HFD
	if( m_common_params.hfd_on )
	{
		const cproc_out_params *out = m_math->get_out_params();
		m_hfd_info_label->setText( QString("HFD: ") +
								   (out->hfd_h > 0 ? QString().setNum(out->hfd_h, 'f', 2) : QString("_.__")) +
								   QString("\", Lmax: " + QString().setNum(out->hfd_lum_max, 'f', 0) ) );
	}

	// draw overlays over video frame AFTER math and update frame
	m_video_out->update();
}


// NOTE! don't add code of unknown performance here
void lin_guider::onRemoteCmd( void )
{
	conn_t *pconn = m_server->take_conn(); // detach conn from server

	if( !pconn )
		return;

	// prepare data
	size_t all_data_sz = 0;
	const uint8_t * const all_data = pconn->get_data( &all_data_sz );
	const proto_hdr *hdr = proto_hdr::get( all_data );
 	const uint8_t * const data = proto_hdr::data( all_data );
 	size_t data_sz = all_data_sz - proto_hdr::HDR_SIZE;
 	char data_str[conn_t::BUF_SZ];	// for safe string
 	size_t data_str_len = 0;
	int answer_sz = 0;
	size_t answer_sz_max = 0;
	char * const answer = reinterpret_cast<char*>( pconn->get_answer( &answer_sz_max ) );

	if( !proto_hdr::check( all_data ) )
		log_e( "lin_guider::onRemoteCmd(): Protocol error. Signature is wrong" );
	if( hdr->data_sz > data_sz )
		log_e( "lin_guider::onRemoteCmd(): Protocol error. Data is too long" );

	// command handler
	switch( hdr->cmd )
	{
	case server::GET_VER:
	{
		answer_sz = snprintf( answer, answer_sz_max, "v." VERSION );
	}
		break;
	case server::SET_GUIDER_SQUARE_POS:
	{
		if( data_sz )
		{
			u_make_safe_str( (const char*)data, data_sz, sizeof(data_str), data_str, &data_str_len );

			// parse coords
			double newx = -1, newy = -1;
			unsigned int parsed = 0, arg_len = 0;
			const char *arg = NULL;
			for( int n = 0; u_memtok( data_str, data_str_len, ' ', &arg, &arg_len, &parsed ) && n < 2; n++ )
			{
				switch( n )
				{
				case 0:	// x
					newx = strtod( arg, NULL );
					break;
				case 1:	// y
					newy = strtod( arg, NULL );
					break;
				default:
					continue;
				}
			}
			// move square
			if( newx != -1 && newy != -1 )
			{
				ovr_params_t *povr = m_math->prepare_overlays();
				m_math->move_square( (double)(newx - povr->square_size/2), (double)(newy - povr->square_size/2) );
				answer_sz = snprintf( answer, answer_sz_max, "OK" );
				break;
			}
		}
		// error
		answer_sz = snprintf( answer, answer_sz_max, "Unable to get coordinates" );
	}
		break;
	case server::SAVE_FRAME:
	case server::SAVE_FRAME_DECORATED:
	{
		u_make_safe_str( (const char*)data, data_sz, sizeof(data_str), data_str, &data_str_len );
		if( data_str_len )
		{
			const char *home_dir = getenv( "HOME" );
			const char *fname = data_str;
			bool res = true;
			if (hdr->cmd == server::SAVE_FRAME_DECORATED) {
				QImage video_buf = m_video_buffer->copy();
				QPainter painter;
				painter.begin( &video_buf );
				draw_overlays( painter );
				painter.end();
				res = video_buf.save( QString( home_dir ) + "/" + QString( fname ) + ".bmp", "BMP" );
			} else {
				res = m_video_buffer->save( QString( home_dir ) + "/" + QString( fname ) + ".bmp", "BMP" );
			}
			if( res )
				answer_sz = snprintf( answer, answer_sz_max, "SAVED:%s/%s.bmp", home_dir, fname );
			else
				answer_sz = snprintf( answer, answer_sz_max, "ERROR saving:%s/%s.bmp", home_dir, fname );
		}
		else
			answer_sz = snprintf( answer, answer_sz_max, "ERROR saving:Empty filename" );
	}
		break;
	case server::DITHER:
	{
		if( m_long_task_conn == NULL )
		{
			int tout = m_math->dither();
			if( tout > 0 ) // do long task
			{
				set_visible_overlays( ovr_params_t::OVR_RETICLE_ORG, true );
				m_long_task_conn = pconn;
				m_timer.setInterval( tout * 1000 );
				m_timer.start();
				return;
			}
			else
				answer_sz = snprintf( answer, answer_sz_max, "Error: %s", m_math->get_dither_errstring( tout ) );
		}
		else
			answer_sz = snprintf( answer, answer_sz_max, "BUSY: in progress..." );
	}
		break;
	case server::DITHER_NO_WAIT_XY:
	{
		if( data_sz )
		{
			u_make_safe_str( (const char*)data, data_sz, sizeof(data_str), data_str, &data_str_len );

			// maximum relative offsets
			double rx = -1, ry = -1;
			unsigned int parsed = 0, arg_len = 0;
			const char *arg = NULL;
			for( int n = 0; u_memtok( data_str, data_str_len, ' ', &arg, &arg_len, &parsed ) && n < 2; n++ )
			{
				switch( n )
				{
				case 0:	// x
					rx = strtod( arg, NULL );
					break;
				case 1:	// y
					ry = strtod( arg, NULL );
					break;
				default:
					continue;
				}
			}
			// move reticle
			if( rx != -1 && ry != -1 )
			{
				int res = m_math->dither_no_wait_xy( fabs(rx), fabs(ry) );
				if (res < 0) {
					answer_sz = snprintf(answer, answer_sz_max, "Error: %s", m_math->get_dither_errstring( res ));
				} else {
					set_visible_overlays( ovr_params_t::OVR_RETICLE_ORG, true );
					answer_sz = snprintf(answer, answer_sz_max, "OK");
				}
				break;
			}
		}
		// error
		answer_sz = snprintf( answer, answer_sz_max, "Unable to get offsets" );
	}
	break;
	case server::GET_DISTANCE:
	{
		double dx, dy;
		int res = m_math->get_distance(&dx, &dy);
		if (res < 0) {
			answer_sz = snprintf(answer, answer_sz_max, "Error: %s", m_math->get_dither_errstring( res ));
		} else {
			answer_sz = snprintf( answer, answer_sz_max, "%0.2f %0.2f", dx,dy);
		}
	}
	break;
       /* Add to support iAstroHub, by Anat & Zachary */
       case server::START_AUTOGUIDING:
       {
               answer_sz = snprintf( answer, answer_sz_max, "Autoguiding Started" );

               reticle_wnd->close();
               guider_wnd->close();

               onShowGuiding();
               guider_wnd->onStartStopButtonClick();
       }
               break;

       case server::STOP_AUTOGUIDING:
       {
               answer_sz = snprintf( answer, answer_sz_max, "Autoguiding Stopped" );

               guider_wnd->close();

       }
               break;

       case server::START_CALIBRATION:
       {
               answer_sz = snprintf( answer, answer_sz_max, "Calibration Started" );

               reticle_wnd->close();
               guider_wnd->close();

               m_calibration_params.auto_mode = true;

               onShowCalibration();
               reticle_wnd->onStartReticleCalibrationButtonClick();
       }
               break;

       case server::STOP_CALIBRATION:
       {
               answer_sz = snprintf( answer, answer_sz_max, "Calibration Stopped" );

               reticle_wnd->close();

       }
               break;

       case server::FINDSTAR:
       {
               answer_sz = snprintf( answer, answer_sz_max, "Find Star" );

               reticle_wnd->onFindStarButtonClick();

       }
               break;

       case server::SET_VIDEO_GAIN:
       {

               video_drv::post_param_t prm;
               video_drv::param_val_t val;
               memset( &prm, 0, sizeof(video_drv::post_param_t) );

               long newgain;
               u_make_safe_str( (const char*)data, data_sz, sizeof(data_str), data_str, &data_str_len );
               newgain = strtol( data_str, NULL, NULL );

               answer_sz = snprintf( answer, answer_sz_max, "Setting video gain" );

               m_capture_params = m_video->get_capture_params();

               m_capture_params.gain = newgain;

               val.set( m_capture_params.gain );
               m_video->pack_params( video_drv::CI_GAIN, val, &prm );
               m_video->post_params( prm );

       }
               break;


       case server::SET_FPS:
       {

               video_drv::post_param_t prm;
               video_drv::param_val_t val;
               memset( &prm, 0, sizeof(video_drv::post_param_t) );

               video_drv::current_format_state_t format_state;
               video_drv::time_fract tmp;

               long index_fps;
               u_make_safe_str( (const char*)data, data_sz, sizeof(data_str), data_str, &data_str_len );
               index_fps = strtol( data_str, NULL, NULL );

               answer_sz = snprintf( answer, answer_sz_max, "Setting FPS" );

               m_capture_params = m_video->get_capture_params();

               format_state = m_video->get_current_format_params();

               tmp = format_state.format_desc->frame_table[format_state.frame_idx].fps_table[index_fps];

               if( m_capture_params.fps != tmp )
               {
                       val.set( tmp.numerator, tmp.denominator );

                       m_video->pack_params( video_drv::CI_FPS, val, &prm );
                       // send params to video thread
                       m_video->post_params( prm );
               }

               m_capture_params.fps = tmp;

       }
               break;

       case server::SET_GAIN_P:
       {
               cproc_in_params *in_params;

               in_params   = m_math->get_in_params();

               answer_sz = snprintf( answer, answer_sz_max, "Setting P gain" );

               u_make_safe_str( (const char*)data, data_sz, sizeof(data_str), data_str, &data_str_len );

                       // parse coords
                       double newx_p, newy_p;
                       unsigned int parsed = 0, arg_len = 0;
                       const char *arg = NULL;
                       for( int n = 0; u_memtok( data_str, data_str_len, ' ', &arg, &arg_len, &parsed ) && n < 2; n++ )
                       {
                               switch( n )
                               {
                               case 0: // x
                                       newx_p = strtod( arg, NULL );
                                       break;
                               case 1: // y
                                       newy_p = strtod( arg, NULL );
                                       break;
                               default:
                                       continue;
                               }
                       }

               in_params->proportional_gain[RA] = newx_p;
               in_params->proportional_gain[DEC] = newy_p;

       }
               break;


       case server::GET_GAIN_P:
       {

               FILE *ptout;
               float xgain, ygain;

               cproc_in_params *in_params;

               in_params   = m_math->get_in_params();

               answer_sz = snprintf( answer, answer_sz_max, "Getting P gain" );

               xgain = in_params->proportional_gain[RA];
               ygain = in_params->proportional_gain[DEC];
               ptout=fopen("/home/pi/www/get_gain.txt","w");
               fprintf(ptout,"%.1f %.1f\n",xgain,ygain);
               fclose(ptout);

       }
               break;


       case server::MOVE_RETICLE:
       {

               double pos_x, pos_y;
               m_math->get_star_screen_pos( &pos_x, &pos_y );
               m_math->set_reticle_params( pos_x, pos_y, -1 );

               answer_sz = snprintf( answer, answer_sz_max, "Reticle moved" );

       }
               break;
       /* iAstroHub patch ends */

	default:
		// write some strange answer
		answer_sz = snprintf( answer, answer_sz_max, "Unknown command" );
	}

	// return connection to server
	pconn->answer_sz = answer_sz;
	m_server->return_conn( pconn );
}


void lin_guider::onCmdTimer()
{
	assert( m_long_task_conn );

	int answer_sz = 0;
	size_t answer_sz_max = 0;
	char * const answer = reinterpret_cast<char*>( m_long_task_conn->get_answer( &answer_sz_max ) );
	answer_sz = snprintf( answer, answer_sz_max, "Long time cmd finished" );

	m_long_task_conn->answer_sz = answer_sz;
	m_server->return_conn( m_long_task_conn );
	m_long_task_conn = NULL;
}


void lin_guider::set_visible_overlays( int ovr_mask, bool set )
{
 	ovr_params_t *povr = m_math->prepare_overlays();

	if( set )
		povr->visible |= ovr_mask;
	else
		povr->visible &= (~ovr_mask);
}


void lin_guider::lock_toolbar( bool lock )
{
	ui.menubar->setEnabled( !lock );
	ui.toolBar_Main->setEnabled( !lock );
}


bool lin_guider::activate_drag_object( int x, int y )
{
 	ovr_params_t *povr = m_math->prepare_overlays();

 	for( int i = 0;i < 2;i++ )
 	{
 		if( d_objs[i].type == ovr_params_t::OVR_SQUARE ) // square
 		{
 			if( !(povr->visible & ovr_params_t::OVR_SQUARE) )
 				continue;
 			if( x > povr->square_pos.x && x < povr->square_pos.x+povr->square_size )
 				if( y > povr->square_pos.y && y < povr->square_pos.y+povr->square_size )
 				{
 					d_objs[i].active = true;
 					m_math->suspend( true );
 					return true;
 				}
 		}
 		else
		if( d_objs[i].type == ovr_params_t::OVR_RETICLE ) // square
		{
			if( !(povr->visible & ovr_params_t::OVR_RETICLE) || !reticle_wnd->isVisible() )
				continue;
			if( x > povr->reticle_pos.x - 4 && x < povr->reticle_pos.x + 4 )
				if( y > povr->reticle_pos.y - 4 && y < povr->reticle_pos.y + 4 )
				{
					d_objs[i].active = true;
					return true;
				}
		}
 	}

 return false;
}


bool lin_guider::deactivate_drag_object( int x, int y )
{
	for( int i = 0;i < 2;i++ )
	 	if( d_objs[i].active )
	 	{
	 		if( d_objs[i].type == ovr_params_t::OVR_RETICLE )
	 			reticle_wnd->update_reticle_pos( (double)x, (double)y );

	 		d_objs[i].active = false;
	 		m_math->suspend( false );
	 		return true;
	 	}

 return false;
}


void lin_guider::move_drag_object( int x, int y )
{
	bool upd = false;
	ovr_params_t *povr = m_math->prepare_overlays();

	for( int i = 0;i < 2;i++ )
	{
		if( d_objs[i].active )  // lets move object
		{
			if( d_objs[i].type == ovr_params_t::OVR_SQUARE )
			{
				m_math->move_square( (double)(x - povr->square_size/2), (double)(y - povr->square_size/2) );
				upd = true;
				break;
			}
			else
			if( d_objs[i].type == ovr_params_t::OVR_RETICLE )
			{
				double rx, ry, rang;
				m_math->get_reticle_params( &rx, &ry, &rang );
				m_math->set_reticle_params( (double)x, (double)y, rang );
				upd = true;
				break;
			}
		}
	}
	if( upd )
		m_video_out->update();
}


void lin_guider::draw_overlays( QPainter &painter )
{
	ovr_params_t *povr = m_math->prepare_overlays();

	if( povr->visible & ovr_params_t::OVR_RETICLE_ORG )
	{
		painter.setPen( RET_ORG_COLOR );
		painter.drawPoint( povr->reticle_org.x, povr->reticle_org.y );
	}
	if( povr->visible & ovr_params_t::OVR_SQUARE )
	{
		painter.setPen( SQR_OVL_COLOR );
		painter.drawRect( povr->square_pos.x, povr->square_pos.y, povr->square_size, povr->square_size );
	}
	if( povr->visible & ovr_params_t::OVR_RETICLE )
	{
		painter.setPen( RA_COLOR );
		painter.drawLine( povr->reticle_pos.x,
				povr->reticle_pos.y,
				povr->reticle_pos.x + povr->reticle_axis_ra.x,
				povr->reticle_pos.y + povr->reticle_axis_ra.y);
		painter.setPen( DEC_COLOR );
		painter.drawLine( povr->reticle_pos.x,
				povr->reticle_pos.y,
				povr->reticle_pos.x + povr->reticle_axis_dec.x,
				povr->reticle_pos.y + povr->reticle_axis_dec.y);
		if( reticle_wnd->isVisible() )
		{
			painter.setPen( DEC_COLOR );
			painter.drawRect( povr->reticle_pos.x-4, povr->reticle_pos.y-4, 8, 8);
		}
	}
}


void lin_guider::update_sb_video_info( int override_fps_idx )
{
	char str[64] = {0};
	m_video->get_current_format_params_string( str, sizeof(str), override_fps_idx );
	m_video_name_label->setText( tr("Video: ") + QString(m_video->get_name()) + ", " + QString(str) );
}


void lin_guider::update_sb_io_info( void )
{
	m_io_name_label->setText( tr("IO: ") + QString(m_driver->get_name()) );
}


void lin_guider::set_ui_params( void )
{
	ui.toolBar_Helper->setVisible( m_ui_params.show_helper_TB );
}
