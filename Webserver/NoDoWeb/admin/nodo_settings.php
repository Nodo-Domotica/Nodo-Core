<?php 

require_once('../connections/tc.php'); 
require_once('../include/auth.php');
require_once('../include/settings.php');

$page_title = "Setup: Nodo settings";	


/************************************************************************************************
HTTPRequest function														
*************************************************************************************************/
function HTTPRequest($Url){


    
    if (!function_exists('curl_init')){
        die('Sorry cURL is not installed!');
    }
 
    global $nodo_port;
    $ch = curl_init();
    curl_setopt($ch, CURLOPT_URL, $Url);
    curl_setopt($ch, CURLOPT_USERAGENT, "Nodo Web App");
    curl_setopt($ch, CURLOPT_HEADER, 0);
	curl_setopt($ch, CURLOPT_PORT, $nodo_port);
    curl_setopt($ch, CURLOPT_RETURNTRANSFER, true);
    curl_setopt($ch, CURLOPT_TIMEOUT, 10);
 
    $output = curl_exec($ch);
    curl_close($ch);
	
	return $output;
	
	}
/************************************************************************************************
END HTTPRequest function														
*************************************************************************************************/



/************************************************************************************************
Status  read															
*************************************************************************************************/
if (isset($_GET['read'])){
	if ($_GET['read'] == "true"){

		$file="STATUS";
		HTTPRequest("http://$nodo_ip/?id=$nodo_id&password=$nodo_password&event=FileErase%20$file");
		HTTPRequest("http://$nodo_ip/?id=$nodo_id&password=$nodo_password&event=FileLog%20$file");
		HTTPRequest("http://$nodo_ip/?id=$nodo_id&password=$nodo_password&event=Status%20All");
		HTTPRequest("http://$nodo_ip/?id=$nodo_id&password=$nodo_password&event=FileLog");
		
			

		//Read file from Nodo to array
		$scriptraw = explode("\n", HTTPRequest("http://$nodo_ip/?id=$nodo_id&password=$nodo_password&file=$file"));
		
			
		//count total lines in $scriptraw
		$total_script_lines_raw = count($scriptraw);
		
		$x=0;
		
		for($i=0;$i<$total_script_lines_raw;$i++){
			
			//Remove !********************************** start en stop lines 
			$pos = strpos($scriptraw[$i],"!*****");
			
			if($pos === false || $pos > 0) {
				$script[$x] = $scriptraw[$i];
				$x++;
			}
		}
		
		$total_script_lines = count($script);
		
		
	 }
 }

/************************************************************************************************
END status Read															
*************************************************************************************************/
?>





<!DOCTYPE html> 
<html> 
 
<head>
	<meta charset="utf-8">
	<meta name="viewport" content="width=device-width, initial-scale=1"> 
	<title><?php echo $title ?></title> 
	<?php require_once('../include/jquery_mobile.php'); ?>
</head> 
 
<body> 

<!-- Start of main page: -->

<div data-role="page" pageid="main">
 
	<?php require_once('../include/header_admin.php'); ?>
 
	<div data-role="content">	

<h3>UITWERKEN</h3>
		
<div id="settings_div">		
<?php
if (isset($script)){  

	for($i=0;$i<$total_script_lines;$i++){
		
			
			//<br /> aan het einde van de regels verwijderen
			//$script[$i] = str_replace("<br />","",$script[$i]);
			
			echo $script[$i];
 
		
	}
}

?>
</div>
<div align="center" id="load_settings_div" style="display: none">
	<h4><img src="../media/loading.gif"/> Please wait, loading settings...</h4>
	</div>
	
<a href="nodo_settings.php?read=true" data-role="button" data-ajax="false" data-inline="true" onclick="show_loading();">Read settings</a>
<br>		
		
	
	
	
		
	
	</div><!-- /content -->
	
	<?php require_once('../include/footer_admin.php'); ?>
	
</div><!-- /main page -->

<!-- Start of saved page: -->
<div data-role="dialog" id="saved">

	<div data-role="header">
		<h1><?php echo $page_title?></h1>
	</div><!-- /header -->

	<div data-role="content">	
		<h2>Script send to Nodo.</h2>
				
		<p><a href="status.php" data-role="button" data-inline="true" data-ajax="false" data-icon="back">Ok</a></p>	
	
	
	</div><!-- /content -->
	
	
</div><!-- /page saved -->
 <script type="text/javascript">
 
function show_loading() {

	$('#settings_div').hide();
	$('#load_settings_div').show();
	//IE bug!?
	document.getElementById('load_settings_div').innerHTML = document.getElementById('load_settings_div').innerHTML;
 
}
</script>
</body>
</html>
