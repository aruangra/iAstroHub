<?

passthru("sudo /home/pi/www/./remote_client 13 > /dev/null 2>&1");

header('Content-type: text/html');

passthru("tail -1 /home/pi/www/get_gain.txt");

?> 
