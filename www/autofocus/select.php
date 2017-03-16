<?

if (!file_exists('../status_ccd') && !file_exists('../status_capture')) {

passthru("echo SETROISIZE:" . $_GET['size'] . " > /tmp/gtkImager");
passthru("echo SETROIPOS:" . $_GET['x'] . " " . $_GET['y'] . " > /tmp/gtkImager");
//passthru("echo GETROIPOS: > /tmp/gtkImager");

sleep(1);

$file=fopen("/tmp/roipos.txt","r");
$pos_x = fgets($file, 10);
$pos_y = fgets($file, 10);
fclose($file);

$x=(int)$pos_x-(int)$_GET['size']/2;
$y=(int)$pos_y-(int)$_GET['size']/2;

passthru("convert /home/pi/www/autofocus/tmp_images/preview.jpg -crop " . $_GET['size'] . "x" . $_GET['size'] . "+" . $x . "+" . $y . " /home/pi/www/autofocus/tmp_images/crop.jpg"); 

passthru("echo GETFWHM: > /tmp/gtkImager");

sleep(1);

$file=fopen("/tmp/fwhm.txt","r");
$fwhm = fgets($file, 10);
fclose($file);

header('Content-type: text/html');

echo $fwhm;

}



?>
