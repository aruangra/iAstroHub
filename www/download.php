<?

header('Content-type: text/html');

if (file_exists('/home/pi/www/download')) {
	passthru("sudo rm /home/pi/www/download");
	echo "DDDDD";
} else {
	$file=fopen("/home/pi/www/download","w");
	fclose($file);
	echo "AAAAA";
}

?> 
