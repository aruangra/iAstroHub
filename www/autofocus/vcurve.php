<?

if (!file_exists('../status_ccd') && !file_exists('../status_capture') && !file_exists('../status_focus')) {

passthru("echo CAMDSPDSET:1 > /tmp/gtkImager");

passthru("echo BASENAME:VCURVE > /tmp/gtkImager");
passthru("echo EXPTIME:" . $_GET['exposure']*1000 . " > /tmp/gtkImager");

$file=fopen("vcurve_point.txt","w");
fwrite($file, $_GET['point']);
fclose($file);

$file=fopen("../focus_move.txt","w");
fwrite($file, $_GET['move']);
fclose($file);

$file=fopen("vcurve_dir.txt","w");
fwrite($file, $_GET['dir']);
fclose($file);

$file=fopen("vcurve_size.txt","w");
fwrite($file, $_GET['size']);
fclose($file);

passthru("sudo rm /home/pi/www/autofocus/images/*.*");

passthru("sudo /home/pi/www/autofocus/./vcurve &");

}

// header('Content-type: text/html');

?>
