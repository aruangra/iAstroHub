<?

// header('Content-type: text/html');

passthru("sudo /home/pi/www/skychart/./getRADEC");

passthru("cat /home/pi/www/skychart/mount_RADEC.txt");

?>
