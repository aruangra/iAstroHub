<?

header('Content-type: text/html');

if (file_exists('/home/pi/www/hattrick')) {
	passthru("sudo rm /home/pi/www/hattrick");
	echo "DDDDD";
} else {
	$file=fopen("/home/pi/www/hattrick","w");
	fclose($file);
	echo "AAAAA";
}

?> 
