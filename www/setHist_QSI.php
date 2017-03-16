<?

if (!file_exists('status_processing')) {

$file=fopen("/home/pi/www/hist_L.txt","w");
fwrite($file, $_GET['L']);
fwrite($file, ' ');
fclose($file);

$file=fopen("/home/pi/www/hist_U.txt","w");
fwrite($file, $_GET['U']);
fclose($file);

passthru("echo MINADU:" . $_GET['L'] . " > /tmp/gtkImager");
sleep(1);
passthru("echo MAXADU:" . $_GET['U'] . " > /tmp/gtkImager");
sleep(1);
passthru("sudo /home/pi/www/./minmaxadu_check");

passthru("echo GETPREVIEW: > /tmp/gtkImager");
sleep(1);

header('Content-type: text/html');

passthru("sudo /home/pi/www/./hist_check");

} else {

echo "XXXXX";

}

?>
