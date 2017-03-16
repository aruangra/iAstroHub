<?

// header('Content-type: text/html');

if (!file_exists('/home/pi/www/status_processing')) {

passthru("sh /home/pi/www/guidestar/INDI_process_image.sh " . $_GET['file']);

}

?>
