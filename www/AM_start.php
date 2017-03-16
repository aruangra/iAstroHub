<?

$file=fopen("/home/pi/www/astrometry.txt","w");
fwrite($file, ' ');
fclose($file);

$file=fopen("/home/pi/www/AM_FOV.txt","r");
$FOV=fgets($file, 10);
$FOV=(float)$FOV;
fclose($file);

passthru("sudo /usr/local/astrometry/bin/solve-field /home/pi/www/image.jpg --scale-low " . $FOV . " --overwrite --no-plots --downsample " . $_GET['downsample'] . " >  /home/pi/www/astrometry.txt &");

// header('Content-type: text/html');

?>
