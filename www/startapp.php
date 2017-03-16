<?

if (!file_exists('status_app')) {

$file=fopen("status_app","w");
fwrite($file, "app");
fclose($file);

if (file_exists('/tmp/gtkImager')) {
   passthru("sudo rm /tmp/gtkImager");
}

if (file_exists('/home/pi/www/Commands.txt')) {
   passthru("echo ' ' > /home/pi/www/Commands.txt");
}

passthru("sudo sh startapp.sh");

}

// header('Content-type: text/html');

?>

