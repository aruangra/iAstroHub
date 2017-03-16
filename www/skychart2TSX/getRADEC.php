<?

// header('Content-type: text/html');

passthru("sudo /home/pi/www/skychart2TSX/./getRADEC");

passthru("cat /home/pi/www/skychart2TSX/mount_RADEC.txt");

?>
