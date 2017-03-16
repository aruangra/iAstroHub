<?

passthru("sudo rmmod ftdi_sio");
passthru("sudo rm /lib/modules/4.4.13-v7+/kernel/drivers/usb/serial/ftdi_sio.ko");
passthru("sudo depmod -a");

// header('Content-type: text/html');

?>

