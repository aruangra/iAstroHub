<!DOCTYPE html>
<html>
    <head>
        <meta http-equiv="Content-Type" content="text/html; charset=utf-8" />
        <title>Image Viewer</title>
        <script type="text/javascript" src="jquery.js" ></script>
        <script type="text/javascript" src="jqueryui.js" ></script>
        <script type="text/javascript" src="jquery.mousewheel.min.js" ></script>
        <script type="text/javascript" src="../jquery.iviewer.js" ></script>
    </head>
	
	
	
    <body>
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
	
	
	function handleResponse() {
		if(xmlhttp.readyState == 4) {
			st = xmlhttp.responseText;
		}
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

    $dir = '../images/';

    $handle = @opendir($dir);
    $images = array();
    while (false !== ($file = readdir($handle)))
    {
        if (eregi('\.(jpg|bmp)$', $file))
        {
            $images[] = $file;
        }
    }
    closedir($handle);
    rsort($images);

    foreach ($images as $option) {
?>

  <option value="<?= htmlentities($option) ?>">
    <?= htmlentities($option) ?>
  </option>
<?php } ?>

</select>

<input type="button" value="Load Image" onClick="javascript: load(document.getElementById('image').value);" style="background-color:#000000; color:#FF0000">		


            <span id="status"></span>

	

        </div>
            
        <div class="wrapper">
            <div id="viewer1" class="viewer"></div>
        </div>
            
    </body>
	
	
</html>
