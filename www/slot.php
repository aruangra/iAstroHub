<?

switch ($_GET['filter']) {

    case 1:
        passthru("sudo /home/pi/www/./F1");
        break;
    case 2:
        passthru("sudo /home/pi/www/./F2");
        break;
    case 3:
        passthru("sudo /home/pi/www/./F3");
        break;
    case 4:
        passthru("sudo /home/pi/www/./F4");
        break;
    case 5:
        passthru("sudo /home/pi/www/./F5");
        break;
    case 6:
        passthru("sudo /home/pi/www/./F6");
        break;
    case 7:
        passthru("sudo /home/pi/www/./F7");
        break;
    case 8:
        passthru("sudo /home/pi/www/./F8");
        break;

}

header('Content-type: text/html');

echo "Filter Change Done";

?>
