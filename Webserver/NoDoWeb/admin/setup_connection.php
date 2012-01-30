<?php 

require_once('../connections/tc.php'); 
require_once('../include/auth.php');
require_once('../include/settings.php');

$page_title = "Setup: Communicatie";	
// check if the form has been submitted. If it has, process the form and save it to the database if 

if (isset($_POST['submit'])) 
{  
 
 // get form data, making sure it is valid 
 $nodo_ip = mysql_real_escape_string(htmlspecialchars($_POST['nodo_ip'])); 
 $nodo_port = mysql_real_escape_string(htmlspecialchars($_POST['nodo_port']));  
 $send_method = mysql_real_escape_string(htmlspecialchars($_POST['send_method']));
 $nodo_password = mysql_real_escape_string(htmlspecialchars($_POST['nodo_password']));
 
 
if (ISSET($_POST["checkbox-1"])) {
   
   
   
// Lengte van de unieke waarde
$unique_ref_length = 8;


$unique_ref_found = false;

//Welke karakters mogen in de unieke waarde voorkomen
$possible_chars = "1234567890ABCDEFGHIJKLMNPQRSTUVW";

//Tot we een unieke waarde vinden gaan we verder
while (!$unique_ref_found) {

	
	$unique_ref = "";

	
	$i = 0;

	
	while ($i < $unique_ref_length) {

		//Random karakter uit $possible_chars halen
		$char = substr($possible_chars, mt_rand(0, strlen($possible_chars)-1), 1);

		$unique_ref .= $char;

		$i++;

	}

	
	//Waarde gegenereerd. Checken of hij al in de database voorkomt
	mysql_select_db($database_tc, $tc);
	$result = mysql_query("SELECT 'nodo_id' FROM NODO_tbl_users WHERE nodo_id='$unique_ref'") or die(mysql_error());  
	$row = mysql_fetch_array($result);
	
	if (mysql_num_rows($result)==0) {

		//We hebben een unieke waarde gevonden
		$unique_ref_found = true;

	}
	
	
}

$nodo_id = $unique_ref;
 }  
   
   else
   
   { 
   
  $nodo_id = mysql_real_escape_string(htmlspecialchars($_POST['nodo_id'])); 
  
  }
   
   

  
 // save the data to the database 
 mysql_select_db($database_tc, $tc);
 mysql_query("UPDATE NODO_tbl_users SET nodo_ip='$nodo_ip', nodo_port='$nodo_port', send_method='$send_method', nodo_password='$nodo_password', nodo_id='$nodo_id' WHERE id='$userId'") or die(mysql_error());   
 // once saved, redirect back to the view page 
 header("Location: setup_connection.php#saved");   
 }
 
else 
{
mysql_select_db($database_tc, $tc);
$result = mysql_query("SELECT * FROM NODO_tbl_users WHERE id='$userId'") or die(mysql_error());  
$row = mysql_fetch_array($result);
}?>




<!DOCTYPE html> 
<html> 
 
<head>
	<meta charset="utf-8">
	<meta name="viewport" content="width=device-width, initial-scale=1"> 
	<title><?php echo $title; ?></title> 
	<?php require_once('../include/jquery_mobile.php'); ?>
</head> 
 
<body> 

<!-- Start of main page: -->

<div data-role="page" pageid="main" >
 
	<?php require_once('../include/header_admin.php'); ?>
 
	<div data-role="content">	

	<form action="setup_connection.php" data-ajax="false" method="post"> 
	
	 
				<label for="sendmethod" class="select">Nodo opdrachten verzenden via:</label>
		    <select name="send_method" id="send_method" data-placeholder="true" data-native-menu="false">
				<option value="1" <?php if ($row['send_method'] == 1) {echo 'selected="selected"';}?>>Network Event client (APOP)</option>
				<option value="2" <?php if ($row['send_method'] == 2 || $row['send_method'] == 0 ) {echo 'selected="selected"';}?>>HTTP</option>
			</select>
	
	<br>
	
		<label for="name">NoDo ip/host: (x.x.x.x)</label>
		<input type="text" name="nodo_ip" id="nodo_ip" value="<?php echo $row['nodo_ip']?>"  />
	
	<br>   
      
		<label for="name">TCP poort:</label>
		<input type="text" name="nodo_port" id="nodo_port" value="<?php echo $row['nodo_port']?>"  />

			
	<br>
      
		<label for="name">Nodo wachtwoord:</label>
		<input type="password" name="nodo_password" id="nodo_password" value="<?php echo $row['nodo_password']?>"  />
		
	<br>
	   	   	
		<input type="checkbox" name="checkbox-1" id="checkbox-0" class="custom" />
		<label for="checkbox-0">Genereer een (nieuw) Nodo ID</label>
	 
	 <br>  
      
		<label for="name">Nodo ID: <b><?php echo $row['nodo_id']?></b></label>
		<input type="hidden" name="nodo_id" id="nodo_id" value="<?php echo $row['nodo_id']?>"  />
    
	<br><br>
        
		<input type="submit" name="submit" value="Opslaan" >

		
	
	</form> 

	

	
	</div><!-- /content -->
	
	<?php include('../include/footer_admin.php'); ?>
	
</div><!-- /main page -->

<!-- Start of saved dialog: -->
<div data-role="dialog" id="saved">

	<div data-role="header">
		<h1><?php echo $page_title;?></h1>
	</div><!-- /header -->

	<div data-role="content">	
		<h2> De wijzigingen zijn opgeslagen.</h2>
				
		<p><a href="#main" data-rel="back" data-role="button" data-inline="true" data-icon="back">Ok</a></p>	
	</div><!-- /content -->
	
	
</div><!-- /dialog saved -->
 
<script>	 
 

$('#send_method').change(function() 
{

if ($(this).attr('value')==1) {   

$('#nodo_port').val("1024"); 
//$('#nodo_port').textinput('refresh'); 
//$("#send_method").selectmenu("refresh");
}
   //alert('Value change to ' + $(this).attr('value'));
if ($(this).attr('value')==2) {   

$('#nodo_port').val("80"); 
//$('#nodo_port').textinput('refresh'); 

}
   
});
</script> 
 
</body>
</html>

