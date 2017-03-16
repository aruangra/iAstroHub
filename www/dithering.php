<?

header('Content-type: text/html');

if (file_exists('/home/pi/www/dithering')) {
	passthru("sudo rm /home/pi/www/dithering");
	echo "DDDDD";
} else {
	$file=fopen("/home/pi/www/dithering","w");
	fclose($file);
	echo "AAAAA";
}

?> 
