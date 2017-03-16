<?

passthru("sudo sh kill_ccd.sh");

$file=fopen("/home/pi/www/image.txt","a");
fwrite($file, "Waiting for CCD to be ready..." . "\n");
fclose($file);

if (file_exists('status_capture')) {
   passthru("echo STOP: > /tmp/gtkImager");
   passthru("echo STOP: > /tmp/gtkImager");
   passthru("sudo /home/pi/www/./check_idle &");
} else {
	if (file_exists('status_ccd')) {
	   passthru("sudo rm status_ccd");
	}
}



// header('Content-type: text/html');

?>
