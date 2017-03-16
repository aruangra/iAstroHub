<?

if (file_exists('status_skychart')) {
   passthru("sudo rm status_skychart");
   passthru("sh stopskychart.sh");
}

// header('Content-type: text/html');

?>

