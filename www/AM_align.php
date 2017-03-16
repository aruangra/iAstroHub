<?

if (!file_exists('status_ccd') || !file_exists('focuserlink')) {

passthru("sudo /home/pi/www/./align");

// header('Content-type: text/html');

}

?>
