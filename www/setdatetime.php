<?

passthru("sudo date +%Y%m%d -s " . $_GET['date']);

passthru("sudo date +%T -s " . $_GET['time']);

// header('Content-type: text/html');


?>
