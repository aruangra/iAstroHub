/*
 * rcalibration.cpp
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

#include <unistd.h>
#include <time.h>
#include <assert.h>
#include <vector>
#include <utility>
#include <limits>

#include "rcalibration.h"
#include "lin_guider.h"
#include "gmath.h"
#include "vect.h"
#include "utils.h"


rcalibration::rcalibration(lin_guider *parent)
    : QDialog(parent), pmain_wnd(parent)
{
 int i;

	ui.setupUi(this);

	// setup ui
	setWindowTitle( tr("Calibration") );

	pmath = NULL;

	is_started = false;
	axis = RA;
	auto_drift_time = 25;
	
	start_x1 = start_y1 = 0;
	end_x1 = end_y1 = 0;
	start_x2 = start_y2 = 0;
	end_x2 = end_y2 = 0;

	ui.checkBox_AutoMode->setChecked( false );
	ui.spinBox_DriftTime->setVisible( false );
	ui.spinBox_DriftTime->setValue( auto_drift_time );
	ui.progressBar->setVisible( false );
	ui.spinBox_ReticleAngle->setMaximum( 360 );

	for( i = 0;guide_squares[i].size != -1;i++ )
		ui.comboBox_SquareSize->addItem( QString().setNum( guide_squares[i].size ) );

	ui.spinBox_FrameCount->setMaximum( 100 );

	// connect ui
	connect( ui.comboBox_SquareSize, 	SIGNAL(activated(int)),    		this, SLOT(onSquareSizeChanged(int)) );
	connect( ui.spinBox_DriftTime, 		SIGNAL(valueChanged(int)),		this, SLOT(onDriftTimeChanged(int)) );
	connect( ui.spinBox_ReticleX, 		SIGNAL(valueChanged(double)),	this, SLOT(onReticleXChanged(double)) );
	connect( ui.spinBox_ReticleY, 		SIGNAL(valueChanged(double)),	this, SLOT(onReticleYChanged(double)) );
	connect( ui.spinBox_ReticleAngle,	SIGNAL(valueChanged(double)),	this, SLOT(onReticleAngChanged(double)) );
	connect( ui.pushButton_StartCalibration, SIGNAL(clicked()), 		this, SLOT(onStartReticleCalibrationButtonClick()) );
	connect( ui.checkBox_TwoAxis, 		SIGNAL(stateChanged(int)), 		this, SLOT(onTwoAxisMode(int)) );
	connect( ui.checkBox_AutoMode, 		SIGNAL(stateChanged(int)), 		this, SLOT(onEnableAutoMode(int)) );
	connect( ui.pushButton_StartVideoCalibration, SIGNAL(clicked()),	this, SLOT(onStartVideoCalibrationButtonClick()) );

	connect( pmain_wnd->m_video, SIGNAL(calibrationFinished()),			this, SLOT(onVideoCalibrationFinished()) );
	
	connect( ui.pushButton_FindStar, SIGNAL(clicked()),					this, SLOT(onFindStarButtonClick()) );

}


rcalibration::~rcalibration()
{

}


void rcalibration::showEvent ( QShowEvent * event )
{
	if( event->spontaneous() )
		return;
	
	calibration_params = pmain_wnd->m_calibration_params;

	pmain_wnd->lock_toolbar( true );

	fill_interface();
}


void rcalibration::closeEvent( QCloseEvent * )
{
	is_started = false;

	pmain_wnd->lock_toolbar( false );

	pmain_wnd->m_driver->reset();
	pmain_wnd->m_video->cancel_calibration();
	
	ui.groupBox_ReticleCalibration->setEnabled( true );
	ui.groupBox_VideoCalibration->setEnabled( true );

	ui.checkBox_TwoAxis->setEnabled( true );
	ui.pushButton_FindStar->setEnabled( true );
	ui.checkBox_AutoMode->setEnabled( true );
	ui.pushButton_StartCalibration->setText( tr("Start") );
	ui.pushButton_StartCalibration->setEnabled( true );
	ui.l_RStatus->setText( tr("State: ...") );

	ui.pushButton_StartVideoCalibration->setText( tr("Start") );
	ui.pushButton_StartVideoCalibration->setEnabled( true );
	ui.l_VStatus->setText( tr("State: ...") );

	calibration_params.two_axis = (ui.checkBox_TwoAxis->checkState() == Qt::Checked);
	calibration_params.auto_mode = (ui.checkBox_AutoMode->checkState() == Qt::Checked);
	calibration_params.dift_time = ui.spinBox_DriftTime->value();
	calibration_params.frame_count = ui.spinBox_FrameCount->value();
	pmain_wnd->m_calibration_params = calibration_params;
}


void rcalibration::hideEvent ( QHideEvent * event )
{
	if( event->spontaneous() )
	{
		return;
	}
	//event->ignore();
	close();
}


void rcalibration::fill_interface( void )
{
 double rx, ry, rang;

 	if( !pmath )
 		return;

 	pmath->get_reticle_params( &rx, &ry, &rang );

 	io_drv::device_ro_params_t ro_device_params = pmain_wnd->m_driver->get_ro_params();
 	ui.spinBox_DriftTime->setMaximum( (int)((double)ro_device_params.max_pulse_length / 1000 / 1.5) ); // return to start position time is 1.5 time greater than drift time, so we must fit it into max_pulse_length

 	ui.comboBox_SquareSize->setCurrentIndex( pmath->get_square_index() );
 	ui.spinBox_ReticleX->setValue( rx );
 	ui.spinBox_ReticleY->setValue( ry );
 	ui.spinBox_ReticleAngle->setValue( rang );
 	ui.progressBar->setValue( 0 );

	int time_value = MIN(ui.spinBox_DriftTime->maximum(), calibration_params.dift_time);
	ui.checkBox_TwoAxis->setChecked( calibration_params.two_axis );
	ui.checkBox_AutoMode->setChecked( calibration_params.auto_mode );
	ui.spinBox_DriftTime->setValue( time_value );
	ui.spinBox_FrameCount->setValue( calibration_params.frame_count );
}


bool rcalibration::set_video_params( int vid_wd, int vid_ht )
{
	if( vid_wd <= 0 || vid_ht <= 0 )
		return false;

	ui.spinBox_ReticleX->setMaximum( (double)vid_wd );
	ui.spinBox_ReticleY->setMaximum( (double)vid_ht );

 return true;
}


void rcalibration::update_reticle_pos( double x, double y )
{
 	if( !isVisible() )
 		return;

  	if( ui.spinBox_ReticleX->value() == x && ui.spinBox_ReticleY->value() == y )
  		return;

	ui.spinBox_ReticleX->setValue( x );
	ui.spinBox_ReticleY->setValue( y );
}


void rcalibration::set_math( cgmath *math )
{
	assert( math );
	pmath = math;
}


void rcalibration::onSquareSizeChanged( int index )
{
	if( !pmath )
		return;

	pmath->resize_square( index );
}


void rcalibration::onTwoAxisMode( int state )
{
	calibration_params.two_axis = ( state == Qt::Checked );
}


void rcalibration::onEnableAutoMode( int state )
{
	calibration_params.auto_mode = ( state == Qt::Checked );
	ui.spinBox_DriftTime->setVisible( calibration_params.auto_mode );
	ui.progressBar->setVisible( calibration_params.auto_mode );
}


void rcalibration::onDriftTimeChanged( int val )
{
	calibration_params.dift_time = val;
}


void rcalibration::onReticleXChanged( double val )
{
 double x, y, ang;

	if( !pmath )
		return;

	pmath->get_reticle_params( &x, &y, &ang );
	pmath->set_reticle_params( val, y, ang );

	// update overlay
	pmain_wnd->update_video_out();
}


void rcalibration::onReticleYChanged( double val )
{
 double x, y, ang;

	if( !pmath )
		return;

	pmath->get_reticle_params( &x, &y, &ang );
	pmath->set_reticle_params( x, val, ang );

	// update overlay
	pmain_wnd->update_video_out();
}


void rcalibration::onReticleAngChanged( double val )
{
 double x, y, ang;

	if( !pmath )
		return;

	pmath->get_reticle_params( &x, &y, &ang );
	pmath->set_reticle_params( x, y, val );

	// update overlay
	pmain_wnd->update_video_out();
}


void rcalibration::onStartReticleCalibrationButtonClick()
{
	FILE *pt;	// The file is for iAstroHub outputing to web UI.

	if( !check_start_position() )
	{
		log_i( "Start point is too close to matrix edge(s)" );
		// u_msg( "Start point is too close to matrix edge(s)" );  <- comment by Anat
		/* Add to support iAstroHub, by Anat & Zachary */
		pt=fopen("/home/pi/www/calibrating.txt","w");
        fprintf(pt,"Start point is too close to edge(s).+++++\n");
        fclose(pt);
		/* iAstroHub patch ends */
	
		return;
	}

	// manual
	if( ui.checkBox_AutoMode->checkState() != Qt::Checked )
	{
		calibrate_reticle_manual();
		return;
	}

	// automatic
	if( ui.checkBox_TwoAxis->checkState() == Qt::Checked )
		calibrate_reticle_by_ra_dec();	
	else
		calibrate_reticle_by_ra();
}


void rcalibration::onStartVideoCalibrationButtonClick()
{
	if( !is_started )
    {
		ui.groupBox_ReticleCalibration->setEnabled( false );	
		ui.pushButton_StartVideoCalibration->setText( tr("Stop") );
		ui.l_VStatus->setText( tr("State: accumulating frames...") );
		//ui.pushButton_StartVideoCalibration->setEnabled( false );
	
		pmain_wnd->m_video->start_calibration( ui.spinBox_FrameCount->value() );

		is_started = true;
	}
	else
	{
		ui.groupBox_ReticleCalibration->setEnabled( true );
		ui.pushButton_StartVideoCalibration->setText( tr("Start") );
		ui.l_VStatus->setText( tr("State: Cancelled") );
		//ui.pushButton_StartVideoCalibration->setEnabled( true );

		pmain_wnd->m_video->cancel_calibration();

		is_started = false;
	}
}


void rcalibration::onVideoCalibrationFinished()
{
	ui.groupBox_ReticleCalibration->setEnabled( true );
	ui.pushButton_StartVideoCalibration->setEnabled( true );
	ui.l_VStatus->setText( tr("State: DONE") );
	ui.pushButton_StartVideoCalibration->setText( tr("Start") );

	is_started = false;
}


void rcalibration::calibrate_reticle_manual( void )
{
	//----- manual mode ----
	// get start point
	if( !is_started )
	{
		if( ui.checkBox_TwoAxis->checkState() == Qt::Checked )
		{
			ui.pushButton_StartCalibration->setText( tr("Stop RA") );
		}
		else
		{
			ui.pushButton_StartCalibration->setText( tr("Stop") );
		}
		ui.l_RStatus->setText( tr("State: RA drifting...") );
		ui.checkBox_TwoAxis->setEnabled( false );
		ui.checkBox_AutoMode->setEnabled( false );
		ui.groupBox_VideoCalibration->setEnabled( false );

		// drop speed info
		pmath->clear_speed_info();
		pmath->get_star_screen_pos( &start_x1, &start_y1 );
		
		axis = RA;
		is_started = true;
	}
	else	// get end point and calc orientation
	{
		if( ui.checkBox_TwoAxis->checkState() == Qt::Checked )
		{
			if( axis == RA )
			{
				pmath->get_star_screen_pos( &end_x1, &end_y1 );
				
				start_x2 = end_x1;
				start_y2 = end_y1;
				
				axis = DEC;
				
				ui.pushButton_StartCalibration->setText( tr("Stop DEC") );
				ui.l_RStatus->setText( tr("State: DEC drifting...") );	
				return;
			}
			else
			{
				pmath->get_star_screen_pos( &end_x2, &end_y2 );	
				// calc orientation
				bool swap_dec = false;
				if( pmath->calc_and_set_reticle2( start_x1, start_y1, end_x1, end_y1, start_x2, start_y2, end_x2, end_y2, &swap_dec ) )
				{
					if( swap_dec )
					{
						pmain_wnd->m_driver->swap_dec_bits();
						log_i( "DEC control bits swapped" );
					}
					fill_interface();
					ui.l_RStatus->setText( tr("State: DONE") );
				}
				else
				{
					ui.l_RStatus->setText( tr("State: ERR") );
					QMessageBox::warning( this, tr("Error"), tr("Calibration rejected. Start drift is too short."), QMessageBox::Ok );
				}
			}
		}
		else
		{
			pmath->get_star_screen_pos( &end_x1, &end_y1 );

			if( pmath->calc_and_set_reticle( start_x1, start_y1, end_x1, end_y1 ) )
			{
				fill_interface();
				ui.l_RStatus->setText( tr("State: DONE") );
			}
			else
			{
				ui.l_RStatus->setText( tr("State: ERR") );
				QMessageBox::warning( this, tr("Error"), tr("Calibration rejected. Start drift is too short."), QMessageBox::Ok );
			}
		}

		ui.checkBox_TwoAxis->setEnabled( true );
		ui.checkBox_AutoMode->setEnabled( true );
		ui.pushButton_StartCalibration->setText( tr("Start") );
		ui.groupBox_VideoCalibration->setEnabled( true );

		is_started = false;
	}
}


void rcalibration::calibrate_reticle_by_ra( void )
{
	time_t start_time, t, cur;
	time_t ra_drift_tm = 0;
	bool auto_term_ok = false;
	int turn_back_time;
	int cnt;
	FILE *pt;	// The file is for iAstroHub.

	if( !pmath )
		return;

	//----- automatic mode -----
	auto_drift_time = ui.spinBox_DriftTime->value();
	turn_back_time = auto_drift_time + auto_drift_time / 2;
	cnt = 0;
	
	ui.checkBox_TwoAxis->setEnabled( false );
	ui.pushButton_FindStar->setEnabled( false );
	ui.checkBox_AutoMode->setEnabled( false );
	ui.progressBar->setMaximum( auto_drift_time*2 );
	ui.progressBar->setValue( 0 );
	ui.pushButton_StartCalibration->setEnabled( false );
	ui.l_RStatus->setText( tr("State: drifting...") );
	/* Add to support iAstroHub, by Anat & Zachary */
	pt=fopen("/home/pi/www/calibrating.txt","w");
	fprintf(pt,"State: drifting...\n");
	fclose(pt);
	/* iAstroHub patch ends */
	ui.groupBox_VideoCalibration->setEnabled( false );

	// drop info
	pmath->clear_speed_info();

	// get start point
	pmath->get_star_screen_pos( &start_x1, &start_y1 );

	// slow down RA drive to make drift
	pmain_wnd->m_driver->do_pulse( io_drv::RA_INC_DIR, auto_drift_time*1000 );

	t = start_time = time( NULL );
	while( (cur = time( NULL )) - start_time < auto_drift_time+1 )
	{
		if( cur != t )
		{
			ui.progressBar->setValue( ++cnt );
			t = cur;
		}
		QApplication::processEvents();
		usleep(LOOP_DELAY);
		if( !isVisible() )
			return;

		double cur_x, cur_y;
		pmath->get_star_screen_pos( &cur_x, &cur_y );
		if( !pmath->is_valid_pos( cur_x, cur_y ) ) // star is near screen edge! stop drive
		{
			pmain_wnd->m_driver->reset();
			log_i( "Star is near matrix edge. Stopping drive" );
			/* Add to support iAstroHub, by Anat & Zachary */
			pt=fopen("/home/pi/www/calibrating.txt","a");
			fprintf(pt,"Star is near the edge. Stopping drive\n");
			fclose(pt);
			/* iAstroHub patch ends */
			break;
		}
	}
	ra_drift_tm = cur - start_time;

	// get end point and calc orientation
	pmath->get_star_screen_pos( &end_x1, &end_y1 );

	// accelerate RA drive to return to start position
	pmain_wnd->m_driver->do_pulse( io_drv::RA_DEC_DIR, turn_back_time*1000 );

	ui.l_RStatus->setText( tr("State: running...") );
	/* Add to support iAstroHub, by Anat & Zachary */
	pt=fopen("/home/pi/www/calibrating.txt","a");
	fprintf(pt,"State: running back...\n");
	fclose(pt);
	/* iAstroHub patch ends */

	double phi = pmath->calc_phi( start_x1, start_y1, end_x1, end_y1 );
	Matrix ROT_Z = RotateZ( -M_PI*phi/180.0 ); // derotates...

	// wait until returning
	t = start_time = time( NULL );
	while( (cur = time( NULL )) - start_time < turn_back_time )
	{
		if( cur != t )
		{
			ui.progressBar->setValue( ++cnt );
			t = cur;
		}
		QApplication::processEvents();
		usleep(LOOP_DELAY);
		if( !isVisible() )
			return;

		//----- Z-check (new!) -----
		double cur_x, cur_y;
		pmath->get_star_screen_pos( &cur_x, &cur_y );
		Vector star_pos = Vector( cur_x, cur_y, 0 ) - Vector( start_x1, start_y1, 0 );
		star_pos.y = -star_pos.y;
		star_pos = star_pos * ROT_Z;

		// start point reached... so exit
		if( star_pos.x <= 0 )
		{
			pmain_wnd->m_driver->reset();
			auto_term_ok = true;
			/* Add to support iAstroHub, by Anat & Zachary */
			pt=fopen("/home/pi/www/calibrating.txt","a");
			fprintf(pt,"State: Start point reached...\n");
			fclose(pt);
			/* iAstroHub patch ends */
			break;
		}
		//----- Z-check end -----
	}
	ra_drift_tm = std::max( cur - start_time, ra_drift_tm );

	if( !auto_term_ok )
	{
		// QMessageBox::warning( this, tr("Warning"), tr("RA: Scope can't reach the start point in ") + QString().number(turn_back_time) + tr("secs.\nPossible mount or drive problems..."), QMessageBox::Ok );  <- Comment by Anat
		/* Add to support iAstroHub, by Anat & Zachary */
		pt=fopen("/home/pi/www/calibrating.txt","a");
		fprintf(pt,"Warning: Scope can't reach the start point! Possible mount or drive problems...\n");
		fclose(pt);
		/* iAstroHub patch ends */
	}

	// calc orientation
	if( pmath->calc_and_set_reticle( start_x1, start_y1, end_x1, end_y1, ra_drift_tm ) )
	{
		fill_interface();
		ui.l_RStatus->setText( tr("State: DONE") );
		/* Add to support iAstroHub, by Anat & Zachary */
		pt=fopen("/home/pi/www/calibrating.txt","a");
		fprintf(pt,"State: DONE +++++\n");
		fclose(pt);
		/* iAstroHub patch ends */
	}
	else
	{
		ui.l_RStatus->setText( tr("State: ERR") );
		// QMessageBox::warning( this, tr("Error"), tr("Calibration rejected. Start drift is too short."), QMessageBox::Ok );  <- Comment by Anat
		/* Add to support iAstroHub, by Anat & Zachary */
		pt=fopen("/home/pi/www/calibrating.txt","a");
		fprintf(pt,"Error: Calibration rejected. Start drift is too short.+++++\n");
		fclose(pt);
		/* iAstroHub patch ends */
	}

	ui.checkBox_TwoAxis->setEnabled( true );
	ui.pushButton_FindStar->setEnabled( true );
	ui.checkBox_AutoMode->setEnabled( true );
	ui.pushButton_StartCalibration->setEnabled( true );
	ui.groupBox_VideoCalibration->setEnabled( true );
	
}


void rcalibration::calibrate_reticle_by_ra_dec( void )
{
	time_t start_time, t, cur;
	time_t ra_drift_tm = 0, dec_drift_tm = 0;
	bool auto_term_ok = false;
	int turn_back_time;
	int cnt;
	FILE *pt;	// The file is for iAstroHub.


	if( !pmath )
		return;

	//----- automatic mode -----
	auto_drift_time = ui.spinBox_DriftTime->value();
	turn_back_time = auto_drift_time + auto_drift_time / 2;
	cnt = 0;
	
	ui.checkBox_TwoAxis->setEnabled( false );
	ui.pushButton_FindStar->setEnabled( false );
	ui.checkBox_AutoMode->setEnabled( false );
	ui.progressBar->setMaximum( auto_drift_time*4 );
	ui.progressBar->setValue( 0 );
	ui.pushButton_StartCalibration->setEnabled( false );
	ui.groupBox_VideoCalibration->setEnabled( false );

	// drop info
	pmath->clear_speed_info();

	//----- RA axis -----
	{
		ui.l_RStatus->setText( tr("State: RA drifting...") );
		/* Add to support iAstroHub, by Anat & Zachary */
		pt=fopen("/home/pi/www/calibrating.txt","w");
        fprintf(pt,"State: RA drifting...\n");
		fclose(pt);
		/* iAstroHub patch ends */
		
		// get start point by
		pmath->get_star_screen_pos( &start_x1, &start_y1 );

		// slow down RA drive to make drift
		pmain_wnd->m_driver->do_pulse( io_drv::RA_INC_DIR, auto_drift_time*1000 );

		t = start_time = time( NULL );
		while( (cur = time( NULL )) - start_time < auto_drift_time+1 )
		{
			if( cur != t )
			{
				ui.progressBar->setValue( ++cnt );
				t = cur;
			}
			QApplication::processEvents();
			usleep(LOOP_DELAY);
			if( !isVisible() )
				return;

			double cur_x, cur_y;
			pmath->get_star_screen_pos( &cur_x, &cur_y );
			if( !pmath->is_valid_pos( cur_x, cur_y ) ) // star is near screen edge! stop drive
			{
				pmain_wnd->m_driver->reset();
				log_i( "Star is near matrix edge. Stopping drive" );
				/* Add to support iAstroHub, by Anat & Zachary */
				pt=fopen("/home/pi/www/calibrating.txt","a");
				fprintf(pt,"RA: Star is near the edge. Stopping drive\n");
				fclose(pt);
				/* iAstroHub patch ends */
				break;
			}
		}
		ra_drift_tm = cur - start_time;

		// get end point and calc orientation
		pmath->get_star_screen_pos( &end_x1, &end_y1 );

		// accelerate RA drive to return to start position
		pmain_wnd->m_driver->do_pulse( io_drv::RA_DEC_DIR, turn_back_time*1000 );

		ui.l_RStatus->setText( tr("State: RA running back...") );
		/* Add to support iAstroHub, by Anat & Zachary */
		pt=fopen("/home/pi/www/calibrating.txt","a");
		fprintf(pt,"State: RA running back...\n");
		fclose(pt);
		/* iAstroHub patch ends */

		double phi = pmath->calc_phi( start_x1, start_y1, end_x1, end_y1 );
		Matrix ROT_Z = RotateZ( -M_PI*phi/180.0 ); // derotates...

		// wait until returning
		t = start_time = time( NULL );
		while( (cur = time( NULL )) - start_time < turn_back_time )
		{
			if( cur != t )
			{
				ui.progressBar->setValue( ++cnt );
				t = cur;
			}
			QApplication::processEvents();
			usleep(LOOP_DELAY);
			if( !isVisible() )
				return;

			//----- Z-check (new!) -----
			double cur_x, cur_y;
			pmath->get_star_screen_pos( &cur_x, &cur_y );
			Vector star_pos = Vector( cur_x, cur_y, 0 ) - Vector( start_x1, start_y1, 0 );
			star_pos.y = -star_pos.y;
			star_pos = star_pos * ROT_Z;

			// start point reached... so exit
			if( star_pos.x <= 0 )
			{
				pmain_wnd->m_driver->reset();
				auto_term_ok = true;
				/* Add to support iAstroHub, by Anat & Zachary */
				pt=fopen("/home/pi/www/calibrating.txt","a");
				fprintf(pt,"State: RA: Start point reached...\n");
				fclose(pt);
				/* iAstroHub patch ends */
				break;
			}
			//----- Z-check end -----
		}
		ra_drift_tm = std::max( cur - start_time, ra_drift_tm );

		if( !auto_term_ok )
		{
			// QMessageBox::warning( this, tr("Warning"), tr("RA: Scope can't reach the start point in ") + QString().number(turn_back_time) + tr("secs.\nPossible mount or drive problems..."), QMessageBox::Ok );  <- Comment by Anat
			/* Add to support iAstroHub, by Anat & Zachary */
			pt=fopen("/home/pi/www/calibrating.txt","a");
			fprintf(pt,"Warning: RA: Scope can't reach the start point! Possible mount or drive problems...\n");
			fclose(pt);
			/* iAstroHub patch ends */
		}
	}

	//----- DEC axis -----
	{
		ui.l_RStatus->setText( tr("State: DEC drifting...") );
		/* Add to support iAstroHub, by Anat & Zachary */
		pt=fopen("/home/pi/www/calibrating.txt","a");
		fprintf(pt,"State: DEC drifting...\n");
		fclose(pt);
		/* iAstroHub patch ends */
		
		// get start point by
		pmath->get_star_screen_pos( &start_x2, &start_y2 );

		// DEC+ drive to make drift
		pmain_wnd->m_driver->do_pulse( io_drv::DEC_INC_DIR, auto_drift_time*1000 );

		t = start_time = time( NULL );
		while( (cur = time( NULL )) - start_time < auto_drift_time+1 )
		{
			if( cur != t )
			{
				ui.progressBar->setValue( ++cnt );
				t = cur;
			}
			QApplication::processEvents();
			usleep(LOOP_DELAY);
			if( !isVisible() )
				return;

			double cur_x, cur_y;
			pmath->get_star_screen_pos( &cur_x, &cur_y );
			if( !pmath->is_valid_pos( cur_x, cur_y ) ) // star is near screen edge! stop drive
			{
				pmain_wnd->m_driver->reset();
				log_i( "Star is near matrix edge. Stopping drive" );
				/* Add to support iAstroHub, by Anat & Zachary */
				pt=fopen("/home/pi/www/calibrating.txt","a");
				fprintf(pt,"DEC: Star is near the edge. Stopping drive\n");
				fclose(pt);
				/* iAstroHub patch ends */
				break;
			}
		}
		dec_drift_tm = cur - start_time;

		// get end point and calc orientation
		pmath->get_star_screen_pos( &end_x2, &end_y2 );

		// DEC- drive to return to start position
		pmain_wnd->m_driver->do_pulse( io_drv::DEC_DEC_DIR, turn_back_time*1000 );

		ui.l_RStatus->setText( tr("State: DEC running back...") );
		/* Add to support iAstroHub, by Anat & Zachary */
		pt=fopen("/home/pi/www/calibrating.txt","a");
		fprintf(pt,"State: DEC running back...\n");
		fclose(pt);
		/* iAstroHub patch ends */

		double phi = pmath->calc_phi( start_x2, start_y2, end_x2, end_y2 );
		Matrix ROT_Z = RotateZ( -M_PI*phi/180.0 ); // derotates...

		// wait until returning
		t = start_time = time( NULL );
		while( (cur = time( NULL )) - start_time < turn_back_time )
		{
			if( cur != t )
			{
				ui.progressBar->setValue( ++cnt );
				t = cur;
			}
			QApplication::processEvents();
			usleep(LOOP_DELAY);
			if( !isVisible() )
				return;

			//----- Z-check (new!) -----
			double cur_x, cur_y;
			pmath->get_star_screen_pos( &cur_x, &cur_y );
			Vector star_pos = Vector( cur_x, cur_y, 0 ) - Vector( start_x2, start_y2, 0 );
			star_pos.y = -star_pos.y;
			star_pos = star_pos * ROT_Z;

			// start point reached... so exit
			if( star_pos.x <= 0 )
			{
				pmain_wnd->m_driver->reset();
				auto_term_ok = true;
				/* Add to support iAstroHub, by Anat & Zachary */
				pt=fopen("/home/pi/www/calibrating.txt","a");
				fprintf(pt,"State: DEC: Start point reached...\n");
				fclose(pt);
				/* iAstroHub patch ends */
				break;
			}
			//----- Z-check end -----
		}
		dec_drift_tm = std::max( cur - start_time, dec_drift_tm );

		if( !auto_term_ok )
		{
			// QMessageBox::warning( this, tr("Warning"), tr("DEC: Scope can't reach the start point in ") + QString().number(turn_back_time) + tr("secs.\nPossible mount or drive problems..."), QMessageBox::Ok );  <- Comment by Anat
			/* Add to support iAstroHub, by Anat & Zachary */
			pt=fopen("/home/pi/www/calibrating.txt","a");
			fprintf(pt,"Warning: DEC: Scope can't reach the start point! Possible mount or drive problems...\n");
			fclose(pt);
			/* iAstroHub patch ends */
		}
	}

	log_i("RA: sx1=%f sy1=%f ex1=%f ey1=%f DEC: sx2=%f sy2=%f ex2=%f ey2=%f", start_x1, start_y1, end_x1, end_y1, start_x2, start_y2, end_x2, end_y2);
	/* Add to support iAstroHub, by Anat & Zachary */
	pt=fopen("/home/pi/www/calibrating.txt","a");
	fprintf(pt,"RA: sx1=%f sy1=%f ex1=%f ey1=%f DEC: sx2=%f sy2=%f ex2=%f ey2=%f\n", start_x1, start_y1, end_x1, end_y1, start_x2, start_y2, end_x2, end_y2);
	fclose(pt);
	/* iAstroHub patch ends */

	// calc orientation
	bool swap_dec = false;
	if( pmath->calc_and_set_reticle2( start_x1, start_y1, end_x1, end_y1, start_x2, start_y2, end_x2, end_y2, &swap_dec, ra_drift_tm, dec_drift_tm ) )
	{
		if( swap_dec )
		{
			pmain_wnd->m_driver->swap_dec_bits();
			log_i( "DEC control bits swapped" );
			/* Add to support iAstroHub, by Anat & Zachary */
			pt=fopen("/home/pi/www/calibrating.txt","a");
			fprintf(pt,"DEC control bits swapped\n");
			fclose(pt);
			/* iAstroHub patch ends */
		}

		fill_interface();
		ui.l_RStatus->setText( tr("State: DONE") );
		/* Add to support iAstroHub, by Anat & Zachary */
		pt=fopen("/home/pi/www/calibrating.txt","a");
		fprintf(pt,"State: DONE +++++\n");
		fclose(pt);
		/* iAstroHub patch ends */
	}
	else
	{
		ui.l_RStatus->setText( tr("State: ERR") );
		// QMessageBox::warning( this, tr("Error"), tr("Calibration rejected. Start drift is too short."), QMessageBox::Ok );  <- Comment by Anat
		/* Add to support iAstroHub, by Anat & Zachary */
		pt=fopen("/home/pi/www/calibrating.txt","a");
		fprintf(pt,"Error: Calibration rejected. Start drift is too short.+++++\n");
		fclose(pt);
		/* iAstroHub patch ends */
	}

	ui.checkBox_TwoAxis->setEnabled( true );
	ui.pushButton_FindStar->setEnabled( true );
	ui.checkBox_AutoMode->setEnabled( true );
	ui.pushButton_StartCalibration->setEnabled( true );
	ui.groupBox_VideoCalibration->setEnabled( true );

}


bool rcalibration::check_start_position( void ) const
{
	if( !pmath )
		return false;

	double cur_x, cur_y;
	pmath->get_star_screen_pos( &cur_x, &cur_y );

	return pmath->is_valid_pos( cur_x, cur_y, FIND_STAR_CLIP_EDGE );
}


void rcalibration::onFindStarButtonClick()
{
	FILE *pt;	// The file is for iAstroHub.

	std::vector< std::pair<Vector, double> > stars;

	if( !pmath )
		return;

	bool res = pmath->find_stars( &stars );
	if( !res )
	{
		// u_msg( "No suitable star in frame" );
		/* Add to support iAstroHub, by Anat & Zachary */
		pt=fopen("/home/pi/www/status_guidestar.txt","w");
		fprintf(pt,"No suitable star in frame\n");
		fclose(pt);
		/* iAstroHub patch ends */
		return;
	}

	double k_min = std::numeric_limits<double>::max();
	std::vector< std::pair<Vector, double> >::const_iterator opt_it;
	for( std::vector< std::pair<Vector, double> >::const_iterator it = stars.begin();
		it != stars.end(); ++it )
	{
		double star_z   = (double)it->first.z;
		double star_lum = (double)it->second;
		double k = star_z / (star_lum > 0 ? star_lum : 1);
		if( k < k_min )
		{
			k_min = k;
			opt_it = it;
		}
	}

	double star_x   = (double)opt_it->first.x;
	double star_y   = (double)opt_it->first.y;

	int idx = pmath->get_square_index();
	pmath->move_square( star_x - (double)guide_squares[idx].size/2, star_y - (double)guide_squares[idx].size/2 );
}
