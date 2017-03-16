<?

header('Content-type: text/html');

passthru("echo GETTEMP: > /tmp/gtkImager");

sleep(1);

passthru("tail -n 20 CCD_log.txt | tac");

?> 
