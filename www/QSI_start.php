<?

if (file_exists('status_connect')) {

if (!file_exists('status_ccd')) {

$file=fopen("status_ccd","w");
fwrite($file, "status_ccd");
fclose($file);

$file=fopen("prepare_ccd","w");
fwrite($file, "prepare_ccd");
fclose($file);

$file=fopen("image.txt","w");
fwrite($file, "Preparing Camera..." . "\n");
fclose($file);

$files = glob('/home/pi/www/tmp_images/*.fit'); // get all file names
foreach($files as $file){ // iterate files
  if(is_file($file))
    unlink($file); // delete file
}

if (file_exists('focuserlink')) {
	passthru("echo 'open_focuser_comms 1' > /home/pi/www/Commands.txt");
	sleep(4);
	passthru("echo '' > /home/pi/www/FC_CUR_POS.txt");
	passthru("echo '' > /home/pi/www/FC_TEMP.txt");
}

passthru("echo 'txtCCDChipTemp " . $_GET['tec'] . "' > /home/pi/www/Commands.txt");
usleep(500000);
if ($_GET['ld']=="0") {
	passthru("echo 'cmbExpType TARGET' > /home/pi/www/Commands.txt");
} else {
	passthru("echo 'cmbExpType DARK' > /home/pi/www/Commands.txt");
}
usleep(500000);
passthru("echo 'txtCCDHBin " . $_GET['bin'] . "' > /home/pi/www/Commands.txt");
usleep(500000);
passthru("echo 'txtCCDVBin " . $_GET['bin'] . "' > /home/pi/www/Commands.txt");
usleep(500000);
passthru("echo 'txtFileCCD " . $_GET['filename'] . "' > /home/pi/www/Commands.txt");
usleep(500000);
passthru("echo 'txtCCDExposure " . $_GET['ex'] . "' > /home/pi/www/Commands.txt");
usleep(500000);

$file=fopen("ccd_nf.txt","w");
fwrite($file, $_GET['nf']);
fclose($file);

$file=fopen("ccd_filter.txt","w");
fwrite($file, $_GET['filter']);
fclose($file);

$file=fopen("ccd_ex.txt","w");
fwrite($file, $_GET['ex']);
fclose($file);

passthru("sudo rm prepare_ccd");

passthru("sudo /home/pi/www/./QSIcontrol &");

}

}

// header('Content-type: text/html');

?>
