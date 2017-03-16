<?

header('Content-type: text/html');

if (file_exists('/home/pi/www/tts')) {
	passthru("sudo rm /home/pi/www/tts");
	echo "DDDDD";
} else {
	$file=fopen("/home/pi/www/tts","w");
	fclose($file);
	echo "AAAAA";
}

?> 
