<?

header('Content-type: text/html');

if (file_exists('../status_ccd')) {
	passthru("sudo tail -1 image.txt");
} else {
	echo "*****";
}

?> 
