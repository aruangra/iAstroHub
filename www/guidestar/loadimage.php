<?

// header('Content-type: text/html');

passthru("sudo cp /home/pi/www/images/" . $_GET['file'] . " /home/pi/www/image.jpg");

?>
