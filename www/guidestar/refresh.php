<?

// header('Content-type: text/html');

passthru("sudo /home/pi/www/guidestar/./cmd_getimage");

sleep(1);

passthru("sudo cp /root/LGimage.bmp /home/pi/www/guidestar/LGimage.bmp");

echo "FFFFF";

?>
