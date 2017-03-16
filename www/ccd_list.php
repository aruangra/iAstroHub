<?

header('Content-type: text/html');

passthru("echo CAMLIST: > /tmp/gtkImager");

sleep(1);

passthru("tail -n 20 CCD_log.txt | tac");

?>