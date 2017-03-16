<?

// header('Content-type: text/html');

passthru("sudo cp /home/pi/www/move_in /home/pi/www/move_tmp");
passthru("sudo cp /home/pi/www/move_out /home/pi/www/move_in");
passthru("sudo cp /home/pi/www/move_tmp /home/pi/www/move_out");

?>
