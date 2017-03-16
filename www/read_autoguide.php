<?

header('Content-type: text/html');

if (file_exists('status_guiding')) {
	passthru("tail -1 guiding.txt");
} else {
	echo "GGGGG";
}

?> 
