<?

// header('Content-type: text/html');

$file=fopen("/home/pi/www/skychart/center.txt","w");
fwrite($file, $_GET['x'] . "\n");
fwrite($file, $_GET['y'] . "\n");
fclose($file);

passthru("sudo perl center.pl");

passthru("sudo perl getra.pl");
passthru("sudo perl getdec.pl");

passthru("sudo perl save_img.pl");

?>
