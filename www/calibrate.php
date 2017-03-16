<?

if (!file_exists('status_calibrating')) {

passthru("echo calibrating > status_calibrating");

$file=fopen("/home/pi/www/calibrating.txt","w");
fwrite($file, ' ');
fclose($file);

passthru("sudo /home/pi/www/./remote_client 7 &");

}

// header('Content-type: text/html');

?>
