<?

if (file_exists('status_app')) {

if (!file_exists('status_connect')) {

$file=fopen("status_connect","w");
fwrite($file, "connect");
fclose($file);

passthru("sudo rm /home/pi/www/tmp_images/*.*");

passthru("echo CAMSET:" . $_GET['camid'] . " > /tmp/gtkImager");
passthru("echo CAMCONNECT:1 > /tmp/gtkImager");

passthru("echo CAMBAYERSET:0 > /tmp/gtkImager");
passthru("echo CAMDEPTHSET:0 > /tmp/gtkImager");


$file=fopen("/home/pi/www/ccd_offset.txt","r");
$offset=fgets($file, 10);
$offset=(int)$offset;
fclose($file);

passthru("echo CAMOFFSET:" . $offset . " > /tmp/gtkImager");

$file=fopen("/home/pi/www/ccd_gain.txt","r");
$gain=fgets($file, 10);
$gain=(int)$gain;
fclose($file);

passthru("echo CAMGAIN:" . $gain . " > /tmp/gtkImager");


passthru("echo BASEFOLDER:/home/pi/www/tmp_images > /tmp/gtkImager");
passthru("echo BASENAME:IMG > /tmp/gtkImager");


passthru("echo CFWMODESET:3 > /tmp/gtkImager");
passthru("echo CFWCONNECT:1 > /tmp/gtkImager");
passthru("echo 'CFWSETFILTERS:R|G|B|L|Ha|S2|O3|Hb' > /tmp/gtkImager");
passthru("echo FLTADD:1 > /tmp/gtkImager");

passthru("echo TECAUTO:1 > /tmp/gtkImager");
passthru("echo TECSPEED:7 > /tmp/gtkImager");

passthru("echo CAPMODE:1 > /tmp/gtkImager");
passthru("echo SETSAVEJPG:1 > /tmp/gtkImager");

}

}

// header('Content-type: text/html');

?>
