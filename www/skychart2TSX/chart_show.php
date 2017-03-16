<?

// header('Content-type: text/html');

$file=fopen("/home/pi/www/skychart2TSX/chart_RA.txt","w");
fwrite($file, $_GET['RA']);
fclose($file);

$file=fopen("/home/pi/www/skychart2TSX/chart_DEC.txt","w");
fwrite($file, $_GET['DEC']);
fclose($file);

passthru("sudo perl chart_show.pl");

passthru("sudo perl getra.pl");
passthru("sudo perl getdec.pl");

passthru("sudo perl save_img.pl");

?>
