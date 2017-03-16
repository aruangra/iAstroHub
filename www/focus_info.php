<?

header('Content-type: text/html');

if (file_exists('status_ccd') && file_exists('focuserlink')) {

		passthru("sudo perl /home/pi/www/focus_info_focuserlink.pl");
		passthru("tail -n 20 /home/pi/www/focus_info.txt | tac");

} else {

	if (!file_exists('status_focus')) {

		passthru("sudo /home/pi/www/./focus_update");
		passthru("sudo /home/pi/www/./focus_temp");
		passthru("sudo perl /home/pi/www/focus_info.pl");
		passthru("tail -n 20 /home/pi/www/focus_info.txt | tac");
		
	}

}

?>
