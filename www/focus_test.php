<?

header('Content-type: text/html');

while (!file_exists('status_focus')) {

	passthru("sudo /home/pi/www/./focus_update");

	passthru("sudo /home/pi/www/./focus_temp");

	passthru("sudo perl /home/pi/www/focus_info.pl");

	sleep(2);

}


?>
