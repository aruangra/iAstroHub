<?

if (!file_exists('status_mlu')) {

	passthru("sudo sh kill_dslr.sh");

	if (file_exists('status_dslr')) {
	   passthru("sudo rm status_dslr");
	}

	if (file_exists('status_loading')) {
	   passthru("sudo rm status_loading");
	}
	
}

?>
