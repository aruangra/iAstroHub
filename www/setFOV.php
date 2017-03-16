<?

$file=fopen("/home/pi/www/AM_FOV.txt","w");
fwrite($file, $_GET['FOV']);
fclose($file);

//header('Content-type: text/html');

?>
