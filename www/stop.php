<?

passthru("sudo /home/pi/www/./remote_client 6");
passthru("sudo /home/pi/www/./remote_client 8");

if (file_exists('status_guiding')) {
   passthru("sudo rm status_guiding");
}

if (file_exists('status_calibrating')) {
   passthru("sudo rm status_calibrating");
}

// header('Content-type: text/html');

?>
