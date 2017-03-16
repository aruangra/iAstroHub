<?

if (!file_exists('../status_focus')) {

$file=fopen("../focus_move_to.txt","w");
fwrite($file, $_GET['to_pos']);
fclose($file);

passthru("sudo /home/pi/www/./move_to &");

header('Content-type: text/html');

echo "MMMMMMMMMM";

}

?>
