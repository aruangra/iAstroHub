<?

$file=fopen("/home/pi/www/ccd_offset.txt","w");
fwrite($file, $_GET['offset']);
fclose($file);

if (file_exists('/tmp/gtkImager')) {
	$offset=(int)$_GET['offset'];
	passthru("echo CAMOFFSET:" . $offset . " > /tmp/gtkImager");	
}

?>
