<?php 

require_once('../connections/tc.php'); 
require_once('../include/auth.php'); 
require_once('../include/user_settings.php'); 

$page_title="Setup: Edit device";



 // check if the form has been submitted. If it has, process the form and save it to the database if 
 
 if (isset($_POST['submit'])) 
 {  
 // confirm that the 'id' value is a valid integer before getting the form data 
 if (is_numeric($_POST['id'])) 
 { 
 // get form data, making sure it is valid 
 $id = $_POST['id']; 
 $type = mysql_real_escape_string(htmlspecialchars($_POST['type']));
 $naam = mysql_real_escape_string(htmlspecialchars($_POST['naam'])); 
 $label_on = mysql_real_escape_string(htmlspecialchars($_POST['label_on']));
 $label_off = mysql_real_escape_string(htmlspecialchars($_POST['label_off']));
 $collapsed = mysql_real_escape_string(htmlspecialchars($_POST['collapsed']));
 $address = mysql_real_escape_string(htmlspecialchars($_POST['address']));  
 $user_event_on = mysql_real_escape_string(htmlspecialchars($_POST['user_event_on'])); 
 $user_event_off = mysql_real_escape_string(htmlspecialchars($_POST['user_event_off'])); 
 $dim = mysql_real_escape_string(htmlspecialchars($_POST['dim']));
 $homecode = mysql_real_escape_string(htmlspecialchars($_POST['homecode']));
 
  
 // save the data to the database 
 mysql_select_db($database_tc, $tc);
 mysql_query("UPDATE nodo_tbl_devices SET naam='$naam', label_on='$label_on', label_off='$label_off', collapsed='$collapsed',address='$address', user_event_on='$user_event_on', user_event_off='$user_event_off', type='$type', homecode='$homecode', dim='$dim' WHERE id='$id' AND user_id='$userId'") or die(mysql_error());   
 // once saved, redirect back to the view page 
 header("Location: devices.php#saved");  
 } 
 } 
 else {
 mysql_select_db($database_tc, $tc);
 $id = $_GET['id']; $result = mysql_query("SELECT * FROM nodo_tbl_devices WHERE id=$id AND user_id='$userId'") or die(mysql_error());  
 $row = mysql_fetch_array($result);  
 
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

 
<div data-role="page" data-theme="<?php echo $theme?>">
 
	<?php require_once('../include/header_admin.php'); ?>
 
	<div data-role="content">	

	<form action="devices_edit.php" data-ajax="false" method="post"> 
	
	 
		
		<input type="hidden" name="id" id="id" value="<?php echo $row['id'] ;?>"  />
	
	<br>
	
		
			<label for="select-choice-0" class="select">Type of device:</label>
		    <select name="type" id="type" data-native-menu="false">
				<option value="1"<?php if ($row['type'] == 1) {echo 'selected="selected"';}?>>Kaku</option>
				<option value="2" <?php if ($row['type'] == 2) {echo 'selected="selected"';}?>>New kaku</option>
				<option value="3" <?php if ($row['type'] == 3) {echo 'selected="selected"';}?>>WiredOut</option>
				<option value="4" <?php if ($row['type'] == 4) {echo 'selected="selected"';}?>>UserEvent</option>
			</select>
	
	<br>
	
		<label for="name">Device name:</label>
		<input type="text" name="naam" id="naam" value="<?php echo $row['naam'] ;?>"  />
	<br>
		<label for="label_on">Label on:</label>
		<input type="text" name="label_on" id="label_on" value="<?php echo $row['label_on'] ;?>"  />
		<br>
		<label for="label_off">Label off:</label>
		<input type="text" name="label_off" id="naam" value="<?php echo $row['label_off'] ;?>"  />
		
	<br>
	
		<label for="select-choice-1" class="select" >Expand on devices page:</label>
		<select name="collapsed" id="collapsed" data-placeholder="true" data-native-menu="false">
			<option value="0"<?php if ($row['collapsed'] == 0) {echo 'selected="selected"';}?>>No</option>
			<option value="1"<?php if ($row['collapsed'] == 1) {echo 'selected="selected"';}?>>Yes</option>
		</select>
		<br>
		
	<div id="dim_div">
		<label for="select-choice-2" class="select" >Dim option:</label>
		<select name="dim" id="dim" data-placeholder="true" data-native-menu="false">
			<option value="0"<?php if ($row['dim'] == 0) {echo 'selected="selected"';}?>>No</option>
			<option value="1"<?php if ($row['dim'] == 1) {echo 'selected="selected"';}?>>Yes - buttons</option>
			<option value="2"<?php if ($row['dim'] == 2) {echo 'selected="selected"';}?>>Yes - slider</option>
		</select>
		<br>
		</div>
    
	<div id="homecode_div">
		<label for="name">Home code: (A...P)</label>
		<input type="text" maxLength="1" name="homecode" id="homecode" value="<?php echo $row['homecode'] ;?>"  />
    <br>
	</div>
   
    <div id="adres_div">
		<div id="label_adres_newkaku">
		<label for="name">Address: (1...255)</label>
		</div>
		<div id="label_adres_kaku">
		<label for="name">Address: (1...16)</label>
		</div>
		<div id="label_adres_wiredout">
		<label for="name">Port: (1...8)</label>
		</div>
		<input type="text" maxLength="3" name="address" id="address" value="<?php echo $row['address'] ;?>"  />
		<br>
	</div>
	
	<div id="userevent_div">
		<label for="user_event_on" >UserEvent on: (Example: 100,1)</label>
		<input type="text" maxLength="7" name="user_event_on" id="user_event_on" value="<?php echo $row['user_event_on'] ;?>"  />
		<br>
		<label for="user_event_off" >UserEvent off: (Example: 100,2)</label>
		<input type="text" maxLength="7" name="user_event_off" id="user_event_off" value="<?php echo $row['user_event_off'] ;?>"  />
		<br>
	</div>
		
		
	<a href="devices.php" data-role="button" data-ajax="false">Cancel</a>     
	<input type="submit" name="submit" value="Save" >

		
	
	</form> 

		
	</div><!-- /content -->
	
	<?php require_once('../include/footer_admin.php'); ?>
	
</div><!-- /page -->

<script type="text/javascript">	

<?php 

if ($row['type'] == 1) {


	echo "$('#name_div').show();";
	echo "$('#adres_div').show();";
	echo "$('#dim_div').hide();";
	echo "$('#homecode_div').show();";
	echo "$('#userevent_div').hide();";
	echo "$('#submit_div').show();";
	echo "$('#label_adres_kaku').show();";
	echo "$('#label_adres_newkaku').hide();";
	echo "$('#label_adres_wiredout').hide();";


}

 if ($row['type'] == 2) {


	echo "$('#name_div').show();";
	echo "$('#adres_div').show();";
	echo "$('#dim_div').show();";
	echo "$('#homecode_div').hide();";
	echo "$('#userevent_div').hide();";
	echo "$('#submit_div').show();";
	echo "$('#label_adres_kaku').hide();";
	echo "$('#label_adres_newkaku').show();";
	echo "$('#label_adres_wiredout').hide();";


}

if ($row['type'] == 3) {


	echo "$('#name_div').show();";
	echo "$('#adres_div').show();";
	echo "$('#dim_div').hide();";
	echo "$('#homecode_div').hide();";
	echo "$('#userevent_div').hide();";
	echo "$('#submit_div').show();";
	echo "$('#label_adres_kaku').hide();";
	echo "$('#label_adres_newkaku').hide();";
	echo "$('#label_adres_wiredout').show();";


}

if ($row['type'] == 4) {


	echo "$('#name_div').show();";
	echo "$('#adres_div').hide();";
	echo "$('#dim_div').hide();";
	echo "$('#homecode_div').hide();";
	echo "$('#userevent_div').show();";
	echo "$('#label_adres_kaku').hide();";
	echo "$('#label_adres_newkaku').hide();";
	echo "$('#label_adres_wiredout').hide();";
	echo "$('#submit_div').show();";


}



?>





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

if ($(this).attr('value')==4) {   

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
</script> 
 
</body>
</html>

