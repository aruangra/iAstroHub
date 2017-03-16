<?

if (!file_exists('/home/pi/www/status_ccd')) {

passthru("echo HIDEROI: > /tmp/gtkImager");
passthru("echo CAMDSPDSET:0 > /tmp/gtkImager");

passthru("echo CAMXMODSET:0 > /tmp/gtkImager");

passthru("echo BASENAME:" . $_GET['filename'] . " > /tmp/gtkImager");
passthru("echo EXPTIME:" . $_GET['ex']*1000 . " > /tmp/gtkImager");

$file=fopen("/home/pi/www/ccd_nf.txt","w");
fwrite($file, $_GET['nf']);
fclose($file);

$file=fopen("/home/pi/www/ccd_filter.txt","w");
fwrite($file, $_GET['filter']);
fclose($file);

passthru("sudo /home/pi/www/autofocus/./ccdcontrol &");

} else {

header('Content-type: text/html');

echo "xxxxxxxxxx";

}

?>
