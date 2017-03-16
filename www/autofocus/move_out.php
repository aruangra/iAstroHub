<?

if (!file_exists('../status_focus')) {

$file=fopen("../focus_move.txt","w");
fwrite($file, $_GET['move']);
fclose($file);

passthru("sudo /home/pi/www/./move_out &");

header('Content-type: text/html');

echo "MMMMMMMMMM";

}

?>
