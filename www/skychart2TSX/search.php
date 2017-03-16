<?

// header('Content-type: text/html');

passthru("sudo perl search.pl '" . $_GET['object'] . "' ");

passthru("sudo perl getra.pl");
passthru("sudo perl getdec.pl");

passthru("sudo perl save_img.pl");

?>
