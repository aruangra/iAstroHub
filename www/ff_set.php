<?

// header('Content-type: text/html');

$file=fopen("/home/pi/www/ff_set.txt","w");
fwrite($file, $_GET['brightness']);
fclose($file);

passthru("echo 'Flip-Flat: Set Brightness' >> mask_log.txt");
passthru("sudo ./ff_set >> mask_log.txt");

?>
