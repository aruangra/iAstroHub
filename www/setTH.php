<?

$file=fopen("/home/pi/www/TH.txt","w");
fwrite($file, $_GET['TH']);
fclose($file);

header('Content-type: text/html');

echo $_GET['TH'];

?>
