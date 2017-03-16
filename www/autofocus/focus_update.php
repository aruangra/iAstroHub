<?

header('Content-type: text/html');

if (!file_exists('../status_focus')) {

	passthru("sudo /home/pi/www/./focus_update");

	passthru("sudo cat /home/pi/www/current_pos.txt");

} else {

	echo "Moving...";

}




?>
