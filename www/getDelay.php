<?

header('Content-type: text/html');

passthru("cat /home/pi/www/ccd_delay.txt");

?> 
