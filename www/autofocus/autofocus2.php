<!DOCTYPE html>
<html>
    <head>
        <meta http-equiv="Content-Type" content="text/html; charset=utf-8" />
        <title>Autofocus</title>
        <script type="text/javascript" src="jquery.js" ></script>
		<script type="text/javascript" src="jquery.flot.js"></script>
        <script type="text/javascript" src="jqueryui.js" ></script>
        <script type="text/javascript" src="jquery.mousewheel.min.js" ></script>
        <script type="text/javascript" src="jquery.iviewer.js" ></script>
		<script type="text/javascript" src="regression.min.js"></script>
		
		<link rel="stylesheet" href="colorbox.css" />
		<script src="jquery.colorbox.js"></script>
		<script src="highcharts.js"></script>
    </head>
	
    <body LINK="#FF0000" VLINK="#FF0000">
	        <div class="toolbar">


<input type="button" value="Step" onClick="javascript: focus_update();" style="background-color:#000000; color:#F6CECE">
<input type="text" size="7" value="" name="pos" id="pos" readonly="readonly" style="background-color:#302217; color:#F6CECE"/>
&nbsp;&nbsp;&nbsp;
<b><font color="#F6CECE">HFD</font></b>
<input type="text" size="7" value="" name="fwhm" id="fwhm" readonly="readonly" style="background-color:#302217; color:#F6CECE"/>
&nbsp;&nbsp;&nbsp;
<b><font color="#F6CECE">Status</font></b>
<input type="text" size="15" value="" name="image" id="image" readonly="readonly" style="background-color:#302217; color:#F6CECE"/>
<p>

#Steps
<input type="text" size="5" value="" name="move" id="move" style="background-color:#302217; color:#FF0000"/>
<select name="dir" id="dir" style="background-color:#000000; color:#FF0000">
  <option value="0" >In</option>
  <option value="1" >Out</option>
</select>
<input type="button" value="Move" onClick="javascript: move_io(document.getElementById('dir').value,document.getElementById('move').value);" style="background-color:#000000; color:#FF0000">
&nbsp;&nbsp;&nbsp;
<input type="button" value="Move To" onClick="javascript: move_to(document.getElementById('to_pos').value);" style="background-color:#000000; color:#FF0000">
<input type="text" size="5" value="" name="to_pos" id="to_pos" style="background-color:#302217; color:#FF0000"/>
<p>

Exp(s)
<input type="text" size="5" value="" name="exposure" id="exposure" style="background-color:#302217; color:#FF0000"/>
<input type="button" value="Capture" onClick="javascript: ccd(document.getElementById('exposure').value);" style="background-color:#000000; color:#FF0000">
<a class='inline' href="#inline_content">Histogram</a>
<!--
<select name="size" id="size" style="background-color:#000000; color:#FF0000">
  <option value= "0">&sdotb;</option>
  <option value="16">16</option>
  <option value="32">32</option>
  <option value="64">64</option>
</select>
-->
<input type="button" value="Select" onClick="javascript: select_star(64);" style="background-color:#000000; color:#FF0000">
<input type="button" value="Reset" onClick="javascript: reset();" style="background-color:#000000; color:#FF0000">
&nbsp;&nbsp;&nbsp;
<input type="button" value="HFD" onClick="javascript: vcurve(1,1,1,document.getElementById('exposure').value,document.getElementById('size').value);" style="background-color:#000000; color:#FF0000">
<p>

#Points
<input type="text" size="3" value="" name="point" id="point" style="background-color:#302217; color:#FF0000"/>
<input type="button" value="Generate V-Curve" onClick="javascript: vcurve(document.getElementById('point').value,document.getElementById('move').value,document.getElementById('dir').value,document.getElementById('exposure').value,document.getElementById('size').value);" style="background-color:#000000; color:#FF0000">
&nbsp;&nbsp;&nbsp;
<input type="button" value="Stop" id="btn_stop" onClick="javascript: stop_main();" style="background-color:#FF0000; color:#000000">
<!--
<p>
<input type="button" value="Load Image at Step#" onClick="javascript: load(document.getElementById('imagefile').value);" style="background-color:#000000; color:#FF0000">		
<span id="returned_msg"></span>
-->

        </div>

	
            
        <div class="wrapper">
            <div id="viewer1" class="viewer"></div>
        </div>
		
		
		<div class="graph">

<div id="placeholder" style="width:100%;height:100%"></div>



        </div>
		
		
		<!-- This contains the hidden content for inline calls -->
		<div style='display:none'>
			<div id='inline_content' style='padding:10px; background:#000000;'>
				<div id="container" style="min-width: 600px; height: 300px; margin: 0 auto"></div>
				<button id="button_hist" style="background-color:#302217; color:#FF0000" >Apply</button>
			</div>
		</div>			
	
	
	
	
	
	
	
	
	
		<script id="source" language="javascript" type="text/javascript">



var data_x = [];
var data_y = [];
var data = [];

var hist_x = [];
var hist_y = [];
var hist_data = [];

var options = {
        series: {
            lines: { show: true },
            points: { show: false },
	        color: "#FF0000"
        },
        xaxis: {
//            ticks: [0, 10, 20, 30, 40, 50, 60],
//            min: 0,
//            max: 60,
        },
        yaxis: {
//            ticks: 10,
//            min: -2,
//            max: 2
        },
	    grid: {
			color: "#FF0000",
			tickColor: "#4A4344"
        }
};

var placeholder = $("#placeholder");
		
		
		
	var xmlhttp;
	var st;
	
var flg_focus=0;
var flg_ccd=0;
var flg_hist=0;

var flg_vcurve=0;
	
	var xcoord=0;
	var ycoord=0;
	
	
	
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
	
	
	
	function stop_main() {
	
		if(flg_vcurve == 0 && flg_ccd == 0 && flg_focus == 0) {	
	
			stop_all();
	
		} else {
	
			focus_stop();
			stop_ccd();
			stop_vcurve();
			
		}
		
	}	
	
	
	function stop_all() {
	
	if(flg_vcurve == 0 && flg_ccd == 0 && flg_focus == 0) {

		xmlhttp15 = GetXmlHttpObject();

		var url15 = "stop_all.php";
				
		xmlhttp15.onreadystatechange = handleResponse15;
		xmlhttp15.open("GET",url15,true);
		xmlhttp15.send(null);
		
	}

	}
	
	
	function handleResponse15() {
		if(xmlhttp15.readyState == 4) {
			st15 = xmlhttp15.responseText;
			document.getElementById('pos').value = " ";
			document.getElementById('fwhm').value = " ";
			document.getElementById('image').value = "All Stopped.";
		}
	}	
	
	
	function move_io(dir,move) {
	
	if (is_int(move) && move>=1 && dir>=0 && dir<=1) {

		if(flg_vcurve == 0 && flg_ccd == 0) {
		
			if((flg_focus == 0) && (document.getElementById('pos').value != "Moving...")) {

				xmlhttp6 = GetXmlHttpObject();
				
				
				if (dir==0){
					var url6 = "move_in.php?move=" + move;
				} else {
					var url6 = "move_out.php?move=" + move;
				}
				
				flg_focus=1;

				xmlhttp6.onreadystatechange = handleResponse6;
				xmlhttp6.open("GET",url6,true);
				xmlhttp6.send(null);
			
				document.getElementById('pos').value = "Moving...";
			
			} else {
			
				alert("Focuser is moving.");
			
			}

		} else {
		
			alert("Imaging in progress...");
			
		}
		
	} else {
	
		alert("The value must be an integer >= 1.");

	}

	}	
	
	
	function move_to(to_pos) {
	
	if (is_int(to_pos) && to_pos>=0 && to_pos<=65535) {

		if(flg_vcurve == 0 && flg_ccd == 0) {
		
			if((flg_focus == 0) && (document.getElementById('pos').value != "Moving...")) {
		
				xmlhttp6 = GetXmlHttpObject();

				var url6 = "move_to.php?to_pos=" + to_pos;
				flg_focus=1;

				xmlhttp6.onreadystatechange = handleResponse6;
				xmlhttp6.open("GET",url6,true);
				xmlhttp6.send(null);
			
				document.getElementById('pos').value = "Moving...";
				
			} else {
			
				alert("Focuser is moving.");
			
			}

		} else {
		
			alert("Some jobs in progress...");
			
		}
		
	} else {
	
		alert("The value must be an integer in the range of 0 to 65535.");

	}

	}	
	
	
	function focus_update() {
	
		if(flg_vcurve == 0 && flg_ccd == 0 && flg_focus == 0) {
	
				xmlhttp6 = GetXmlHttpObject();

				var url6 = "focus_update.php";

				xmlhttp6.onreadystatechange = handleResponse6;
				xmlhttp6.open("GET",url6,true);
				xmlhttp6.send(null);
				
		} else {
		
			alert("Some jobs in progress...");
			
		}
				
	}
	
	
	function focus_stop() {
	
		if(flg_vcurve == 0 && flg_ccd == 0 && flg_focus == 1) {
	
				xmlhttp6 = GetXmlHttpObject();

				var url6 = "focus_stop.php";
				flg_focus=0;

				xmlhttp6.onreadystatechange = handleResponse6;
				xmlhttp6.open("GET",url6,true);
				xmlhttp6.send(null);
			
				document.getElementById('pos').value = "Stopped.";
				
		}
				
	}	
	
	
	function focus_read() {
	
				xmlhttp6 = GetXmlHttpObject();

				var url6 = "focus_read.php";

				xmlhttp6.onreadystatechange = handleResponse6;
				xmlhttp6.open("GET",url6,true);
				xmlhttp6.send(null);
			
	}	
	
	
	function handleResponse6() {
		if(xmlhttp6.readyState == 4) {
			var temptext6 = xmlhttp6.responseText;
			
			if (flg_focus == 1) {  // focusing
				var temptext6_tmp=temptext6.substr(temptext6.length-8,5);
				if (temptext6_tmp == "SSSSS"){
					flg_focus=0;
					focus_update();
				} else {
					document.getElementById('pos').value = "Moving...";
					setTimeout(focus_read(), 1000);
				}
			} else {
				document.getElementById('pos').value = temptext6;
			}
			
		}
	}
	
	
	
	
	
	
	
	function ccd(exposure) {
	
	if (isNumber(exposure) && exposure>=0.1 && exposure<=30) {

		if(flg_vcurve == 0 && flg_ccd == 0) {
		
			filter=0;
			nframe=1;
			filename="PREVIEW";

			xmlhttp2 = GetXmlHttpObject();

			var url2 = "ccd.php?filter=" + filter + "&ex=" + exposure + "&nf=" + nframe + "&filename=" + filename;
			flg_ccd=1;

			xmlhttp2.onreadystatechange = handleResponse2;
			xmlhttp2.open("GET",url2,true);
			xmlhttp2.send(null);
			
			document.getElementById('fwhm').value = " ";

			monitor_ccd();

		} else {
		
			document.getElementById('image').value = "Busy!";
			
		}
		
	} else {
	
		alert("Exposure must be in the range of 0.1-30 sec.");

	}

	}

	function stop_ccd() {
	
	if(flg_vcurve == 0 && flg_ccd == 1 && flg_focus == 0) {

		xmlhttp2 = GetXmlHttpObject();

		var url2 = "stop_ccd.php";
		flg_ccd=0;
				
		xmlhttp2.onreadystatechange = handleResponse2;
		xmlhttp2.open("GET",url2,true);
		xmlhttp2.send(null);
		
	}

	}

	function ReadCCD() {
		xmlhttp2 = GetXmlHttpObject();

		var url2 = "read_ccd.php";
				
		xmlhttp2.onreadystatechange = handleResponse2;
		xmlhttp2.open("GET",url2,true);
		xmlhttp2.send(null);
	}	
	
	
	function handleResponse2() {
		if(xmlhttp2.readyState == 4) {
			st2 = xmlhttp2.responseText;
			
				st2_tmp=st2.substr(0,5);
				
				if (st2_tmp=="*****" && flg_ccd==1){
					flg_ccd=0;
					st2_tmp="";
					document.getElementById('fwhm').value = " ";
					$("#viewer1").iviewer('loadImage','tmp_images/preview.jpg' + '?' + (new Date()).getTime() );
					read_hist_x();
				}
				else {
				
					if (st2_tmp == "xxxxx"){
						flg_ccd=0;
						document.getElementById('image').value = " ";
						alert("Imaging in progress!");
					}
					else {
						document.getElementById('image').value = st2;
					}
				
				}
				
		}
	}
	
	
	
	function select_star(size) {
	
	if (size != 0){
	
	if (xcoord>0 && xcoord < $("#viewer1").iviewer('info','orig_width',true) && ycoord>0 && ycoord < $("#viewer1").iviewer('info','orig_height',true) ) {
	
		if(flg_vcurve == 0 && flg_ccd == 0) {

			xmlhttp3 = GetXmlHttpObject();

			var url3 = "select.php?x=" + xcoord + "&y=" + ycoord + "&size=" + size;
				
			xmlhttp3.onreadystatechange = handleResponse3;
			xmlhttp3.open("GET",url3,true);
			xmlhttp3.send(null);
						
		} else {
		
			alert("Busy!");
			
		}		

	} else {
	
		alert("Select a new position!");

	}
	
	} else {
	
		alert("Select the square size!");
	
	}
	
	}
	
	
	function handleResponse3() {
		if(xmlhttp3.readyState == 4) {
			st3 = xmlhttp3.responseText;
			document.getElementById('fwhm').value = st3;
			$("#viewer1").iviewer('loadImage','tmp_images/crop.jpg' + '?' + (new Date()).getTime() );
			document.getElementById('image').value = " ";
		}
	}
	


	function reset() {
	
		if(flg_vcurve == 0 && flg_ccd == 0) {

			xmlhttp4 = GetXmlHttpObject();

			var url4 = "reset.php";
				
			xmlhttp4.onreadystatechange = handleResponse4;
			xmlhttp4.open("GET",url4,true);
			xmlhttp4.send(null);
						
		} else {
		
			alert("Busy!");
			
		}		

	}	

	
	function handleResponse4() {
		if(xmlhttp4.readyState == 4) {
			st4 = xmlhttp4.responseText;
			document.getElementById('fwhm').value = " ";
			document.getElementById('image').value = "Select a star.";
			$("#viewer1").iviewer('loadImage','tmp_images/preview.jpg' + '?' + (new Date()).getTime() );
		}
	}
	
	
	
	
	function vcurve(point,move,dir,exposure,size) {
	
	if (is_int(point) && point>=1 && is_int(move) && move>=1 && isNumber(exposure) && exposure>=0.001 && exposure<=30) {

		if(flg_vcurve == 0 && flg_ccd == 0 && flg_focus == 0) {

			xmlhttp7 = GetXmlHttpObject();

			var url7 = "vcurve.php?point=" + point + "&move=" + move + "&dir=" + dir + "&exposure=" + exposure + "&size=" + size;
			flg_vcurve=1;

			xmlhttp7.onreadystatechange = handleResponse7;
			xmlhttp7.open("GET",url7,true);
			xmlhttp7.send(null);
			
			document.getElementById('pos').value = " ";
			document.getElementById('image').value = " ";
			document.getElementById('fwhm').value = " ";
		//	document.getElementById('returned_msg').innerHTML = "";			
			
			monitor_vcurve();

		} else {
		
			alert("Busy!");
			
		}
		
	} else {
	
		alert("Invalid values! Please check.");

	}

	}

	function stop_vcurve() {
	
	if(flg_vcurve == 1 && flg_ccd == 0 && flg_focus == 0) {

		xmlhttp7 = GetXmlHttpObject();

		var url7 = "stop_vcurve.php";
		flg_vcurve = 0;
				
		xmlhttp7.onreadystatechange = handleResponse7;
		xmlhttp7.open("GET",url7,true);
		xmlhttp7.send(null);
		
		document.getElementById('pos').value = " ";
		document.getElementById('fwhm').value = " ";			
		document.getElementById('image').value = "Stopped.";
		refresh();
		
	}

	}

	function read_vcurve() {
		xmlhttp7 = GetXmlHttpObject();

		var url7 = "read_vcurve.php";
				
		xmlhttp7.onreadystatechange = handleResponse7;
		xmlhttp7.open("GET",url7,true);
		xmlhttp7.send(null);
	}	
	
	
	function handleResponse7() {
		if(xmlhttp7.readyState == 4) {
			st7 = xmlhttp7.responseText;
				st7_tmp=st7.substr(0,5);

				if (st7_tmp=="====="){
					st7_tmp="";
					document.getElementById('pos').value = "Moving...";
				}
				if (st7_tmp=="+++++"){
					st7_tmp="";
					document.getElementById('pos').value = st7.substr(5,5);
					document.getElementById('image').value = " ";
					document.getElementById('fwhm').value = " ";			
				}
				if (st7_tmp==">>>>>"){
					st7_tmp="";
					document.getElementById('image').value = "Capturing...";
				}
				if (st7_tmp=="-----"){
					st7_tmp="";
					append_vcurve();
					document.getElementById('image').value = "Downloaded.";
					document.getElementById('fwhm').value = st7.substr(5,5);
					$("#viewer1").iviewer('loadImage','tmp_images/image.jpg' + '?' + (new Date()).getTime() );
					read_data_x();
				}
				if (st7_tmp=="_____"){
					st7_tmp="";
					flg_vcurve = 0;
					document.getElementById('image').value = "Done.";
					refresh();
					
					// do the regression (polynomial)
					var myRegression = regression('polynomial', data, 2);
					var coeff = myRegression.equation;
					var xvalue = (-1)*coeff[1]/(2*coeff[2]);
					xvalue=Math.round(xvalue);
					
					if (xvalue>Math.min(data_x[0],data_x[data_x.length-1]) && xvalue<Math.max(data_x[0],data_x[data_x.length-1])){
						//alert("V-curve is done. Best focus point = " + xvalue);
						document.getElementById('to_pos').value=xvalue;
					} else {
						//alert("V-curve is done. Best focus point is outside V-curve!");
						document.getElementById('to_pos').value=" ";
					}
				}
				if (st7_tmp=="^^^^^"){
					st7_tmp="";
					flg_vcurve = 0;
					document.getElementById('image').value = " ";
					document.getElementById('fwhm').value = " ";			
					alert("Select a star!");
				}				
				if (st7_tmp=="eeeee"){
					st7_tmp="";
					flg_vcurve = 0;
					document.getElementById('image').value = " ";
					document.getElementById('fwhm').value = " ";			
					alert("Error while capturing!");
				}	

				
		}
	}
	
	
	
	function append_vcurve() {
	
		xmlhttp9 = GetXmlHttpObject();
		var url9 = "append_vcurve.php";
		xmlhttp9.onreadystatechange = handleResponse9;
		xmlhttp9.open("GET",url9,true);
		xmlhttp9.send(null);
			
	}	

	function handleResponse9() {
		if(xmlhttp9.readyState == 4) {
			var temptext9 = xmlhttp9.responseText;
		}
	}	
	
	
	function read_data_x() {
	
		xmlhttp10 = GetXmlHttpObject();
		var url10 = "read_data_x.php";
		xmlhttp10.onreadystatechange = handleResponse10;
		xmlhttp10.open("GET",url10,true);
		xmlhttp10.send(null);
			
	}	
	
	function handleResponse10() {
		if(xmlhttp10.readyState == 4) {
			var temptext10 = xmlhttp10.responseText;
			data_x = temptext10.substring(0, temptext10.length - 1);
			data_x=data_x.split(' ').map(function(el){ return +el;});
			read_data_y();
		}
	}	
	
	
	function read_data_y() {
	
		xmlhttp11 = GetXmlHttpObject();
		var url11 = "read_data_y.php";
		xmlhttp11.onreadystatechange = handleResponse11;
		xmlhttp11.open("GET",url11,true);
		xmlhttp11.send(null);
			
	}	
	
	function handleResponse11() {
		if(xmlhttp11.readyState == 4) {
			var temptext11 = xmlhttp11.responseText;
			data_y = temptext11.substring(0, temptext11.length - 1);
			data_y=data_y.split(' ').map(function(el){ return +el;});
			plotgraph();
		}
	}
	
	
	
	function read_hist_x() {
	
		xmlhttp12 = GetXmlHttpObject();
		var url12 = "read_hist_x.php";
		xmlhttp12.onreadystatechange = handleResponse12;
		xmlhttp12.open("GET",url12,true);
		xmlhttp12.send(null);
			
	}	
	
	function handleResponse12() {
		if(xmlhttp12.readyState == 4) {
			var temptext12 = xmlhttp12.responseText;
			hist_x = temptext12.substring(0, temptext12.length - 2);
			hist_x=hist_x.split(', ').map(function(el){ return +el;});
			read_hist_y();
		}
	}	
	
	
	function read_hist_y() {
	
		xmlhttp13 = GetXmlHttpObject();
		var url13 = "read_hist_y.php";
		xmlhttp13.onreadystatechange = handleResponse13;
		xmlhttp13.open("GET",url13,true);
		xmlhttp13.send(null);
			
	}	
	
	function handleResponse13() {
		if(xmlhttp13.readyState == 4) {
			var temptext13 = xmlhttp13.responseText;
			hist_y = temptext13.substring(0, temptext13.length - 2);
			hist_y=hist_y.split(', ').map(function(el){ return +el;});
			read_hist();
		}
	}
	
	
function read_hist() {
	
	hist_data = new Array (hist_x.length);
	for (i = 0; i < hist_x.length; ++ i)
		hist_data [i] = new Array (2);
		
	for (i = 0; i < hist_x.length; ++ i) {
		hist_data [i][0]   = hist_x [i];
		hist_data [i][1]   = hist_y [i];
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
                data: hist_data
				
            }]
        });
	
	
	    $('#button_hist').click(function() {
		
		if(flg_vcurve == 0 && flg_ccd == 0) {
		
			var extremes = chart.xAxis[0].getExtremes();
			if ( Math.round(extremes.min) < Math.round(extremes.dataMin) ) {
				L = Math.round(extremes.dataMin);
			} else {
				L = Math.round(extremes.min);
			}
			if ( Math.round(extremes.max) > Math.round(extremes.dataMax) ) {
				U = Math.round(extremes.dataMax);
			} else {
				U = Math.round(extremes.max);
			}
			
			setHist(L,U);
			
		} else {
		
			alert("Busy!");
			$.colorbox.close();
			
		}		
			
		});
	
		
}
	
	
	
	function setHist(L,U) {
	
	if(flg_hist == 0) {
	
		if (is_int(L) && is_int(U) && L<U && L>=0 && U<=65535) {
		
			xmlhttp14 = GetXmlHttpObject();

			var url14 = "setHist.php?L=" + L + "&U=" + U;
			flg_hist = 1;
				
			xmlhttp14.onreadystatechange = handleResponse14;
			xmlhttp14.open("GET",url14,true);
			xmlhttp14.send(null);
			
		} else {
		
			alert("Min and Max values must be integers from 0 to 65535.");
		
		}
		
	}
	
	}
	
	
	
	function handleResponse14() {
		if(xmlhttp14.readyState == 4) {
			st14 = xmlhttp14.responseText;
			
			st14_tmp=st14.substr(1,4);
			if (st14_tmp == "IIII"){
				flg_hist = 0;
				$("#viewer1").iviewer('loadImage','tmp_images/preview.jpg' + '?' + (new Date()).getTime() );
				$.colorbox.close();
			}
			if (st14_tmp == "OOOO"){
				flg_hist = 0;
				alert("Error!");
				$.colorbox.close();
			}
			if (st14_tmp == "XXXX"){
				flg_hist = 0;
				alert("Imaging in progress!");
				$.colorbox.close();
			}
			
		}
	}
	
	
	
	
	
	
	
	
	
	
	
	
	
	
	
	
		
	function refresh() {
	
		//$("#viewer1").iviewer('loadImage','tmp_images/image.jpg' + '?' + (new Date()).getTime() );
		//read_data_x();
		
		xmlhttp = GetXmlHttpObject();
		var url = "refresh.php";
		xmlhttp.onreadystatechange = handleResponse;
		xmlhttp.open("GET",url,true);
		xmlhttp.send(null);
			
	}	
	
	
	function load(filename) {
	
		$("#viewer1").iviewer('loadImage','images/' + filename + '?' + (new Date()).getTime() );
		document.getElementById('fwhm').value = " ";
			
	}
	
	
	function handleResponse() {
		if(xmlhttp.readyState == 4) {
			var temptext = xmlhttp.responseText;
		//	document.getElementById('returned_msg').innerHTML = temptext;
		}
	}	

	
	
	
	
	
	
function plotgraph() {
	
	data = new Array (data_x.length);
	for (i = 0; i < data_x.length; ++ i)
		data [i] = new Array (2);
		
	for (i = 0; i < data_x.length; ++ i) {
		data [i][0]   = data_x [i];
		data [i][1]   = data_y [i];
	}
		
    $.plot(placeholder, [data], options);
	
}

$(function () {
		
                var viewer;

				$.plot(placeholder, [data], options);
        
                viewer = $("#viewer1").iviewer({
           //       src: "tmp_images/image.jpg?" + (new Date()).getTime(),
					
                    onStopDrag: function(ev, coords) {
                        
						 $('#status').html('Star coordinate: ('+coords.x.toFixed(0)+', '+coords.y.toFixed(0)+').');
						 xcoord=coords.x.toFixed(0)*1;
						 ycoord=coords.y.toFixed(0)*1;
						 this.container.css('cursor', null);

                    },
					
                    initCallback: function(ev) {
                        this.container.context.iviewer = this;
                    }
                });
				
				
	
	
});


function monitor_ccd() {

	st2="";
	st2_tmp="";
	flg_ccd=1;

        setTimeout(fetchData3, 1000);

        function fetchData3() {
			if (flg_ccd == 1) {
				ReadCCD();
                setTimeout(fetchData3, 1000);
			}
        }

}


function monitor_vcurve() {

	st7="";
	st7_tmp="";
	flg_vcurve=1;

        setTimeout(fetchData7, 1000);

        function fetchData7() {
            if (flg_vcurve == 1) {
				read_vcurve();
                setTimeout(fetchData7, 1000);
			}
        }

}


function isNumber(n) {

  return !isNaN(parseFloat(n)) && isFinite(n);
  
}


function is_int(value){
  if((parseFloat(value) == parseInt(value)) && !isNaN(value)){
      return true;
  } else {
      return false;
  }
}
			
			
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
                border: 2px solid red;

                position: absolute;
                top: 13em;
                left: 1em;
                bottom: 1em;
                right: 1em;

                overflow: hidden; /*for opera*/
            }
            
            .toolbar
            {
                border: 0px solid red;

                position: absolute;
                top: 1em;
                left: 1em;
                right: 50%;
                height: 11.5em;
            }
			
            .graph
            {
                border: 0px solid red;

                position: absolute;
                top: 1em;
                left: 50%;
                right: 1em;
                height: 11.5em;
            }			
			
        </style>


            
    </body>
	
	
</html>
