<?

header('Content-type: text/html');

passthru("cat /home/pi/www/ccd_offset.txt");

?> 
