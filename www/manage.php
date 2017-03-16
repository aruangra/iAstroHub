<html>
 <head>

    <meta http-equiv="Content-Type" content="text/html">
    <title>Admin</title>
    <script language="javascript" type="text/javascript" src="jquery.js"></script>

 </head>
 <body bgcolor="#000000" TEXT="#FF0000" LINK="#FF0000" VLINK="#FF0000">


<script id="source" language="javascript" type="text/javascript">

var xmlhttp;
var st;

	function GetXmlHttpObject() {
		if(window.XMLHttpRequest) {
			// code for IE7+, Firefox, Chrome, Opera, Safari
			return new XMLHttpRequest();
		}
		
		if(window.ActiveXObject) {
			// code for IE6, IE5
			return new ActiveXObject("Microsoft.XMLHTTP");
		}	
		return null;
	}
	
	function startskychart() {
	
		var r=confirm("Do you want to START Skychart?")
		if (r==true)
		{
	
		xmlhttp = GetXmlHttpObject();

		var url = "startskychart.php";
				
		xmlhttp.onreadystatechange = handleResponse;
		xmlhttp.open("GET",url,true);
		xmlhttp.send(null);
		
		document.getElementById('returned_msg').innerHTML = "Skychart started.";
		
		}
		
	}
	
	function startskychartvnc() {
	
		var r=confirm("Do you want to START Skychart for setup via VNC?")
		if (r==true)
		{
	
		xmlhttp = GetXmlHttpObject();

		var url = "startskychartvnc.php";
				
		xmlhttp.onreadystatechange = handleResponse;
		xmlhttp.open("GET",url,true);
		xmlhttp.send(null);
		
		document.getElementById('returned_msg').innerHTML = "Skychart is available for setup via VNC";
		
		}
		
	}

	function stopskychart() {
	
		var r=confirm("Do you want to STOP Skychart?")
		if (r==true)
		{
	
		xmlhttp = GetXmlHttpObject();

		var url = "stopskychart.php";
				
		xmlhttp.onreadystatechange = handleResponse;
		xmlhttp.open("GET",url,true);
		xmlhttp.send(null);
		
		document.getElementById('returned_msg').innerHTML = "Skychart stopped.";
		
		}
		
	}
	
	function resetoffset() {
		xmlhttp = GetXmlHttpObject();

		var url = "resetoffset.php";
				
		xmlhttp.onreadystatechange = handleResponse;
		xmlhttp.open("GET",url,true);
		xmlhttp.send(null);
		
		document.getElementById('returned_msg').innerHTML = "Offsets were reset.";
		
	}
	
	function file_getStatus() {
			
		xmlhttp3 = GetXmlHttpObject();

		var url3 = "file_getStatus.php";
		
		xmlhttp3.onreadystatechange = handleResponse3;
		xmlhttp3.open("GET",url3,true);
		xmlhttp3.send(null);
		
	}
	
	function deleteimages() {
	
		var r=confirm("Do you want to DELETE all images?")
		if (r==true)
		{
	
		xmlhttp = GetXmlHttpObject();

		var url = "deleteimages.php";
				
		xmlhttp.onreadystatechange = handleResponse;
		xmlhttp.open("GET",url,true);
		xmlhttp.send(null);
		
		document.getElementById('returned_msg').innerHTML = "All images were deleted.";
		
		}
		
	}
	
	function downloadimages() {
	
		var r=confirm("Wait until LED on USB flash drive stops blinking! And check File Status.")
		if (r==true)
		{
	
		xmlhttp = GetXmlHttpObject();

		var url = "downloadimages.php";
				
		xmlhttp.onreadystatechange = handleResponse;
		xmlhttp.open("GET",url,true);
		xmlhttp.send(null);
		
		document.getElementById('returned_msg').innerHTML = "Wait until LED on USB flash drive stops blinking! And check File Status.";
		
		}
		
	}
	
	function uploadpatch() {
	
		var r=confirm("Do you want to upload the patch? Wait until LED on USB flash drive stops blinking! And check File Status.")
		if (r==true)
		{
	
		xmlhttp = GetXmlHttpObject();

		var url = "uploadpatch.php";
				
		xmlhttp.onreadystatechange = handleResponse;
		xmlhttp.open("GET",url,true);
		xmlhttp.send(null);
		
		document.getElementById('returned_msg').innerHTML = "Wait until LED on USB flash drive stops blinking! And check File Status.";
		
		}
		
	}
	
	function clearguidinglog() {
	
		var r=confirm("Do you want to DELETE guiding and alert logs?")
		if (r==true)
		{
	
		xmlhttp = GetXmlHttpObject();

		var url = "clearguidinglog.php";
				
		xmlhttp.onreadystatechange = handleResponse;
		xmlhttp.open("GET",url,true);
		xmlhttp.send(null);
		
		document.getElementById('returned_msg').innerHTML = "Guiding and alert logs were deleted.";
		
		}
		
	}	
	
	function clearfocuslog() {
	
		var r=confirm("Do you want to DELETE focus log?")
		if (r==true)
		{
	
		xmlhttp = GetXmlHttpObject();

		var url = "clearfocuslog.php";
				
		xmlhttp.onreadystatechange = handleResponse;
		xmlhttp.open("GET",url,true);
		xmlhttp.send(null);
		
		document.getElementById('returned_msg').innerHTML = "Focus log was deleted.";
		
		}
		
	}	
	
	function setdslr1() {
		xmlhttp = GetXmlHttpObject();

		var url = "setdslr1.php";
				
		xmlhttp.onreadystatechange = handleResponse;
		xmlhttp.open("GET",url,true);
		xmlhttp.send(null);
		
		document.getElementById('returned_msg').innerHTML = "Camera protocol for Canon 40D (or similar) was selected.";
		
	}
	
	function setdslr2() {
		xmlhttp = GetXmlHttpObject();

		var url = "setdslr2.php";
				
		xmlhttp.onreadystatechange = handleResponse;
		xmlhttp.open("GET",url,true);
		xmlhttp.send(null);
		
		document.getElementById('returned_msg').innerHTML = "Camera protocol for Canon 550D/1100D (or similar) was selected.";
		
	}
	
	function setdslr3() {
		xmlhttp = GetXmlHttpObject();

		var url = "setdslr3.php";
				
		xmlhttp.onreadystatechange = handleResponse;
		xmlhttp.open("GET",url,true);
		xmlhttp.send(null);
		
		document.getElementById('returned_msg').innerHTML = "Camera protocol for Canon 1000D (or similar) was selected.";
		
	}
		
	function setdslr5() {
		xmlhttp = GetXmlHttpObject();

		var url = "setdslr5.php";
				
		xmlhttp.onreadystatechange = handleResponse;
		xmlhttp.open("GET",url,true);
		xmlhttp.send(null);
		
		document.getElementById('returned_msg').innerHTML = "Pentax camera protocol was selected.";
		
	}
	
	function setdslr6() {
		xmlhttp = GetXmlHttpObject();

		var url = "setdslr6.php";
				
		xmlhttp.onreadystatechange = handleResponse;
		xmlhttp.open("GET",url,true);
		xmlhttp.send(null);
		
		document.getElementById('returned_msg').innerHTML = "FTDI-based shutter cable (Port=ttyUSB0) was selected to control the camera shutter.";
		
	}
	
	function setdslr7() {
		xmlhttp = GetXmlHttpObject();

		var url = "setdslr7.php";
				
		xmlhttp.onreadystatechange = handleResponse;
		xmlhttp.open("GET",url,true);
		xmlhttp.send(null);
		
		document.getElementById('returned_msg').innerHTML = "FTDI-based shutter cable (Port=ttyUSB1) was selected to control the camera shutter.";
		
	}		
	
	
	function ftdi_load() {
		xmlhttp = GetXmlHttpObject();

		var url = "ftdi_load.php";
				
		xmlhttp.onreadystatechange = handleResponse;
		xmlhttp.open("GET",url,true);
		xmlhttp.send(null);
		
		document.getElementById('returned_msg').innerHTML = "FTDI driver was loaded.";
		
	}
	
	
	function ftdi_unload() {
		xmlhttp = GetXmlHttpObject();

		var url = "ftdi_unload.php";
				
		xmlhttp.onreadystatechange = handleResponse;
		xmlhttp.open("GET",url,true);
		xmlhttp.send(null);
		
		document.getElementById('returned_msg').innerHTML = "FTDI driver was unloaded.";
		
	}	
	
	
    function startPK() {
		
		var r=confirm("Do you want to START/RESTART Pagekite?")
		if (r==true)
		{
		
                xmlhttp = GetXmlHttpObject();

                var url = "startPK.php";

                xmlhttp.onreadystatechange = handleResponse;
                xmlhttp.open("GET",url,true);
                xmlhttp.send(null);

                document.getElementById('returned_msg').innerHTML = "Pagekite was started.";

        }
		
	}
	
	
	function startddserver() {
		xmlhttp = GetXmlHttpObject();

		var url = "startddserver.php";
				
		xmlhttp.onreadystatechange = handleResponse;
		xmlhttp.open("GET",url,true);
		xmlhttp.send(null);
		
		document.getElementById('returned_msg').innerHTML = "DslrDashboard Server was started.";
		
	}
	
	function stopddserver() {
		xmlhttp = GetXmlHttpObject();

		var url = "stopddserver.php";
				
		xmlhttp.onreadystatechange = handleResponse;
		xmlhttp.open("GET",url,true);
		xmlhttp.send(null);
		
		document.getElementById('returned_msg').innerHTML = "DslrDashboard Server was stopped.";
		
	}	
	
	function focus_swap() {
	
		var r=confirm("Do you want to swap focuser directions?")
		if (r==true)
		{
	
		xmlhttp = GetXmlHttpObject();

		var url = "focus_swap.php";
				
		xmlhttp.onreadystatechange = handleResponse;
		xmlhttp.open("GET",url,true);
		xmlhttp.send(null);
		
		document.getElementById('returned_msg').innerHTML = "Focuser directions were swapped.";
		
		}
		
	}
	
	function startapp() {
	
		var r=confirm("Do you want to START apps?")
		if (r==true)
		{
	
		xmlhttp = GetXmlHttpObject();

		var url = "startapp.php";
				
		xmlhttp.onreadystatechange = handleResponse;
		xmlhttp.open("GET",url,true);
		xmlhttp.send(null);
		
		document.getElementById('returned_msg').innerHTML = "Apps started.";
		
		}
		
	}
	
	function stopapp() {
	
		var r=confirm("Do you want to STOP apps?")
		if (r==true)
		{

		xmlhttp = GetXmlHttpObject();

		var url = "stopapp.php";
				
		xmlhttp.onreadystatechange = handleResponse;
		xmlhttp.open("GET",url,true);
		xmlhttp.send(null);
		
		document.getElementById('returned_msg').innerHTML = "Apps stopped.";
		
		}
		
	}	
	

	
	function vnc() {
		xmlhttp = GetXmlHttpObject();

		var url = "vnc.php";
				
		xmlhttp.onreadystatechange = handleResponse;
		xmlhttp.open("GET",url,true);
		xmlhttp.send(null);
		
		document.getElementById('returned_msg').innerHTML = "VNC server was started.";
		
	}
	
	function mount() {
	
		var r=confirm("Do you want to MOUNT a USB flash drive?")
		if (r==true)
		{
	
		xmlhttp = GetXmlHttpObject();

		var url = "mount.php";
				
		xmlhttp.onreadystatechange = handleResponse;
		xmlhttp.open("GET",url,true);
		xmlhttp.send(null);
		
		document.getElementById('returned_msg').innerHTML = "USB flash drive was mounted.";
		
		}
		
	}
	
	function unmount() {
	
		var r=confirm("Do you want to UNMOUNT a USB flash drive?")
		if (r==true)
		{

		xmlhttp = GetXmlHttpObject();

		var url = "unmount.php";
				
		xmlhttp.onreadystatechange = handleResponse;
		xmlhttp.open("GET",url,true);
		xmlhttp.send(null);
		
		document.getElementById('returned_msg').innerHTML = "USB flash drive was unmounted.";
		
		}
		
	}	
	
	function shutdown() {
	
		var r=confirm("Do you want to SHUTDOWN iAstroHub?")
		if (r==true)
		{
	
		xmlhttp = GetXmlHttpObject();

		var url = "halt.php";
				
		xmlhttp.onreadystatechange = handleResponse;
		xmlhttp.open("GET",url,true);
		xmlhttp.send(null);
		
		document.getElementById('returned_msg').innerHTML = "Shutting down... Unplug iAstroHub after LED is off.";
		
		}
		
	}
	
	function reboot() {
	
		var r=confirm("Do you want to REBOOT iAstroHub?")
		if (r==true)
		{
	
		xmlhttp = GetXmlHttpObject();

		var url = "reboot.php";
				
		xmlhttp.onreadystatechange = handleResponse;
		xmlhttp.open("GET",url,true);
		xmlhttp.send(null);
		
		document.getElementById('returned_msg').innerHTML = "Rebooting...";
		
		}
		
	}


	function tts() {
		xmlhttp23 = GetXmlHttpObject();

		var url23 = "tts.php";
				
		xmlhttp23.onreadystatechange = handleResponse23;
		xmlhttp23.open("GET",url23,true);
		xmlhttp23.send(null);
	}
	
	
	function dithering() {
		xmlhttp = GetXmlHttpObject();

		var url = "dithering.php";
				
		xmlhttp.onreadystatechange = handleResponse;
		xmlhttp.open("GET",url,true);
		xmlhttp.send(null);
	}

	
	function hattrick() {
		xmlhttp2 = GetXmlHttpObject();

		var url2 = "hattrick.php";
				
		xmlhttp2.onreadystatechange = handleResponse2;
		xmlhttp2.open("GET",url2,true);
		xmlhttp2.send(null);
	}
	
	
	function focuserlink() {
		xmlhttp22 = GetXmlHttpObject();

		var url22 = "focuserlink.php";
				
		xmlhttp22.onreadystatechange = handleResponse22;
		xmlhttp22.open("GET",url22,true);
		xmlhttp22.send(null);
	}	
	
	
	function download() {
		xmlhttp4 = GetXmlHttpObject();

		var url4 = "download.php";
				
		xmlhttp4.onreadystatechange = handleResponse4;
		xmlhttp4.open("GET",url4,true);
		xmlhttp4.send(null);
	}
	
	
	function mlu() {
		xmlhttp5 = GetXmlHttpObject();

		var url5 = "mlu.php";
				
		xmlhttp5.onreadystatechange = handleResponse5;
		xmlhttp5.open("GET",url5,true);
		xmlhttp5.send(null);
	}	
	
	
	function ccd_getStatus() {
			
		xmlhttp3 = GetXmlHttpObject();

		var url3 = "ccd_getStatus.php";
		
		xmlhttp3.onreadystatechange = handleResponse3;
		xmlhttp3.open("GET",url3,true);
		xmlhttp3.send(null);
		
	}
	
	function ccd_list() {
			
		xmlhttp3 = GetXmlHttpObject();

		var url3 = "ccd_list.php";
		
		xmlhttp3.onreadystatechange = handleResponse3;
		xmlhttp3.open("GET",url3,true);
		xmlhttp3.send(null);
		
	}
	
	function camset(camid) {
	
		xmlhttp = GetXmlHttpObject();

		var url = "camset.php?camid=" + camid;
		
		xmlhttp.onreadystatechange = handleResponse;
		xmlhttp.open("GET",url,true);
		xmlhttp.send(null);
			
		document.getElementById('returned_msg').innerHTML = "Camera connected";

	}	
	
	
	function ccd_getStatus_QSI() {
			
		xmlhttp3 = GetXmlHttpObject();

		var url3 = "ccd_getStatus_QSI.php";
		
		xmlhttp3.onreadystatechange = handleResponse3;
		xmlhttp3.open("GET",url3,true);
		xmlhttp3.send(null);
		
	}	

	
	function camset_QSI() {
	
	//	var r=confirm("Wait for 30s until the camera status shows... COMMAND configure_ccd_camera")
		var r=confirm("Wait until the camera status shows >>>> Opened CCD camera")
		if (r==true)
		{
	
			xmlhttp = GetXmlHttpObject();

			var url = "camset_QSI.php";
			
			xmlhttp.onreadystatechange = handleResponse;
			xmlhttp.open("GET",url,true);
			xmlhttp.send(null);
				
			document.getElementById('returned_msg').innerHTML = "Press 'Status' to view the camera status.";
		
		}

	}
	

	function default_dslr() {
		xmlhttp = GetXmlHttpObject();

		var url = "default_dslr.php";
				
		xmlhttp.onreadystatechange = handleResponse;
		xmlhttp.open("GET",url,true);
		xmlhttp.send(null);
		
		document.getElementById('returned_msg').innerHTML = "The default page is set for Canon/Nikon DSLR control.";
	}	
	
	function default_pentax() {
		xmlhttp = GetXmlHttpObject();

		var url = "default_pentax.php";
				
		xmlhttp.onreadystatechange = handleResponse;
		xmlhttp.open("GET",url,true);
		xmlhttp.send(null);
		
		document.getElementById('returned_msg').innerHTML = "The default page is set for Pentax DSLR control.";
	}

	function default_INDI() {
		xmlhttp = GetXmlHttpObject();

		var url = "default_INDI.php";
				
		xmlhttp.onreadystatechange = handleResponse;
		xmlhttp.open("GET",url,true);
		xmlhttp.send(null);
		
		document.getElementById('returned_msg').innerHTML = "The default page is set for INDI-compatible instrument.";
	}
	
	function default_ccd() {
		xmlhttp = GetXmlHttpObject();

		var url = "default_ccd.php";
				
		xmlhttp.onreadystatechange = handleResponse;
		xmlhttp.open("GET",url,true);
		xmlhttp.send(null);
		
		document.getElementById('returned_msg').innerHTML = "The default page is set for SBIG, ATIK, or QHY camera.";
	}

	function default_qsi() {
		xmlhttp = GetXmlHttpObject();

		var url = "default_qsi.php";
				
		xmlhttp.onreadystatechange = handleResponse;
		xmlhttp.open("GET",url,true);
		xmlhttp.send(null);
		
		document.getElementById('returned_msg').innerHTML = "The default page is set for QSI or SX camera.";
	}
	
	
	function setFOV(FOV) {
		xmlhttp = GetXmlHttpObject();

		var url = "setFOV.php?FOV=" + FOV;
		
		xmlhttp.onreadystatechange = handleResponse;
		xmlhttp.open("GET",url,true);
		xmlhttp.send(null);
			
		document.getElementById('returned_msg').innerHTML = "Minimum FOV was set.";
	}
	
	
	function getCCDGain() {
		xmlhttp6 = GetXmlHttpObject();

		var url6 = "getCCDGain.php";
				
		xmlhttp6.onreadystatechange = handleResponse6;
		xmlhttp6.open("GET",url6,true);
		xmlhttp6.send(null);
	}


	function setCCDGain(gain) {
	
		if (is_int(gain) && gain>=0 && gain<=100) {

			xmlhttp = GetXmlHttpObject();

			var url = "setCCDGain.php?gain=" + gain;
				
			xmlhttp.onreadystatechange = handleResponse;
			xmlhttp.open("GET",url,true);
			xmlhttp.send(null);
			
			document.getElementById('returned_msg').innerHTML = "CCD gain was set.";
		
		} else {
	
			alert("Gain must be an interger [0-100].");

		}

	}
	
	
	function getCCDOffset() {
		xmlhttp7 = GetXmlHttpObject();

		var url7 = "getCCDOffset.php";
				
		xmlhttp7.onreadystatechange = handleResponse7;
		xmlhttp7.open("GET",url7,true);
		xmlhttp7.send(null);
	}	


	function setCCDOffset(offset) {
	
		if (is_int(offset) && offset>=0 && offset<=255) {

			xmlhttp = GetXmlHttpObject();

			var url = "setCCDOffset.php?offset=" + offset;
				
			xmlhttp.onreadystatechange = handleResponse;
			xmlhttp.open("GET",url,true);
			xmlhttp.send(null);
			
			document.getElementById('returned_msg').innerHTML = "CCD offset was set.";
		
		} else {
	
			alert("Offset must be an interger [0-255].");

		}

	}
	
	
	function getDelay() {
		xmlhttp8 = GetXmlHttpObject();

		var url8 = "getDelay.php";
				
		xmlhttp8.onreadystatechange = handleResponse8;
		xmlhttp8.open("GET",url8,true);
		xmlhttp8.send(null);
	}	


	function setDelay(delay) {
	
		if (is_int(delay) && delay>=5 && delay<=60) {

			xmlhttp = GetXmlHttpObject();

			var url = "setDelay.php?delay=" + delay;
				
			xmlhttp.onreadystatechange = handleResponse;
			xmlhttp.open("GET",url,true);
			xmlhttp.send(null);
			
			document.getElementById('returned_msg').innerHTML = "Delay was set.";
		
		} else {
	
			alert("Delay must be an interger [5-60].");

		}

	}	
	

	
	function setIP_TSX(IP) {

			xmlhttp = GetXmlHttpObject();

			var url = "setIP_TSX.php?IP=" + IP;
				
			xmlhttp.onreadystatechange = handleResponse;
			xmlhttp.open("GET",url,true);
			xmlhttp.send(null);
			
			document.getElementById('returned_msg').innerHTML = "IP address was set.";

	}
	
	
	
	function setNetwork(net_name,net_pwd) {
	
		if (net_pwd.length>=8) {

			xmlhttp = GetXmlHttpObject();

			var url = "setNetwork.php?net_name=" + net_name + "&net_pwd=" + net_pwd;
				
			xmlhttp.onreadystatechange = handleResponse;
			xmlhttp.open("GET",url,true);
			xmlhttp.send(null);
			
			document.getElementById('returned_msg').innerHTML = "Network name and password were changed. Shutting down...";
		
		} else {
	
			alert("Use 8 non-special characters (or more) for the password.");

		}

	}

	function setPushover(ap,user) {

			xmlhttp = GetXmlHttpObject();

			var url = "setPushover.php?ap=" + ap + "&user=" + user;
				
			xmlhttp.onreadystatechange = handleResponse;
			xmlhttp.open("GET",url,true);
			xmlhttp.send(null);
			
			document.getElementById('returned_msg').innerHTML = "Pushover keys were set.";

	}
	
	function notify_imaging() {
		xmlhttp20 = GetXmlHttpObject();

		var url20 = "notify_imaging.php";
				
		xmlhttp20.onreadystatechange = handleResponse20;
		xmlhttp20.open("GET",url20,true);
		xmlhttp20.send(null);
	}	
	
	function notify_guiding() {
		xmlhttp21 = GetXmlHttpObject();

		var url21 = "notify_guiding.php";
				
		xmlhttp21.onreadystatechange = handleResponse21;
		xmlhttp21.open("GET",url21,true);
		xmlhttp21.send(null);
	}	
		
	function set_temma() {
	
			xmlhttp = GetXmlHttpObject();

			var url = "set_temma.php";
					
			xmlhttp.onreadystatechange = handleResponse;
			xmlhttp.open("GET",url,true);
			xmlhttp.send(null);
			
			document.getElementById('returned_msg').innerHTML = "Temma protocol was selected.";
		
	}	
	
	function set_iOptron() {
	
			xmlhttp = GetXmlHttpObject();

			var url = "set_iOptron.php";
					
			xmlhttp.onreadystatechange = handleResponse;
			xmlhttp.open("GET",url,true);
			xmlhttp.send(null);
			
			document.getElementById('returned_msg').innerHTML = "iOptron protocol was selected.";
		
	}	


	function set_nexstar() {
	
			xmlhttp = GetXmlHttpObject();

			var url = "set_nexstar.php";
					
			xmlhttp.onreadystatechange = handleResponse;
			xmlhttp.open("GET",url,true);
			xmlhttp.send(null);
			
			document.getElementById('returned_msg').innerHTML = "Nexstar/Skywatcher protocol was selected.";
		
	}	
	
	function set_lx200() {
	
			xmlhttp = GetXmlHttpObject();

			var url = "set_lx200.php";
					
			xmlhttp.onreadystatechange = handleResponse;
			xmlhttp.open("GET",url,true);
			xmlhttp.send(null);
			
			document.getElementById('returned_msg').innerHTML = "LX200 protocol was selected.";
		
	}
	
	function set_TSX() {
	
			xmlhttp = GetXmlHttpObject();

			var url = "set_TSX.php";
					
			xmlhttp.onreadystatechange = handleResponse;
			xmlhttp.open("GET",url,true);
			xmlhttp.send(null);
			
			document.getElementById('returned_msg').innerHTML = "TheSkyX protocol was selected.";
		
	}
	
	
	
		
	function is_int(value){
	  if((parseFloat(value) == parseInt(value)) && !isNaN(value)){
		  return true;
	  } else {
		  return false;
	  }
	}

	
	function handleResponse() {
		if(xmlhttp.readyState == 4) {
			st = xmlhttp.responseText;
			st=st.substr(0,4);
			if (st == "AAAA") {
				document.getElementById('dithering').value = "Disable";
				document.getElementById('returned_msg').innerHTML = "Dithering was enabled.";
			}
			if (st == "DDDD") {
				document.getElementById('dithering').value = "Enable";
				document.getElementById('returned_msg').innerHTML = "Dithering was disabled.";
			}
		}
	}
	
	
	
	function handleResponse2() {
		if(xmlhttp2.readyState == 4) {
			st2 = xmlhttp2.responseText;
			st2=st2.substr(0,4);
			if (st2 == "AAAA") {
				document.getElementById('hattrick').value = "Disable";
				document.getElementById('returned_msg').innerHTML = "Auto Hat-trick was enabled.";
			}
			if (st2 == "DDDD") {
				document.getElementById('hattrick').value = "Enable";
				document.getElementById('returned_msg').innerHTML = "Auto Hat-trick was disabled.";
			}
		}
	}
	
	
	function handleResponse20() {
		if(xmlhttp20.readyState == 4) {
			st20 = xmlhttp20.responseText;
			st20=st20.substr(0,4);
			if (st20 == "AAAA") {
				document.getElementById('notify_imaging').value = "Disable";
			}
			if (st20 == "DDDD") {
				document.getElementById('notify_imaging').value = "Enable";
			}
		}
	}
	
	
	function handleResponse21() {
		if(xmlhttp21.readyState == 4) {
			st21 = xmlhttp21.responseText;
			st21=st21.substr(0,4);
			if (st21 == "AAAA") {
				document.getElementById('notify_guiding').value = "Disable";
			}
			if (st21 == "DDDD") {
				document.getElementById('notify_guiding').value = "Enable";
			}
		}
	}
	
	
	function handleResponse22() {
		if(xmlhttp22.readyState == 4) {
			st22 = xmlhttp22.responseText;
			st22=st22.substr(0,4);
			if (st22 == "AAAA") {
				document.getElementById('focuserlink').value = "Disable";
			}
			if (st22 == "DDDD") {
				document.getElementById('focuserlink').value = "Enable";
			}
		}
	}

	
	function handleResponse23() {
		if(xmlhttp23.readyState == 4) {
			st23 = xmlhttp23.responseText;
			st23=st23.substr(0,4);
			if (st23 == "AAAA") {
				document.getElementById('tts').value = "Disable";
			}
			if (st23 == "DDDD") {
				document.getElementById('tts').value = "Enable";
			}
		}
	}


	function handleResponse3() {
		if(xmlhttp3.readyState == 4) {
			var temptext = xmlhttp3.responseText;
				document.getElementById('returned_msg').innerHTML = temptext;
		}
	}
	
	
	function handleResponse4() {
		if(xmlhttp4.readyState == 4) {
			st4 = xmlhttp4.responseText;
			st4=st4.substr(0,4);
			if (st4 == "AAAA") {
				document.getElementById('download').value = "Disable";
				document.getElementById('returned_msg').innerHTML = "Image download was enabled.";
			}
			if (st4 == "DDDD") {
				document.getElementById('download').value = "Enable";
				document.getElementById('returned_msg').innerHTML = "Image download was disabled.";
			}
		}
	}	
	
	
	function handleResponse5() {
		if(xmlhttp5.readyState == 4) {
			st5 = xmlhttp5.responseText;
			st5=st5.substr(0,4);
			if (st5 == "AAAA") {
				document.getElementById('mlu').value = "Disable Mirror Lock-Up";
				document.getElementById('returned_msg').innerHTML = "Mirror lock-up was enabled. Make sure that the function is also enabled in your camera. The FTDI-based shutter control cable is needed.";
			}
			if (st5 == "DDDD") {
				document.getElementById('mlu').value = "Enable Mirror Lock-Up";
				document.getElementById('returned_msg').innerHTML = "Mirror lock-up was disabled. Make sure that the function is also disabled in your camera.";
			}
		}
	}		
	
	
	function handleResponse6() {
		if(xmlhttp6.readyState == 4) {
			var temptext6 = xmlhttp6.responseText;
			document.getElementById('gain').value = temptext6; 
		}
	}
	
	
	function handleResponse7() {
		if(xmlhttp7.readyState == 4) {
			var temptext7 = xmlhttp7.responseText;
			document.getElementById('offset').value = temptext7; 
		}
	}	
	
	
	function handleResponse8() {
		if(xmlhttp8.readyState == 4) {
			var temptext8 = xmlhttp8.responseText;
			document.getElementById('delay').value = temptext8; 
		}
	}	
	
	
</script>

<b><u>Autoguider and CCD Camera</u></b>
<p>
<input type="button" value="Start Apps" onClick="javascript: startapp();" style="background-color:#000000; color:#F6CECE">
<p>
<b>Camera Control using OpenSkyImager (for SBIG ATIK and QHY cameras)</b>
<p>
<input type="button" value="Status" onClick="javascript: ccd_getStatus();" style="background-color:#000000; color:#F6CECE">
&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;
<font color="#F6CECE">Camera</font>
<input type="button" value="List" onClick="javascript: ccd_list();" style="background-color:#000000; color:#F6CECE">
<select name="camid" id="camid" style="background-color:#000000; color:#F6CECE">
  <option value="1">#1</option>
  <option value="2">#2</option>
</select>
<input type="button" value="Connect" onClick="javascript: camset(document.getElementById('camid').value);" style="background-color:#000000; color:#F6CECE">
<p>
Gain [0-100]
<input type="button" value="Get" onClick="javascript: getCCDGain();" style="background-color:#000000; color:#FF0000">
<input type="text" size="5" value="" name="gain" id="gain" style="background-color:#302217; color:#FF0000"/>
<input type="button" value="Set" onClick="javascript: setCCDGain(document.getElementById('gain').value);" style="background-color:#000000; color:#FF0000">
<p>
Offset [0-255]
<input type="button" value="Get" onClick="javascript: getCCDOffset();" style="background-color:#000000; color:#FF0000">
<input type="text" size="5" value="" name="offset" id="offset" style="background-color:#302217; color:#FF0000"/>
<input type="button" value="Set" onClick="javascript: setCCDOffset(document.getElementById('offset').value);" style="background-color:#000000; color:#FF0000">
<p>
<b>Camera Control using GoQat (for QSI and SX cameras)</b>
<p>
<input type="button" value="Status" onClick="javascript: ccd_getStatus_QSI();" style="background-color:#000000; color:#F6CECE">
&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;
<input type="button" value="Connect" onClick="javascript: camset_QSI();" style="background-color:#000000; color:#F6CECE">

<!--
<p>
Focuser Link (for filter offset):
<?php
if (file_exists('/home/pi/www/focuserlink')) {
    $btn_focuserlink = 'Press to Disable';
} else {
	$btn_focuserlink = 'Press to Enable';
} 
?>
<input type="button" id="focuserlink" value="<?= htmlentities($btn_focuserlink) ?>" onClick="javascript: focuserlink();" style="background-color:#000000; color:#FF0000">

<p>
Auto Hat-trick (exposure time >= 30s):
<?php
if (file_exists('/home/pi/www/hattrick')) {
    $btn_hattrick = 'Press to Disable';
} else {
	$btn_hattrick = 'Press to Enable';
} 
?>
<input type="button" id="hattrick" value="<?= htmlentities($btn_hattrick) ?>" onClick="javascript: hattrick();" style="background-color:#000000; color:#FF0000">
-->
<p>
<b>General Setting</b>
<p>
Delay (s) [5-60]
<input type="button" value="Get" onClick="javascript: getDelay();" style="background-color:#000000; color:#FF0000">
<input type="text" size="5" value="" name="delay" id="delay" style="background-color:#302217; color:#FF0000"/>
<input type="button" value="Set" onClick="javascript: setDelay(document.getElementById('delay').value);" style="background-color:#000000; color:#FF0000">

<p>
Report Imaging Progress via Speaker:
<?php
if (file_exists('/home/pi/www/tts')) {
    $btn_tts = 'Press to Disable';
} else {
	$btn_tts = 'Press to Enable';
} 
?>
<input type="button" id="tts" value="<?= htmlentities($btn_tts) ?>" onClick="javascript: tts();" style="background-color:#000000; color:#FF0000">

<p>
Dithering:
<?php
if (file_exists('/home/pi/www/dithering')) {
    $btn_dithering = 'Press to Disable';
} else {
	$btn_dithering = 'Press to Enable';
} 
?>
<input type="button" id="dithering" value="<?= htmlentities($btn_dithering) ?>" onClick="javascript: dithering();" style="background-color:#000000; color:#FF0000">

<p>
<input type="button" value="View Calibration Log" onclick="window.open('calibrating.txt')"; " style="background-color:#000000; color:#FF0000">
<input type="button" value="View Guiding Log" onclick="window.open('guiding.txt')"; " style="background-color:#000000; color:#FF0000">
<input type="button" value="View Alert Log" onclick="window.open('alert.txt')"; " style="background-color:#000000; color:#FF0000">
<p>
<input type="button" value="DELETE Guiding and Alert Logs" onClick="javascript: clearguidinglog();" style="background-color:#000000; color:#FF0000">
<p>
<!--
<input type="button" value="Stop Apps" onClick="javascript: stopapp();" style="background-color:#000000; color:#FF0000">
<p>
-->

<hr color="#4A4344">
<b><u>DSLR</u></b>
<p>
DslrDashboard Server:
<input type="button" value="Start" onClick="javascript: startddserver();" style="background-color:#000000; color:#FF0000">
<input type="button" value="Stop" onClick="javascript: stopddserver();" style="background-color:#000000; color:#FF0000">
<p>
Camera Protocol:
<input type="button" value="Canon 6D/60D/550D/1100D" onClick="javascript: setdslr2();" style="background-color:#000000; color:#FF0000">
<input type="button" value="Canon 40D" onClick="javascript: setdslr1();" style="background-color:#000000; color:#FF0000">
<input type="button" value="Canon 1000D and Nikon" onClick="javascript: setdslr3();" style="background-color:#000000; color:#FF0000">
<input type="button" value="Pentax" onClick="javascript: setdslr5();" style="background-color:#000000; color:#FF0000">
<p>
<input type="button" value="FTDI-based shutter cable (ttyUSB0)" onClick="javascript: setdslr6();" style="background-color:#000000; color:#FF0000">
<input type="button" value="FTDI-based shutter cable (ttyUSB1)" onClick="javascript: setdslr7();" style="background-color:#000000; color:#FF0000">
<p>
FTDI Driver:
<input type="button" value="Load" onClick="javascript: ftdi_load();" style="background-color:#000000; color:#FF0000">
<input type="button" value="Unload" onClick="javascript: ftdi_unload();" style="background-color:#000000; color:#FF0000">
<p>

<!--
<?php
if (file_exists('/home/pi/www/mlu')) {
    $btn_mlu = 'Press to Disable Mirror Lock-Up';
} else {
	$btn_mlu = 'Press to Enable Mirror Lock-Up';
} 
?>
<input type="button" id="mlu" value="<?= htmlentities($btn_mlu) ?>" onClick="javascript: mlu();" style="background-color:#000000; color:#FF0000">
-->

<p>
Image Download:
<?php
if (file_exists('/home/pi/www/download')) {
    $btn_download = 'Press to Disable';
} else {
	$btn_download = 'Press to Enable';
} 
?>
<input type="button" id="download" value="<?= htmlentities($btn_download) ?>" onClick="javascript: download();" style="background-color:#000000; color:#FF0000">
<p>

<hr color="#4A4344">
<b><u>Plate Solving</u></b>
<p>
Minimum FOV (deg)
<select name="FOV" id="FOV" style="background-color:#000000; color:#FF0000">
  <option value="0.1">0.1</option>
  <option value="0.2">0.2</option>
  <option value="0.5">0.5</option>  
  <option value="1.0">1.0</option>  
  <option value="2.0">2.0</option>  
  <option value="5.0">5.0</option>  
  <option value="10">10</option>  
</select>
<input type="button" value="Set" onClick="javascript: setFOV(document.getElementById('FOV').value);" style="background-color:#000000; color:#FF0000">
<p>
<input type="button" value="View Astrometry Log" onclick="window.open('astrometry.txt')"; " style="background-color:#000000; color:#FF0000">
<p>

<hr color="#4A4344">
<b><u>Mount Control and Skychart</u></b>
<p>
<input type="button" value="Start Skychart" onClick="javascript: startskychart();" style="background-color:#000000; color:#F6CECE">
&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;
<input type="button" value="Skychart (direct mount control)" onclick="window.open('skychart/skychart.html')"; " style="background-color:#000000; color:#FF0000">
&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;
<input type="button" value="Skychart (via TheSkyX)" onclick="window.open('skychart2TSX/skychart.html')"; " style="background-color:#000000; color:#FF0000">
<p>
<input type="button" value="Stop Skychart" onClick="javascript: stopskychart();" style="background-color:#000000; color:#FF0000">
<p>
<input type="button" value="Start Skychart to Setup via VNC" onClick="javascript: startskychartvnc();" style="background-color:#000000; color:#FF0000">
<p>
Protocol:
<input type="button" value="LX200" onClick="javascript: set_lx200();" style="background-color:#000000; color:#FF0000">
<input type="button" value="iOptron" onClick="javascript: set_iOptron();" style="background-color:#000000; color:#FF0000">
<input type="button" value="Nexstar/Skywatcher" onClick="javascript: set_nexstar();" style="background-color:#000000; color:#FF0000">
<input type="button" value="TheSkyX" onClick="javascript: set_TSX();" style="background-color:#000000; color:#FF0000">
<p>
IP Address of TheSkyX Server:
<input type="text" size="15" value="" name="IP_TSX" id="IP_TSX" style="background-color:#302217; color:#FF0000"/>
<input type="button" value="Set" onClick="javascript: setIP_TSX(document.getElementById('IP_TSX').value);" style="background-color:#000000; color:#FF0000">
<p>

<hr color="#4A4344">
<b><u>Focuser</u></b>
<p>
<input type="button" value="Swap Focuser Directions" onClick="javascript: focus_swap();" style="background-color:#000000; color:#FF0000">
<p>
<input type="button" value="View Focus Log" onclick="window.open('focus_info.txt')"; " style="background-color:#000000; color:#FF0000">
<p>
<input type="button" value="DELETE Focus Log" onClick="javascript: clearfocuslog();" style="background-color:#000000; color:#FF0000">
<p>

<hr color="#4A4344">
<b><u>Pushover Notification</u></b>
<p>
Application Key: 
<input type="text" size="30" value="" name="pushover_ap" id="pushover_ap" style="background-color:#302217; color:#FF0000"/>
User Key:
<input type="text" size="30" value="" name="pushover_user" id="pushover_user" style="background-color:#302217; color:#FF0000"/>
<input type="button" value="Set" onClick="javascript: setPushover(document.getElementById('pushover_ap').value,document.getElementById('pushover_user').value);" style="background-color:#000000; color:#FF0000">

<p>
Completion of imaging sequence:
<?php
if (file_exists('/home/pi/www/notify_imaging')) {
    $btn_notify_imaging = 'Press to Disable';
} else {
	$btn_notify_imaging = 'Press to Enable';
} 
?>
<input type="button" id="notify_imaging" value="<?= htmlentities($btn_notify_imaging) ?>" onClick="javascript: notify_imaging();" style="background-color:#000000; color:#FF0000">

<p>
Guiding errors when exceeding tolerance:
<?php
if (file_exists('/home/pi/www/notify_guiding')) {
    $btn_notify_guiding = 'Press to Disable';
} else {
	$btn_notify_guiding = 'Press to Enable';
} 
?>
<input type="button" id="notify_guiding" value="<?= htmlentities($btn_notify_guiding) ?>" onClick="javascript: notify_guiding();" style="background-color:#000000; color:#FF0000">

<hr color="#4A4344">

<b><u>File Management</u></b>
<p>
<input type="button" value="Mount USB Flash Drive" onClick="javascript: mount();" style="background-color:#000000; color:#FF0000">
&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;
<input type="button" value="Unmount USB Flash Drive" onClick="javascript: unmount();" style="background-color:#000000; color:#FF0000">
<p>
<input type="button" value="Download Images to USB Flash Drive" onClick="javascript: downloadimages();" style="background-color:#000000; color:#FF0000">
&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;
<input type="button" value="File Status" onClick="javascript: file_getStatus();" style="background-color:#000000; color:#FF0000">
<p>
<input type="button" value="DELETE Images" onClick="javascript: deleteimages();" style="background-color:#000000; color:#FF0000">
<p>

<hr color="#4A4344">

<b><u>System</u></b>

<p>
Set Default Page:
<input type="button" value="QSI or SX" onClick="javascript: default_qsi();" style="background-color:#000000; color:#FF0000">
<input type="button" value="SBIG ATIK or QHY" onClick="javascript: default_ccd();" style="background-color:#000000; color:#FF0000">
<input type="button" value="Canon or Nikon" onClick="javascript: default_dslr();" style="background-color:#000000; color:#FF0000">
<input type="button" value="Pentax" onClick="javascript: default_pentax();" style="background-color:#000000; color:#FF0000">
<input type="button" value="INDI" onClick="javascript: default_INDI();" style="background-color:#000000; color:#FF0000">
<p>
Set Network SSID: 
<input type="text" size="15" value="" name="net_name" id="net_name" style="background-color:#302217; color:#FF0000"/>
Password:
<input type="text" size="15" value="" placeholder="8+ characters" name="net_pwd" id="net_pwd" style="background-color:#302217; color:#FF0000"/>
<input type="button" value="Set" onClick="javascript: setNetwork(document.getElementById('net_name').value,document.getElementById('net_pwd').value);" style="background-color:#000000; color:#FF0000">
<!--
<p>
<input type="button" value="Start/Restart Pagekite" onClick="javascript: startPK();" style="background-color:#000000; color:#FF0000">
-->
<p>
<input type="button" value="Start VNC Server" onClick="javascript: vnc();" style="background-color:#000000; color:#FF0000">
&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;
<input type="button" value="VNC for LinGuider/Skychart" onclick="window.open('noVNC/vnc.html?autoconnect=true&host=10.0.0.1&port=5551')"; " style="background-color:#000000; color:#FF0000">
<input type="button" value="VNC for OpenSkyImager" onclick="window.open('noVNC/vnc.html?autoconnect=true&host=10.0.0.1&port=5552')"; " style="background-color:#000000; color:#FF0000">
<input type="button" value="VNC for GoQat" onclick="window.open('noVNC/vnc.html?autoconnect=true&host=10.0.0.1&port=5553')"; " style="background-color:#000000; color:#FF0000">
<p>
<input type="button" value="Upload Patch from USB Flash Drive" onClick="javascript: uploadpatch();" style="background-color:#000000; color:#FF0000">
&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;
<input type="button" value="File Status" onClick="javascript: file_getStatus();" style="background-color:#000000; color:#FF0000">
<p>
<input type="button" value="System Info" onclick="window.open('sysinfo/index.php')"; " style="background-color:#000000; color:#FF0000">
<input type="button" value="About" onClick="document.getElementById('returned_msg').innerHTML = 'iAstroHub is mainly developed by Anat Ruangrassamee, Ph.D. (Bangkok, Thailand). And the efforts by the following contributors are highly acknowledged.\nCopyright 2012-2016  \n\nCredits (in the alphabetical order): \nAndrew Stepanenko and Rumen Bogdanovski, Developer of LinGuider used for controlling an autoguider \nDustin Lang, Developer of Astrometry.net used for plate-solving  \nEdward Simonson, Developer of GoQat used for controlling CCD cameras \nGiampiero Spezzano, Developer of OpenSkyImager used for controlling CCD cameras \nJasem Mutlaq, Developer of INDI tools  \nJean-Luc (geehalel), Developer of INDI tools  \nPatrick Chevalley, Developer of SkyChart (Cartes du Ciel) \nPeter Polakovic, Developer of INDI tools  \nPierluigi Vittori (Pentax camera control) \nRichard Garanger, Main author of the manual  \nStephane Lucas (beta-testing) \nZhirong Li and Hongyun Qiu (QHYCCD) \nZachary Wu (Pentax camera and mount control) \nZoltan Hubai, Developer of qDslrDashboard and DslrDashboard Server ';"  style="background-color:#000000; color:#FF0000">
<p>
<input type="button" value="REBOOT iAstroHub" onClick="javascript: reboot();" style="background-color:#FF0000; color:#000000">
<p>
<input type="button" value="SHUTDOWN iAstroHub" onClick="javascript: shutdown();" style="background-color:#FF0000; color:#000000">


<hr color="#4A4344">

<pre STYLE="font-size: large">
<span id="returned_msg"></span>
</pre>

 </body>
</html>
