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
 $unit = mysql_real_escape_string(htmlspecialchars($_POST['unit'])); 
 $port = mysql_real_escape_string(htmlspecialchars($_POST['port'])); 
 
    
 // save the data to the database 
 mysql_select_db($database_tc, $tc);
 mysql_query("UPDATE NODO_tbl_sensor SET sensor_name='$name', sensor_suffix='$suffix', nodo_unit_nr='$unit', port='$port' WHERE id='$id' AND user_id='$userId'") or die(mysql_error());   
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
	
		<label for="name">Name: (example: Temperature outside:)</label>
		<input type="text" name="name" id="name" value="<?php echo $row['sensor_name'] ;?>"  />
		<br>
		<label for="select-choice-0" class="select" >Suffix:</label>
		    <select name="suffix" id="suffix" data-native-menu="false" >
				<option value="0" data-placeholder="true">Select suffix:....</option>
				<option value="%"<?php if ($row['sensor_suffix'] == "%") {echo 'selected="selected"';}?>>%</option>
				<option value="&deg;C"<?php if ($row['sensor_suffix'] == "°C") {echo 'selected="selected"';}?>>&deg;C</option>
				<option value=" "<?php if ($row['sensor_suffix'] == " ") {echo 'selected="selected"';}?>>None</option>
			</select>
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
		<label for="select-choice-2" class="select" >Wiredanalog port:</label>
		    <select name="port" id="port" data-native-menu="false" >
				<option value="0" data-placeholder="true">Select port:....</option>
				<option value="1"<?php if ($row['port'] == 1) {echo 'selected="selected"';}?>>1</option>
				<option value="2"<?php if ($row['port'] == 2) {echo 'selected="selected"';}?>>2</option>
				<option value="3"<?php if ($row['port'] == 3) {echo 'selected="selected"';}?>>3</option>
				<option value="4"<?php if ($row['port'] == 4) {echo 'selected="selected"';}?>>4</option>
				<option value="5"<?php if ($row['port'] == 5) {echo 'selected="selected"';}?>>5</option>
				<option value="6"<?php if ($row['port'] == 6) {echo 'selected="selected"';}?>>6</option>
				<option value="7"<?php if ($row['port'] == 7) {echo 'selected="selected"';}?>>7</option>
				<option value="8"<?php if ($row['port'] == 8) {echo 'selected="selected"';}?>>8</option>
			</select>
		<br>
	    <br>
        
		<input type="submit" name="submit" value="Edit" >

		
	
	</form> 
			
				
	</div><!-- /content -->
	
	<?php require_once('../include/footer_admin.php'); ?>
	
</div><!-- /page -->

<script>	

<?php if ($row['type'] == 1) {

echo "$('#label_adres_newkaku').hide();"; }

else {

echo "$('#label_adres_kaku').hide();"; }

?>




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

