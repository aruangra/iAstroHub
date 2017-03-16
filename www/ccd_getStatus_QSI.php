<?

header('Content-type: text/html');

passthru("sudo tail -n 20 /root/GoQat/log.txt | tac");

?> 
