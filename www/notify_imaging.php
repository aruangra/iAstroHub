<?

header('Content-type: text/html');

if (file_exists('/home/pi/www/notify_imaging')) {
	passthru("sudo rm /home/pi/www/notify_imaging");
	echo "DDDDD";
} else {
	$file=fopen("/home/pi/www/notify_imaging","w");
	fclose($file);
	echo "AAAAA";
}

?> 
