<?

header('Content-type: text/html');

passthru("tail -n 20 CCD_log.txt | tac");

?> 
