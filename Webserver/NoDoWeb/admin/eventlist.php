<?php 

require_once('../connections/tc.php'); 
require_once('../include/auth.php');
require_once('../include/settings.php');

$page_title = "Setup: Eventlist";	  


//Eventlist opvragen
if (isset($_POST['Read'])) 
{  

	//Eventlist door nodo laten wegschrijven
	$Url = "http://$nodo_ip/?id=$nodo_id&password=$nodo_password&event=EventListfile%20events";
 
    
    if (!function_exists('curl_init')){
        die('Sorry cURL is not installed!');
    }
 
    $ch = curl_init();
    curl_setopt($ch, CURLOPT_URL, $Url);
    curl_setopt($ch, CURLOPT_USERAGENT, "NoDoWeb");
    curl_setopt($ch, CURLOPT_HEADER, 0);
	curl_setopt($ch, CURLOPT_PORT, $nodo_port );
    curl_setopt($ch, CURLOPT_RETURNTRANSFER, true);
    curl_setopt($ch, CURLOPT_TIMEOUT, 10);
 
    curl_exec($ch);
    curl_close($ch);


	//Eventlist bij nodo opvragen
	$Url = "http://$nodo_ip/?id=$nodo_id&password=$nodo_password&file=events";
 
    
    if (!function_exists('curl_init')){
        die('Sorry cURL is not installed!');
    }
 
    
    $ch = curl_init();
    curl_setopt($ch, CURLOPT_URL, $Url);
    curl_setopt($ch, CURLOPT_USERAGENT, "NoDoWeb");
    curl_setopt($ch, CURLOPT_HEADER, 0);
	curl_setopt($ch, CURLOPT_PORT, $nodo_port );
    curl_setopt($ch, CURLOPT_RETURNTRANSFER, true);
    curl_setopt($ch, CURLOPT_TIMEOUT, 10);
 
   	//create array
	$eventlist = explode("\n", curl_exec($ch));
	
	
	//count total lines in $eventlist
	$total_events = count($eventlist);

	
    curl_close($ch);
 
 
}


// check if the form has been submitted. If it has, process the form and save it to the database if 

if (isset($_POST['submit2'])) 
{  
 
 // get form data, making sure it is valid 
 $name = mysql_real_escape_string(htmlspecialchars($_POST['name'])); 
 $events = mysql_real_escape_string(htmlspecialchars($_POST['events']));  
 
 
 
  
 // save the data to the database 
 mysql_select_db($database_tc, $tc);
 
   
 mysql_query("INSERT INTO nodo_tbl_activities (name, events, user_id) 
 VALUES 
 ('$name','$events','$userId')");
 // once saved, redirect back to the view page 
 header("Location: activities.php#saved");    }
 
else 
{
mysql_select_db($database_tc, $tc);
$result = mysql_query("SELECT * FROM nodo_tbl_eventlist WHERE user_id='$userId'") or die(mysql_error());  
}?>




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

	<h1>UITWERKEN!</h1>
	
	<form action="eventlist.php" data-ajax="false" method="post"> 
	
	 
				
	<br>
	
		<label for="eventlist">Eventlist:</label>
		<textarea name="textarea" id="eventlist">
<?php 

if (isset($eventlist)){  

	for($i=0;$i<$total_events;$i++){
		//Alleen als de regel met EventList begint behandelen we de regel.
		if(strstr($eventlist[$i],"EventList ")){ 
			
			//<br /> aan het einde van de regels verwijderen
			$eventlist[$i] = str_replace("<br />","",$eventlist[$i]);
			//Eventlist [nr]; aan het begin van de regels verwijderen
			$eventlist[$i] = substr($eventlist[$i],strpos($eventlist[$i],";")+2);
			echo $eventlist[$i];
 
		}
	}
}

?>
		</textarea>

		<br>
        
		<input type="submit" name="Write" value="Write eventlist" >
		<input type="submit" name="Read" value="Read eventlist" >

	
	</form> 
	</div>
	
	
		
	
	</div><!-- /content -->
	
	<?php require_once('../include/footer_admin.php'); ?>
	
</div><!-- /main page -->

<!-- Start of saved page: -->
<div data-role="dialog" id="saved">

	<div data-role="header">
		<h1><?php echo $page_title?></h1>
	</div><!-- /header -->

	<div data-role="content">	
		<h2>Settings saved.</h2>
				
		<p><a href="activities.php" data-role="button" data-inline="true" data-icon="back">Ok</a></p>	
	
	
	</div><!-- /content -->
	
	
</div><!-- /page saved -->
 
</body>
</html>

