<?

header('Content-type: text/html');

if (file_exists('status_dslr')) {
	passthru("tail -1 photo.txt");
} else {
	echo "*****";
}

?> 
