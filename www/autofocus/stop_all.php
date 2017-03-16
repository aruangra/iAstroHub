<?

// Stop Focuser

passthru("sudo sh kill_focus.sh");
passthru("sudo /home/pi/www/./focus_stop");
if (file_exists('../status_focus')) {
	passthru("sudo rm ../status_focus");
}



// Stop CCD

if (file_exists('../status_capture')) {
   passthru("echo STOP: > /tmp/gtkImager");
   passthru("echo STOP: > /tmp/gtkImager");
   passthru("sudo rm ../status_capture");
}

if (file_exists('../status_ccd')) {
   passthru("sudo rm ../status_ccd");
}

passthru("sudo sh kill_ccd.sh");


// Stop V-Curve

passthru("sudo sh kill_vcurve.sh");

?>
