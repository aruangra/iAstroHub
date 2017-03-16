<?

if (file_exists('status_connect')) {

if (!file_exists('status_ccd')) {

passthru("echo CAMDSPDSET:0 > /tmp/gtkImager");

passthru("echo BASENAME:" . $_GET['filename'] . " > /tmp/gtkImager");
passthru("echo EXPTIME:" . $_GET['ex']*1000 . " > /tmp/gtkImager");

$file=fopen("ccd_nf.txt","w");
fwrite($file, $_GET['nf']);
fclose($file);

$file=fopen("ccd_filter.txt","w");
fwrite($file, $_GET['filter']);
fclose($file);

passthru("sudo /home/pi/www/./ccdcontrol &");

}

}

// header('Content-type: text/html');

?>
