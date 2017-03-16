<?

// header('Content-type: text/html');

if (!file_exists('/home/pi/www/status_guiding')) {

$file=fopen("/home/pi/www/guidestar/set_videogain.txt","w");
fwrite($file, $_GET['value']);
fwrite($file, ' ');
fclose($file);

passthru("sudo /home/pi/www/./cmd_setvideogain");

sleep(4);

passthru("sudo /home/pi/www/guidestar/./cmd_getimage");

sleep(1);

passthru("sudo cp /root/LGimage.bmp /home/pi/www/guidestar/LGimage.bmp");

echo "FFFFF";



} else {

echo "YYYYY";

}



?>
