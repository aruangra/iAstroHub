<?

header('Content-type: text/html');

passthru("sudo cat /root/GoQat/CCD_status.txt > /home/pi/www/QSI_status.txt");
passthru("echo ' ' >> /home/pi/www/QSI_status.txt");
passthru("echo '-------------------------------------------------------' >> /home/pi/www/QSI_status.txt");
passthru("sudo tail -n 20 /root/GoQat/log.txt | tac >> /home/pi/www/QSI_status.txt");
passthru("cat /home/pi/www/QSI_status.txt");

?> 
