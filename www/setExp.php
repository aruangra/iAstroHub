<?

// header('Content-type: text/html');

$file=fopen("/home/pi/www/set_fps.txt","w");
fwrite($file, $_GET['exp']);
fwrite($file, ' ');
fclose($file);

passthru("sudo /home/pi/www/./cmd_setfps");

?>
