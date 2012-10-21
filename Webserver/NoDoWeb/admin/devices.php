<?php 
/***********************************************************************************************************************
"Nodo Web App" Copyright © 2012 Martin de Graaf

This program is free software: you can redistribute it and/or modify
it under the terms of the GNU Affero General Public License as
published by the Free Software Foundation, either version 3 of the
License, or (at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU Affero General Public License for more details.

You should have received a copy of the GNU Affero General Public License
along with this program.  If not, see <http://www.gnu.org/licenses/>.
*************************************************************************************************************************/


require_once('../connections/db_connection.php'); 
require_once('../include/auth.php');
require_once('../include/user_settings.php');

$page_title = "Setup: Devices";

// check if the form has been submitted. If it has, process the form and save it to the database if 

if (isset($_POST['submit'])) 
{  
 
  
$type = mysql_real_escape_string(htmlspecialchars($_POST['type']));
$naam = mysql_real_escape_string(htmlspecialchars($_POST['naam'])); 
$label_on = mysql_real_escape_string(htmlspecialchars($_POST['label_on']));
$label_off = mysql_real_escape_string(htmlspecialchars($_POST['label_off']));
$address = mysql_real_escape_string(htmlspecialchars($_POST['address']));  
$user_event_on = mysql_real_escape_string(htmlspecialchars($_POST['user_event_on'])); 
$user_event_off = mysql_real_escape_string(htmlspecialchars($_POST['user_event_off'])); 
$dim = mysql_real_escape_string(htmlspecialchars($_POST['dim'])); 
$homecode = mysql_real_escape_string(htmlspecialchars($_POST['homecode']));
$toggle = mysql_real_escape_string(htmlspecialchars($_POST['presentation']));




mysql_select_db($database, $db);

//Totaal aantal records bepalen 
$RSDevices = mysql_query("SELECT id FROM nodo_tbl_devices WHERE user_id='$userId'") or die(mysql_error()); 
$RSDevices_rows = mysql_num_rows($RSDevices);
//aantal records met 1 verhogen zodat we deze waarde in het sorteerveld kunnen gebruiken
$sort_order = $RSDevices_rows + 1; 

mysql_query("INSERT INTO nodo_tbl_devices(naam, label_on, label_off, type, toggle, dim, homecode, address, user_event_on, user_event_off, user_id,sort_order)
VALUES
('$naam','$label_on','$label_off','$type','$toggle','$dim','$homecode','$address','$user_event_on','$user_event_off','$userId','$sort_order')") or die(mysql_error());
 header("Location: devices.php#saved");   
 
 }
 
 
 //Records sorteren
 if (isset($_GET['sort'])) {
	
	$device_id = $_GET['id'];
	$sort = $_GET['sort'];
	$sort_order = $_GET['sort_order'];
	$prev_record = $_GET['sort_order'] - 1;
	$next_record = $_GET['sort_order'] + 1;
	
	if ($sort == "up") {
	

	 mysql_query("UPDATE nodo_tbl_devices SET sort_order=sort_order +1 WHERE user_id='$userId' AND sort_order='$prev_record'") or die(mysql_error()); 	
	 mysql_query("UPDATE nodo_tbl_devices SET sort_order=sort_order -1 WHERE user_id='$userId' AND sort_order='$sort_order' AND id='$device_id'") or die(mysql_error()); 
	
	}
	if ($sort == "down") {

	 mysql_query("UPDATE nodo_tbl_devices SET sort_order=sort_order -1 WHERE user_id='$userId' AND sort_order='$next_record'") or die(mysql_error()); 	
	 mysql_query("UPDATE nodo_tbl_devices SET sort_order=sort_order +1 WHERE user_id='$userId' AND sort_order='$sort_order' AND id='$device_id'") or die(mysql_error()); 
	
	}
header("Location: devices.php?id=$device_id"); 
}

 ?>



<!DOCTYPE html> 
<html> 
 
<head>
	<meta charset="utf-8">
	<meta name="viewport" content="width=device-width, initial-scale=1"> 
	<title><?php echo $title;?></title> 
	<?php require_once('../include/jquery_mobile.php'); ?>
</head> 
 
<body> 

 
<div data-role="page" id="admin_devices_page" data-theme="<?php echo $theme?>">
 
	<?php require_once('../include/header_admin.php');?>
 
	<div data-role="content">	

<form action="devices.php" data-ajax="false" method="post"> 
		 
	<div data-role="collapsible" data-inset="false" data-collapsed-icon="plus" data-expanded-icon="minus" data-iconpos="right" data-content-theme="<?php echo $theme?>">
			
		<h3>Add device:</h3>
			<label for="select-choice-0" class="select" >Type of device:</label>
		    <select name="type" id="type" data-native-menu="false" >
				<option value="0" data-placeholder="true">Select device</option>
				<option value="1">Kaku</option>
				<option value="2">New Kaku</option>
				<option value="3">WiredOut</option>
				<option value="4">SendUserEvent</option>
				<option value="5">UserEvent</option>
			</select>
		<br \>
		
		<div id="name_div"> 		
		<label for="naam">Device name:</label>
		<input type="text" name="naam" id="naam" value=""  />
		<br \>
		<label for="select-choice-1" class="select" >Switch type:</label>
		    <select name="presentation" id="presentation" data-native-menu="false" >
				<option value="1">Toggle</option>
				<option value="0">Popup</option>
			</select>
		<br \>
		<div id="label_div"> 
		<label for="label_on">Label on button:</label>
		<input type="text" name="label_on" id="label_on" value="On"  />
		<br \>
		<label for="label_off">Label off button:</label>
		<input type="text" name="label_off" id="naam" value="Off"  />
		<br \>
		</div>
		
		
		
		</div>
	
		
		<div id="dim_div">
			<label for="select-choice-3" class="select">Dim option:</label>
			<select name="dim" id="dim" data-native-menu="false">
				<option value="0">No</option>
				<option value="1">Yes - buttons</option>
				<option value="2">Yes - slider</option>
			</select>
			<br \>
		</div>
     
		<div id="homecode_div">
		<label for="homecode" >Home code: (A...P)</label>
		<input type="text" maxLength="1" name="homecode" id="homecode" value=""  />
		<br \>
		</div>
   
		<div id="adres_div">
		<div id="label_adres_newkaku">
		<label for="address_newkaku">Address (1...255)</label>
		</div>
		<div id="label_adres_kaku">
		<label for="address_kaku">Address: (1...16)</label>
		</div>
		<div id="label_adres_wiredout">
		<label for="wiredout">Port: (1...8)</label>
		</div>
		<input type="text" maxLength="3" name="address" id="address" value=""  />
		<br \>
		</div>
		
		<div id="userevent_div">
		<label for="user_event_on" >UserEvent on: (Example: 100,2)</label>
		<input type="text" maxLength="7" name="user_event_on" id="user_event_on" value=""  />
		<br \>
		<label for="user_event_off" >UserEvent off: (Example: 100,1)</label>
		<input type="text" maxLength="7" name="user_event_off" id="user_event_off" value=""  />
		<br \>
		</div>
     
		<div id="submit_div">
		<input type="submit" name="submit" id="submit" value="Save" >
		</div>
		
</form> 

	</div>

	<div data-role="collapsible" data-inset="false" data-collapsed="false" data-iconpos="right" >
		<h3>Edit devices:</h3>
		<?php
		// get results from database        
		mysql_select_db($database, $db);
		$result = mysql_query("SELECT * FROM nodo_tbl_devices WHERE user_id='$userId' ORDER BY sort_order ASC") or die(mysql_error());  
		$rows = mysql_num_rows($result);
				               
		
		
		// loop through results of database query, displaying them in the table        
		
		$i=0;
		
		while($row = mysql_fetch_array($result)) 
		{                                
		 
		$i++;
		?>          
		<div data-role="collapsible" data-collapsed="<?php if ($_GET['id'] == $row['id']) {echo "false";} else {echo "true";} ?>" data-iconpos="right" data-content-theme="<?php echo $theme;?>">
		<h3><?php echo $row['naam']; ?></h3>
		<?php if ($i > 1) { ?>
		<a href="devices.php?sort=up&sort_order=<?php echo $row['sort_order']; ?>&id=<?php echo $row['id']; ?>" data-role="button"  data-icon="arrow-u" data-ajax="false">Move up</a>
		<?php } ?>
		<?php if ($i != $rows) { ?>
		<a href="devices.php?sort=down&sort_order=<?php echo $row['sort_order']; ?>&id=<?php echo $row['id']; ?>" data-role="button" data-icon="arrow-d"  data-ajax="false">Move down</a>
		<?php } ?>
		<a href="devices_edit.php?id=<?php echo $row['id']; ?>" data-role="button" data-icon="gear" data-ajax="false">Edit</a>
		<a href="devices_delete_confirm.php?id=<?php echo $row['id']; ?>" data-role="button"  data-icon="delete" data-rel="dialog">Delete</a>
		</div>
	
		<?php
		}         
		?>
				
	</div>
	
	</div><!-- /content -->
	
	<?php include('../include/footer_admin.php');?>
	
</div><!-- /main page -->


<!-- Start of saved page: -->
<div data-role="dialog" id="saved" data-theme="<?php echo $theme?>">

	<div data-role="header" data-theme="<?php echo $theme_header?>">
		<h1><?php echo $page_title;?></h1>
	</div><!-- /header -->

	<div data-role="content">	
		<h2>Device saved</h2>
				
		<p><a href="#main" data-role="button" data-inline="true" data-icon="back">Ok</a></p>	
	</div><!-- /content -->
	
</div><!-- /page saved -->

<script>	
$(document).ready(function() {
      $('#submit').click(function() {
		  $(".error").hide();
		  var hasError = false;
		 
		  
		 var nameVal = $("#naam").val();
		 if(nameVal == '') {
		 $("#naam").before('<span class="error"><b>Please enter a device name<b></span>');
		 hasError = true;
		 }
		  
		 
		 if(hasError == true) { return false; }
	 });
	 
	 
		  });
</script>

<script type="text/javascript">		

$(document).ready(function() {

$('#name_div').hide();   
$('#dim_div').hide();   
$('#homecode_div').hide();
$('#adres_div').hide();
$('#userevent_div').hide();
$('#submit_div').hide();
$('#label_div').hide();


	
	
});


$('#type').change(function() 
{

if ($(this).attr('value')==1) {   

$('#name_div').show();  
$('#adres_div').show();
$('#dim_div').hide();
$('#homecode_div').show();
$('#userevent_div').hide();
$('#submit_div').show();

$('#label_adres_kaku').show();
$('#label_adres_wiredout').hide();
$('#label_adres_newkaku').hide();
}
   //alert('Value change to ' + $(this).attr('value'));
if ($(this).attr('value')==2) {   

$('#name_div').show();  
$('#adres_div').show();
$('#dim_div').show();
$('#homecode_div').hide();
$('#userevent_div').hide();
$('#submit_div').show();

$('#label_adres_kaku').hide();
$('#label_adres_wiredout').hide();
$('#label_adres_newkaku').show();

}

if ($(this).attr('value')==3) {   

$('#name_div').show();  
$('#adres_div').show();
$('#dim_div').hide();
$('#homecode_div').hide();
$('#userevent_div').hide();
$('#submit_div').show();

$('#label_adres_kaku').hide();
$('#label_adres_wiredout').show();
$('#label_adres_newkaku').hide();

}

if ($(this).attr('value')==4 || $(this).attr('value')==5) {   

$('#name_div').show();  
$('#adres_div').hide();
$('#dim_div').hide();
$('#homecode_div').hide();
$('#label_adres_kaku').hide();
$('#label_adres_wiredout').hide();
$('#label_adres_newkaku').hide();
$('#userevent_div').show();


$('#submit_div').show();

} 
 
});

$('#presentation').change(function() 
{

if ($(this).attr('value')==0) {   

$('#label_div').show();


}

if ($(this).attr('value')==1) {   

$('#label_div').hide();
 

}
 
 
});
</script>


</body>
</html>

