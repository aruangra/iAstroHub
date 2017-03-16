<?

if (!file_exists('status_dslr')) {

$file=fopen("dslr_nf.txt","w");
fwrite($file, $_GET['nf']);
fclose($file);

$file=fopen("dslr_ex.txt","w");
fwrite($file, $_GET['ex']);
fclose($file);

$file=fopen("dslr_de.txt","w");
fwrite($file, $_GET['delay']);
fclose($file);

$file=fopen("dslr_iso.txt","w");
fwrite($file, $_GET['iso']);
fclose($file);

//passthru("sudo ./dslr &");
passthru("echo dslr > startdslr");

}

// header('Content-type: text/html');

?>
