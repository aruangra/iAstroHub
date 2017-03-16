<?

passthru("sudo cp /home/pi/ftdi_sio.ko /lib/modules/4.4.13-v7+/kernel/drivers/usb/serial/.");
passthru("sudo depmod -a");

// header('Content-type: text/html');

?>

