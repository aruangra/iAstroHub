<?

if (!file_exists('prepare_ccd')) {

$file=fopen("/home/pi/www/image.txt","a");
fwrite($file, "Waiting for CCD to be ready..." . "\n");
fclose($file);

passthru("sudo sh kill_QSI.sh");

if (file_exists('status_processing')) {
	passthru("sudo rm status_processing");
}

passthru("echo btnCCDCancel > /home/pi/www/Commands.txt");

if (file_exists('status_capture')) {
	passthru("sudo rm status_capture");
	passthru("sudo /home/pi/www/./check_idle_QSI &");
} else {
	if (file_exists('status_ccd')) {
	   passthru("sudo rm status_ccd");
	}
	if (file_exists('focuserlink')) {
		passthru("echo 'open_focuser_comms 0' > /home/pi/www/Commands.txt");
	}
}

}

// header('Content-type: text/html');

?>
