<?

if (file_exists('status_app')) {

if (!file_exists('status_connect')) {

$file=fopen("status_connect","w");
fwrite($file, "connect");
fclose($file);

passthru("echo 'flcFolderCCD /home/pi/www/tmp_images/' > /home/pi/www/Commands.txt");
sleep(1);
passthru("echo 'chkFileAutoSaveCCD 1' > /home/pi/www/Commands.txt");
sleep(1);
passthru("echo 'chkFileLongNames 1' > /home/pi/www/Commands.txt");
sleep(1);
passthru("echo 'write_CCD_status_to_file 1' > /home/pi/www/Commands.txt");
sleep(1);
passthru("echo 'open_focuser_comms 0' > /home/pi/www/Commands.txt");
sleep(2);
passthru("echo 'open_ccd_comms 1' > /home/pi/www/Commands.txt");
//sleep(25);
//passthru("echo 'SetConfFloat Misc/BlockCCDCommands 5.0' > /home/pi/www/Commands.txt");
//sleep(1);
//passthru("echo 'configure_ccd_camera' > /home/pi/www/Commands.txt");

}

}

// header('Content-type: text/html');

?>
