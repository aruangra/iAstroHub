<?

// header('Content-type: text/html');

if (!file_exists('/home/pi/www/status_guiding')) {



$file=fopen("/home/pi/www/guidestar/position.txt","w");
fwrite($file, $_GET['x']);
fwrite($file, ' ');
fwrite($file, $_GET['y']);
fwrite($file, ' ');
fclose($file);

passthru("sudo /home/pi/www/guidestar/./cmd_setposition");

sleep(3);

passthru("sudo /home/pi/www/guidestar/./cmd_getimage");

sleep(1);

passthru("sudo cp /root/LGimage.bmp /home/pi/www/guidestar/LGimage.bmp");

echo "FFFFF";



} else {

echo "YYYYY";

}

?>
