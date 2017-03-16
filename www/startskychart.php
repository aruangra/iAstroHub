<?

if (!file_exists('status_skychart')) {

$file=fopen("status_skychart","w");
fwrite($file, "skychart");
fclose($file);

passthru("sudo sh startskychart.sh");

}

// header('Content-type: text/html');

?>

