<?

passthru("echo 'Robomask: Open' >> mask_log.txt");
passthru("sudo ./YServo robomask.servo1 move -1000 5000 >> mask_log.txt");

// header('Content-type: text/html');

?>

