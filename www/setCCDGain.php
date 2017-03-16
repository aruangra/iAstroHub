<?

$file=fopen("/home/pi/www/ccd_gain.txt","w");
fwrite($file, $_GET['gain']);
fclose($file);

if (file_exists('/tmp/gtkImager')) {
	$gain=(int)$_GET['gain'];
	passthru("echo CAMGAIN:" . $gain . " > /tmp/gtkImager");
}

?>
