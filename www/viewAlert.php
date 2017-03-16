<?

header('Content-type: text/html');

passthru("tail -n 20 alert.txt | tac");

?>
