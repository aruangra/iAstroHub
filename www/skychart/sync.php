<?

// header('Content-type: text/html');

passthru("sudo /home/pi/www/skychart/./sync");

passthru("sudo /home/pi/www/skychart/./getRADEC");

?>
