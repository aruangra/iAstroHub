<?

header('Content-type: text/html');

passthru("cat /home/pi/www/hist_L.txt /home/pi/www/hist_U.txt");

?> 
