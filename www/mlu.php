<?

header('Content-type: text/html');

if (file_exists('/home/pi/www/mlu')) {
	passthru("sudo rm /home/pi/www/mlu");
	echo "DDDDD";
} else {
	$file=fopen("/home/pi/www/mlu","w");
	fclose($file);
	echo "AAAAA";
}

?> 
