/*
 * rcalibration.h
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

#ifndef RETICLE_CALIBRATION_H
#define RETICLE_CALIBRATION_H

#include <QtGui/QDialog>
#include "ui_rcalibration.h"
#include "gmath.h"

typedef struct
{
	bool two_axis;
	bool auto_mode;
	int  dift_time;
	int  frame_count;
}calibrationparams_t;


class lin_guider;


class rcalibration : public QDialog
{
    Q_OBJECT

public:
    rcalibration(lin_guider *parent = 0);
    ~rcalibration();

    bool set_video_params( int vid_wd, int vid_ht );
    void update_reticle_pos( double x, double y );
    void set_math( cgmath *math );

public slots:	// Changed from protected to public for iAstroHub
	void onSquareSizeChanged( int index );
	void onTwoAxisMode( int state );
	void onEnableAutoMode( int state );
	void onDriftTimeChanged( int val );
	void onReticleXChanged( double val );
	void onReticleYChanged( double val );
	void onReticleAngChanged( double val );

	void onStartReticleCalibrationButtonClick();
	void onStartVideoCalibrationButtonClick();

	void onVideoCalibrationFinished();

	void onFindStarButtonClick();

protected:
	void showEvent ( QShowEvent * event );
	void closeEvent( QCloseEvent *event );
	void hideEvent ( QHideEvent * event );

private:
	void fill_interface( void );
	void calibrate_reticle_manual( void );
	void calibrate_reticle_by_ra( void );     // 1-axis calibration
	void calibrate_reticle_by_ra_dec( void ); // 2-axis calibration
	bool check_start_position( void ) const;
	bool is_started;
	
	calibrationparams_t calibration_params;
	int  axis;
	int  auto_drift_time;
	double start_x1, start_y1;
	double end_x1, end_y1;
	double start_x2, start_y2;
	double end_x2, end_y2;

	cgmath *pmath;
	lin_guider *pmain_wnd;
private:
    Ui::rcalibrationClass ui;
};

#endif // RETICLE_CALIBRATION_H
