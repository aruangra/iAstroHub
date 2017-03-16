<?

if (file_exists('../status_capture')) {
   passthru("echo STOP: > /tmp/gtkImager");
   passthru("echo STOP: > /tmp/gtkImager");
   passthru("sudo rm ../status_capture");
}

if (file_exists('../status_ccd')) {
   passthru("sudo rm ../status_ccd");
}

passthru("sudo sh kill_ccd.sh");

header('Content-type: text/html');

echo "Stopped.";

?>
