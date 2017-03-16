<?

header('Content-type: text/html');

if (file_exists('/home/pi/www/notify_guiding')) {
	passthru("sudo rm /home/pi/www/notify_guiding");
	echo "DDDDD";
} else {
	$file=fopen("/home/pi/www/notify_guiding","w");
	fclose($file);
	echo "AAAAA";
}

?> 
