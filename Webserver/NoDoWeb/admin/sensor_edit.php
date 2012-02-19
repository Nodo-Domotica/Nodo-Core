<?php 

require_once('../connections/tc.php'); 
require_once('../include/auth.php'); 
require_once('../include/settings.php'); 

$page_title="Setup: Edit sensors";



 // check if the form has been submitted. If it has, process the form and save it to the database if 
 
 if (isset($_POST['submit'])) 
 {  
 // confirm that the 'id' value is a valid integer before getting the form data 
 if (is_numeric($_POST['id'])) 
 { 
 // get form data, making sure it is valid 
 $id = $_POST['id']; 
 $name = mysql_real_escape_string(htmlspecialchars($_POST['name'])); 
 $suffix = mysql_real_escape_string(htmlspecialchars($_POST['suffix']));
 $suffix_true = mysql_real_escape_string(htmlspecialchars($_POST['suffix_true']));
 $suffix_false = mysql_real_escape_string(htmlspecialchars($_POST['suffix_false']));
 $unit = mysql_real_escape_string(htmlspecialchars($_POST['unit'])); 
 $par1 = mysql_real_escape_string(htmlspecialchars($_POST['par1'])); 
 
    
 // save the data to the database 
 mysql_select_db($database_tc, $tc);
 mysql_query("UPDATE NODO_tbl_sensor SET sensor_name='$name', sensor_suffix='$suffix', sensor_suffix_true='$suffix_true', sensor_suffix_false='$suffix_false', nodo_unit_nr='$unit', par1='$par1' WHERE id='$id' AND user_id='$userId'") or die(mysql_error());   
 // once saved, redirect back to the view page 
 header("Location: sensor.php#saved");  
 } 
 } 
 else {
 mysql_select_db($database_tc, $tc);
 $id = $_GET['id']; $result = mysql_query("SELECT * FROM nodo_tbl_sensor WHERE id=$id AND user_id='$userId'") or die(mysql_error());  
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

 
<div data-role="page">
 
	<?php require_once('../include/header_admin.php'); ?>
 
	<div data-role="content">	

<form action="sensor_edit.php" data-ajax="false" method="post"> 
	
	 <input type="hidden" name="id" id="id" value="<?php echo $row['id'] ;?>"  />
				
	<br>
	
	
	<div id="label_value_div">
		<label for="name">Name: (Example: Temperature outside:)</label>
		</div>
		<div id="label_state_div">
		<label for="name">Name: (Example: Door:)</label>
		</div>
		<input type="text" name="name" id="name" value="<?php echo $row['sensor_name'] ;?>"  />
		<br>
	
	<div id="value_div">
		<label for="suffix">Suffix: (Example: &deg;C, M&sup3;)</label>
		<input type="text" name="suffix" id="suffix" value="<?php echo $row['sensor_suffix'] ;?>"  />
		<br>
		</div>
		
		<div id="state_div">
		<label for="suffix_false">Suffix: >0 (Example: Open)</label>
		<input type="text" name="suffix_true" id="suffix_true" value="<?php echo $row['sensor_suffix_true'] ;?>"  />
		<br>
		<label for="suffix_true">Suffix: <=0 (Example: Closed)</label>
		<input type="text" name="suffix_false" id="suffix_false" value="<?php echo $row['sensor_suffix_false'] ;?>"  />
		<br>
		</div>
		
			
		
		
		<br>
		<label for="select-choice-1" class="select" >NoDo unit:</label>
		    <select name="unit" id="unit" data-native-menu="false" >
				<option value="0" data-placeholder="true">Select unit nr:....</option>
				<option value="1"<?php if ($row['nodo_unit_nr'] == 1) {echo 'selected="selected"';}?>>1</option>
				<option value="2"<?php if ($row['nodo_unit_nr'] == 2) {echo 'selected="selected"';}?>>2</option>
				<option value="3"<?php if ($row['nodo_unit_nr'] == 3) {echo 'selected="selected"';}?>>3</option>
				<option value="4"<?php if ($row['nodo_unit_nr'] == 4) {echo 'selected="selected"';}?>>4</option>
				<option value="5"<?php if ($row['nodo_unit_nr'] == 5) {echo 'selected="selected"';}?>>5</option>
				<option value="6"<?php if ($row['nodo_unit_nr'] == 6) {echo 'selected="selected"';}?>>6</option>
				<option value="7"<?php if ($row['nodo_unit_nr'] == 7) {echo 'selected="selected"';}?>>7</option>
				<option value="8"<?php if ($row['nodo_unit_nr'] == 8) {echo 'selected="selected"';}?>>8</option>
				<option value="9"<?php if ($row['nodo_unit_nr'] == 9) {echo 'selected="selected"';}?>>9</option>
				<option value="10"<?php if ($row['nodo_unit_nr'] == 10) {echo 'selected="selected"';}?>>10</option>
				<option value="11"<?php if ($row['nodo_unit_nr'] == 11) {echo 'selected="selected"';}?>>11</option>
				<option value="12"<?php if ($row['nodo_unit_nr'] == 12) {echo 'selected="selected"';}?>>12</option>
				<option value="13"<?php if ($row['nodo_unit_nr'] == 13) {echo 'selected="selected"';}?>>13</option>
				<option value="14"<?php if ($row['nodo_unit_nr'] == 14) {echo 'selected="selected"';}?>>14</option>
				<option value="15"<?php if ($row['nodo_unit_nr'] == 15) {echo 'selected="selected"';}?>>15</option>
			</select>
		<br>
		<div id="label_wiredanalog_div">
		<label for="name">WiredAnalog port (1 until 15)</label>
		</div>
		<div id="label_variable_div">
		<label for="name">Variable (1 until 15)</label>
		</div>
		<input type="text" name="par1" id="par1" value="<?php echo $row['par1'];?>"  />
		<br>
	    <br>
        
		<input type="submit" name="submit" value="Edit" >

		
	
	</form> 
			
				
	</div><!-- /content -->
	
	<?php require_once('../include/footer_admin.php'); ?>
	
</div><!-- /page -->

<script type="text/javascript">	

<?php 

if ($row['sensor_type'] == 1) {


	echo "$('#state_div').hide();";
	echo "$('#label_state_div').hide();";
	//echo "$('#value_div').show(); ";
	//echo "$('#label_wiredanalog_div').show();";
	echo "$('#label_variable_div').hide();";
 
 




}

 if ($row['sensor_type'] == 2) {


	echo "$('#state_div').show();";
	echo "$('#label_state_div').show();";
	echo "$('#value_div').hide(); ";
	echo "$('#label_wiredanalog_div').hide();";
	echo "$('#label_variable_div').show();";
	echo "$('#label_value_div').hide();";


} ?>
</script> 
 
</body>
</html>

