<?

if (!file_exists('status_ccd')) {

$file=fopen("/home/pi/www/hist_L.txt","w");
fwrite($file, '0 ');
fclose($file);

$file=fopen("/home/pi/www/hist_U.txt","w");
fwrite($file, '65535 ');
fclose($file);

$file=fopen("/home/pi/www/hist_A.txt","w");
fwrite($file, '1');
fclose($file);

passthru("sudo sh /home/pi/www/hist.sh");

header('Content-type: text/html');

passthru("sudo /home/pi/www/./hist_check");

} else {

echo "XXXXX";

}

?>
