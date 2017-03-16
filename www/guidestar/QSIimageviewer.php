<!DOCTYPE html>
<html>
    <head>
        <meta http-equiv="Content-Type" content="text/html; charset=utf-8" />
        <title>Image Viewer</title>
        <script type="text/javascript" src="jquery.js" ></script>
        <script type="text/javascript" src="jqueryui.js" ></script>
        <script type="text/javascript" src="jquery.mousewheel.min.js" ></script>
        <script type="text/javascript" src="../jquery.iviewer.js" ></script>
		
		<link rel="stylesheet" href="colorbox.css" />
		<script src="jquery.colorbox.js"></script>
		<script src="highcharts.js"></script>
		
    </head>
	
	
	
    <body bgcolor="#000000" TEXT="#FF0000" LINK="#FF0000" VLINK="#FF0000">
		<script id="source" language="javascript" type="text/javascript">
	
	var xmlhttp;
	var flg_Hist = 0;
	var flg_getHist = 0;
	var st="";
	var st_tmp="";
	var chart;
	
	$(document).ready(function(){
		$(".inline").colorbox({inline:true, width:false});
	});
		
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
		
		
	function resetHist() {
	
	if (flg_Hist == 0) {

		xmlhttp = GetXmlHttpObject();

		var url = "../setHist_QSI.php?L=" + 0 + "&U=" + 65535;
		flg_Hist = 1;
				
		xmlhttp.onreadystatechange = handleResponse;
		xmlhttp.open("GET",url,true);
		xmlhttp.send(null);
		
		document.getElementById('L').value = 0; 
		document.getElementById('U').value = 65535; 	
		
		document.getElementById('status').innerHTML = "Processing...";
		
	} else {
	
		document.getElementById('status').innerHTML = "Previous job is being processed...";
	
	}
	
	}
	
	
	
	function getHist() {
	
	if (flg_Hist == 0) {

		xmlhttp = GetXmlHttpObject();

		var url = "../getHist.php";
		flg_getHist = 1;
				
		xmlhttp.onreadystatechange = handleResponse;
		xmlhttp.open("GET",url,true);
		xmlhttp.send(null);
		
	} else {
	
		document.getElementById('status').innerHTML = "Previous job is being processed...";
	
	}
		
	}
	
	
	function setHist(L,U) {
	
	if (flg_Hist == 0) {
	
		L=L*1;
		U=U*1;
	
		if (is_int(L) && is_int(U) && L<U && L>=0 && U<=65535) {

			xmlhttp = GetXmlHttpObject();

			var url = "../setHist_QSI.php?L=" + L + "&U=" + U;
			flg_Hist = 1;
				
			xmlhttp.onreadystatechange = handleResponse;
			xmlhttp.open("GET",url,true);
			xmlhttp.send(null);
			
		    document.getElementById('status').innerHTML = "Processing...";
			
		} else {
		
			alert("Min and Max values must be integers from 0 to 65535.");
		
		}
	
	} else {
	
		document.getElementById('status').innerHTML = "Previous job is being processed...";
	
	}

	}
		
	function handleResponse() {
		if(xmlhttp.readyState == 4) {
			st = xmlhttp.responseText;
			
			st_tmp=st.substr(1,4);
			if (st_tmp == "IIII"){
				$("#viewer1").iviewer('loadImage','../image.jpg' + '?' + (new Date()).getTime() );
				document.getElementById('status').innerHTML = "Done.";
				flg_Hist = 0;
			}
			if (st_tmp == "OOOO"){
				alert("No image!");
				document.getElementById('status').innerHTML = " ";
				flg_Hist = 0;
			}
			if (st_tmp == "XXXX"){
				alert("Processing the downloaded image from the camera!");
				document.getElementById('status').innerHTML = " ";
				flg_Hist = 0;
			}
			
			if (flg_getHist == 1){
				str = st.split ( " " );
				L=str[0]*1;
				U=str[1]*1;
				document.getElementById('L').value = L;
				document.getElementById('U').value = U;
				flg_getHist = 0;	
			}				
			
		}
	}
		
		
		
	function is_int(value){
	  if((parseFloat(value) == parseInt(value)) && !isNaN(value)){
		  return true;
	  } else {
		  return false;
	  }
	}		
		

	function refresh() {
	
		document.location.reload(true);
	
	}	
	
	
	function load(filename) {
	
		$("#viewer1").iviewer('loadImage','../images/' + filename + '?' + (new Date()).getTime() );
		
		xmlhttp = GetXmlHttpObject();
		var url = "loadimage.php?file=" + filename;
		xmlhttp.onreadystatechange = handleResponse;
		xmlhttp.open("GET",url,true);
		xmlhttp.send(null);
		
	}
	
	function platesolved() {
	
		$("#viewer1").iviewer('loadImage','../image-ngc.png' + '?' + (new Date()).getTime() );
		
	}		
	
	jQuery(function($){
		
                var viewer;
        
                viewer = $("#viewer1").iviewer({
                    src: "../image.jpg?" + (new Date()).getTime(),
					
                    initCallback: function(ev) {
                        this.container.context.iviewer = this;
                    }
                });
	});
	
	
$(function () {

	data_x = [ <?php readfile('/home/pi/www/histogram_x.txt') ?> ];
    data_y = [ <?php readfile('/home/pi/www/histogram_y.txt') ?> ];
	
	data = new Array (257);
	for (i = 0; i < data.length; ++ i)
		data [i] = new Array (2);
		
	for (i = 0; i < data.length; ++ i) {
		data [i] [0] = data_x [i];
		data [i] [1] = data_y [i];
	}
	
	
	
        chart = new Highcharts.Chart({
            chart: {
                renderTo: 'container',
                zoomType: 'x',
				resetZoomButton: {
					theme: {
						fill: 'white',
						stroke: 'silver',
						r: 0,
						states: {
							hover: {
								fill: '#FF0000',
								style: {
									color: 'white'
								}
							}
						}
					}
				},					
                spacingRight: 20,
				backgroundColor: '#000000'
            },
            title: {
                text: ' '
            },
            credits: {
                enabled: false
            },			
			colors: [
				'#FF0000', 
				'#FF0000'
			],
            xAxis: {
				lineColor: '#FF0000',
                maxZoom: 3
            },
            yAxis: {
				gridLineColor: '#000000',
                title: {
                    text: ' '
                },
                showFirstLabel: false
				
            },
            tooltip: {
                shared: true,
				backgroundColor: '#FCFFC5'
            },
            legend: {
                enabled: false
            },
            plotOptions: {
				series: {
					marker: {
						enabled: false,
						states: {
							hover: {
								enabled: true
							}
						}
					}
				}
            },
    
            series: [{
                name: 'histogram',
                data: data
				
            }]
        });
	
	
	    $('#button_hist').click(function() {
		
			var extremes = chart.xAxis[0].getExtremes();
			if ( Math.round(extremes.min) < Math.round(extremes.dataMin) ) {
				document.getElementById('L').value = Math.round(extremes.dataMin);
			} else {
				document.getElementById('L').value = Math.round(extremes.min);
			}
			if ( Math.round(extremes.max) > Math.round(extremes.dataMax) ) {
				document.getElementById('U').value = Math.round(extremes.dataMax);
			} else {
				document.getElementById('U').value = Math.round(extremes.max);
			}			
			$.colorbox.close();
			
		});
	
    
});	
	
	
	
			
			
        </script>
		
        <link rel="stylesheet" href="../jquery.iviewer.css" />
        <style>
            html, body {
                height: 100%;
                padding: 0;
                margin: 0;
				background-color: black;
				color: red;
            }
        
            .viewer
            {
                height: 100%;
                position: relative;
                background-color: black;
            }
            
            .wrapper
            {
                border: 1px solid red;

                position: absolute;
                top: 5em;
                left: 1em;
                bottom: 1em;
                right: 1em;

                overflow: hidden; /*for opera*/
            }
            
            .toolbar
            {
                border: 1px solid red;

                position: absolute;
                top: 1em;
                left: 1em;
                right: 1em;
                height: 3em;
            }
        </style>


        <div class="toolbar">
<input type="button" value="Refresh" onClick="javascript: refresh();" style="background-color:#000000; color:#FF0000">		

<select name="image" id="image" style="background-color:#000000; color:#FF0000">

<?php

	function scan_dir($dir) {
		$ignored = array('.', '..');

		$files = array();    
		foreach (scandir($dir) as $file) {
			if (in_array($file, $ignored)) continue;
			if (strpos($file,'.fit') == true) continue;
			$files[$file] = filemtime($dir . $file);
		}

		arsort($files);
		$files = array_keys($files);

		return ($files) ? $files : false;
	}	
	
    $dir = '/home/pi/www/images/';
    $images = array();
	$images =  scan_dir($dir);
	
    foreach ($images as $option) {
?>

  <option value="<?= htmlentities($option) ?>">
    <?= htmlentities($option) ?>
  </option>
<?php } ?>

</select>

<input type="button" value="Load" onClick="javascript: load(document.getElementById('image').value);" style="background-color:#000000; color:#FF0000">		

<!-- 
<input type="button" value="Plate-Solved Image" onClick="javascript: platesolved();" style="background-color:#000000; color:#FF0000">		
-->

&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;
<a class='inline' href="#inline_content">Histogram</a>
<input type="button" value="Get" onClick="javascript: getHist();" style="background-color:#000000; color:#FF0000">
<input type="text" size="5" value="" name="L" id="L" placeholder="Min" style="background-color:#302217; color:#FF0000"/>
<input type="text" size="5" value="" name="U" id="U" placeholder="Max" style="background-color:#302217; color:#FF0000"/>
<input type="button" value="Apply" onClick="javascript: setHist(document.getElementById('L').value,document.getElementById('U').value);" style="background-color:#000000; color:#FF0000">
<input type="button" value="Reset" onClick="javascript: resetHist();" style="background-color:#000000; color:#FF0000">


<span id="status"></span>

	

        </div>
		
		
        <div class="wrapper">
            <div id="viewer1" class="viewer"></div>
        </div>
		
		
		<!-- This contains the hidden content for inline calls -->
		<div style='display:none'>
			<div id='inline_content' style='padding:10px; background:#000000;'>
				<div id="container" style="min-width: 600px; height: 300px; margin: 0 auto"></div>
				<button id="button_hist" style="background-color:#302217; color:#FF0000" >Get Min:Max</button>
			</div>
		</div>			
		
		
            
    </body>
	
	
</html>
