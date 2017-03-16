<?

passthru("sudo sh kill_focus.sh");

passthru("sudo /home/pi/www/./focus_stop");

if (file_exists('../status_focus')) {
   passthru("sudo rm ../status_focus");
}

sleep(1);

passthru("sudo /home/pi/www/./focus_update");

header('Content-type: text/html');

passthru("sudo cat /home/pi/www/current_pos.txt");

?>
