<?

header('Content-type: text/html');

if (file_exists('/home/pi/www/SoftSync')) {
	passthru("sudo rm /home/pi/www/SoftSync");
	echo "DDDDD";
} else {
	$file=fopen("/home/pi/www/SoftSync","w");
	fclose($file);
	echo "AAAAA";
}

?> 
