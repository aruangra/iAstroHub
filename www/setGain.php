<?

// header('Content-type: text/html');

$file=fopen("/home/pi/www/set_gain.txt","w");
fwrite($file, $_GET['x']);
fwrite($file, ' ');
fwrite($file, $_GET['y']);
fwrite($file, ' ');
fclose($file);

passthru("sudo /home/pi/www/./cmd_setgain");

?>
