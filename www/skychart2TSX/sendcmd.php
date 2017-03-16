<?

// header('Content-type: text/html');


switch ($_GET['cmd']) {

    case 0:
        passthru("sudo perl redraw.pl");
        break;
    case 1:
        passthru("sudo perl zoom_in.pl");
        break;
    case 2:
        passthru("sudo perl zoom_out.pl");
        break;
    case 3:
        passthru("sudo perl setfov1d.pl");
        break;
    case 4:
        passthru("sudo perl setfov5d.pl");
        break;
    case 5:
        passthru("sudo perl setfov20d.pl");
        break;
    case 6:
        passthru("sudo perl move_E.pl");
        break;
    case 7:
        passthru("sudo perl move_W.pl");
        break;
    case 8:
        passthru("sudo perl move_N.pl");
        break;
    case 9:
        passthru("sudo perl move_S.pl");
        break;
    case 10:
        passthru("sudo perl bg_on.pl");
        break;
    case 11:
        passthru("sudo perl bg_off.pl");
        break;
    case 12:
        passthru("sudo perl solvedframe.pl");
        break;
    case 13:
        passthru("sudo perl frame_on.pl");
        break;
    case 14:
        passthru("sudo perl frame_off.pl");
        break;
    case 15:
        passthru("sudo perl i_on.pl");
        break;
    case 16:
        passthru("sudo perl i_off.pl");
        break;

}

passthru("sudo perl getra.pl");
passthru("sudo perl getdec.pl");

passthru("sudo perl save_img.pl");

?>
