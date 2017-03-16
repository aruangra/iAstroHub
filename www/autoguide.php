<?

if (!file_exists('status_guiding')) {

passthru("echo guiding > status_guiding");

passthru("sudo date >> guiding.txt");

passthru("sudo /home/pi/www/./remote_client 14");
passthru("sudo /home/pi/www/./remote_client 5");

}

// header('Content-type: text/html');

?>
