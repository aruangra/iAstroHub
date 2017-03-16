<?

// header('Content-type: text/html');

passthru("sudo date +%Y%m%d -s " . $_GET['date']);

passthru("sudo date +%T -s " . $_GET['time']);


passthru("sudo perl redraw.pl");

passthru("sudo perl getra.pl");
passthru("sudo perl getdec.pl");

passthru("sudo perl save_img.pl");

?>
