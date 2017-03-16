<?

passthru('cp /home/pi/www/ap.conf.tmp /home/pi/www/ap.conf');
passthru('echo wpa_passphrase=' . $_GET['net_pwd'] . ' >> /home/pi/www/ap.conf');
passthru('echo ssid=' . $_GET['net_name'] . ' >> /home/pi/www/ap.conf');
passthru('sudo cp /home/pi/www/ap.conf /etc/hostapd/ap.conf');
passthru("sudo sh halt.sh");

?>
