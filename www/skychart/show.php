<?

// header('Content-type: text/html');

passthru("sudo perl mount_show.pl");

passthru("sudo perl getra.pl");
passthru("sudo perl getdec.pl");

passthru("sudo perl save_img.pl");

?>
