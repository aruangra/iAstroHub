<?

$file=fopen("pushover_ap.txt","w");
fwrite($file, $_GET['ap']);
fclose($file);

$file=fopen("pushover_user.txt","w");
fwrite($file, $_GET['user']);
fclose($file);

// header('Content-type: text/html');

?>
