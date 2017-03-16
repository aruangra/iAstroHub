<?

$file=fopen("IP_TSX.txt","w");
fwrite($file, $_GET['IP']);
fclose($file);

// header('Content-type: text/html');

?>
