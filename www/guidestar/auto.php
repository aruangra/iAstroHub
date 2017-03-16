<?

// header('Content-type: text/html');

if (!file_exists('/home/pi/www/status_guiding')) {

passthru("sudo /home/pi/www/./remote_client 9");

sleep(4);

passthru("sudo /home/pi/www/guidestar/./cmd_getimage");

sleep(1);

passthru("sudo cp /root/LGimage.bmp /home/pi/www/guidestar/LGimage.bmp");

echo "FFFFF";



} else {

echo "YYYYY";

}

?>
