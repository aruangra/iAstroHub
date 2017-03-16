<?

$file=fopen("/home/pi/www/ccd_delay.txt","w");
fwrite($file, $_GET['delay']);
fclose($file);

?>
