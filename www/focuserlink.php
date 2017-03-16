<?

if (!file_exists('status_ccd')) {

header('Content-type: text/html');

if (file_exists('/home/pi/www/focuserlink')) {
	passthru("sudo rm /home/pi/www/focuserlink");
	echo "DDDDD";
} else {
	$file=fopen("/home/pi/www/focuserlink","w");
	fclose($file);
	echo "AAAAA";
}

}

?> 
