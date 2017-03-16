<?

header('Content-type: text/html');

passthru("tail -n 20 mask_log.txt | tac");

?> 
