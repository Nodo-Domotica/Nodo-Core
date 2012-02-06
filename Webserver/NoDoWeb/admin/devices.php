<?php 

require_once('../connections/tc.php'); 
require_once('../include/auth.php');
require_once('../include/settings.php');

$page_title = "Setup: Devices";

// check if the form has been submitted. If it has, process the form and save it to the database if 

if (isset($_POST['submit'])) 
{  
 
  
$naam = mysql_real_escape_string(htmlspecialchars($_POST['naam'])); 
$address = mysql_real_escape_string(htmlspecialchars($_POST['address']));  
$type = mysql_real_escape_string(htmlspecialchars($_POST['type']));
 
  //Als de schakelaar een newkaku schakelaar is dan is homecode 0
 if ($type == 2) {
	$homecode = 0; 
 }
 else {
 	$homecode = mysql_real_escape_string(htmlspecialchars($_POST['homecode']));
 }
 
 $dim = mysql_real_escape_string(htmlspecialchars($_POST['dim']));  
  
mysql_select_db($database_tc, $tc);
mysql_query("INSERT INTO nodo_tbl_devices(naam, type, dim, homecode, address, user_id)
VALUES
('$naam','$type','$dim','$homecode','$address','$userId')") or die(mysql_error());
 header("Location: devices.php#saved");   }
 
else 
{
//Continue
}?>



<!DOCTYPE html> 
<html> 
 
<head>
	<meta charset="utf-8">
	<meta name="viewport" content="width=device-width, initial-scale=1"> 
	<title><?php echo $title;?></title> 
	<?php require_once('../include/jquery_mobile.php'); ?>
</head> 
 
<body> 

 
<div data-role="page" id="main">
 
	<?php require_once('../include/header_admin.php');?>
 
	<div data-role="content">	

<form action="devices.php" data-ajax="false" method="post"> 
		 
	<div data-role="collapsible">
			
		<h3>Add</h3>
			<label for="select-choice-0" class="select" >Type of device:</label>
		    <select name="type" id="type" data-native-menu="false" >
				<option value="0" data-placeholder="true">Select device</option>
				<option value="2">New Kaku</option>
				<option value="1">Kaku</option>
			</select>
	<br>
		
		<div id="name_div"> 		
		<label for="name">Device name:</label>
		<input type="text" name="naam" id="naam" value=""  />
		</div>
	
	<br>
		
		<div id="dim_div">
			<label for="select-choice-1" class="select">Dim option:</label>
			<select name="dim" id="dim" data-native-menu="false">
				<option value="0">No</option>
				<option value="1">Yes - buttons</option>
				<option value="2">Yes - slider</option>
			</select>
			<br>
		</div>
     
		<div id="homecode_div">
		<label for="name" >Home code: (A until P)</label>
		<input type="text" name="homecode" id="homecode" value=""  />
		<br>
		</div>
   
		<div id="adres_div">
		<div id="label_adres_newkaku">
		<label for="name">Address (1 until 255)</label>
		</div>
		<div id="label_adres_kaku">
		<label for="name">Address: (1 until 16)</label>
		</div>
		<input type="text" name="address" id="address" value=""  />
		<br>
		</div>
     
		<div id="submit_div">
		<input type="submit" name="submit" value="Add" >
		</div>
		
</form> 

	</div>

	<div data-role="collapsible" data-collapsed="false">
		<h3>Edit</h3>
		<?php
		// get results from database        
		mysql_select_db($database_tc, $tc);
		$result = mysql_query("SELECT * FROM nodo_tbl_devices WHERE user_id='$userId'") or die(mysql_error());  
				               
		echo '<ul data-role="listview" data-split-icon="delete" data-split-theme="a">';
  
		echo '<br>';   
		// loop through results of database query, displaying them in the table        
		while($row = mysql_fetch_array($result)) 
		{                                
		           
		echo '<li><a href="devices_edit.php?id=' . $row['id'] . '" title=Edit data-ajax="false">'. $row['naam'] .'</a>';                
		echo '<a href="devices_delete_confirm.php?id=' . $row['id'] . '" data-rel="dialog">Delete</a></li>';
		
		}         
		?>
	</div>
	
	</div><!-- /content -->
	
	<?php include('../include/footer_admin.php');?>
	
</div><!-- /main page -->


<!-- Start of saved page: -->
<div data-role="dialog" id="saved">

	<div data-role="header">
		<h1><?php echo $page_title;?></h1>
	</div><!-- /header -->

	<div data-role="content">	
		<h2>Settings saved</h2>
				
		<p><a href="#main" data-role="button" data-inline="true" data-icon="back">Ok</a></p>	
	</div><!-- /content -->
	
</div><!-- /page saved -->

<script>	

$(document).ready(function() {

$('#name_div').hide();   
$('#dim_div').hide();   
$('#homecode_div').hide();
$('#adres_div').hide();
$('#submit_div').hide();

	
	
});


$('#type').change(function() 
{

if ($(this).attr('value')==1) {   

$('#name_div').show();  
$('#adres_div').show();
$('#dim_div').hide();
$('#homecode_div').show();
$('#submit_div').show();

$('#label_adres_kaku').show();
$('#label_adres_newkaku').hide();
}
   //alert('Value change to ' + $(this).attr('value'));
if ($(this).attr('value')==2) {   

$('#name_div').show();  
$('#adres_div').show();
$('#dim_div').show();
$('#homecode_div').hide();
$('#submit_div').show();

$('#label_adres_kaku').hide();
$('#label_adres_newkaku').show();

}
   
});
</script>


</body>
</html>

