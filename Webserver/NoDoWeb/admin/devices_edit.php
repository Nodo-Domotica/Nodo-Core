<?php 

require_once('../connections/tc.php'); 
require_once('../include/auth.php'); 
require_once('../include/settings.php'); 

$page_title="Setup: Edit device";



 // check if the form has been submitted. If it has, process the form and save it to the database if 
 
 if (isset($_POST['submit'])) 
 {  
 // confirm that the 'id' value is a valid integer before getting the form data 
 if (is_numeric($_POST['id'])) 
 { 
 // get form data, making sure it is valid 
 $id = $_POST['id']; 
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
 
  
 // save the data to the database 
 mysql_select_db($database_tc, $tc);
 mysql_query("UPDATE NODO_tbl_devices SET naam='$naam', address='$address', type='$type', homecode='$homecode', dim='$dim' WHERE id='$id' AND user_id='$userId'") or die(mysql_error());   
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

 
<div data-role="page">
 
	<?php require_once('../include/header_admin.php'); ?>
 
	<div data-role="content">	

	<form action="devices_edit.php" data-ajax="false" method="post"> 
	
	 
		
		<input type="hidden" name="id" id="id" value="<?php echo $row['id'] ;?>"  />
	
	<br>
	
		
			<label for="select-choice-0" class="select">Type of device:</label>
		    <select name="type" id="type" data-native-menu="false">
				<option value="1"<?php if ($row['type'] == 1) {echo 'selected="selected"';}?>>Kaku</option>
				<option value="2" <?php if ($row['type'] == 2) {echo 'selected="selected"';}?>>New kaku</option>
			</select>
	
	<br>
	
		<label for="name">Device name:</label>
		<input type="text" name="naam" id="naam" value="<?php echo $row['naam'] ;?>"  />
	
	<br>
	<div id="dim_div">
		<label for="select-choice-1" class="select" >Dim option:</label>
		<select name="dim" id="dim" data-placeholder="true" data-native-menu="false">
			<option value="0"<?php if ($row['dim'] == 0) {echo 'selected="selected"';}?>>No</option>
			<option value="1"<?php if ($row['dim'] == 1) {echo 'selected="selected"';}?>>Yes - buttons</option>
			<option value="2"<?php if ($row['dim'] == 2) {echo 'selected="selected"';}?>>Yes - slider</option>
		</select>
		<br>
		</div>
    
	<div id="homecode_div">
		<label for="name">Home code: (A until P)</label>
		<input type="text" name="homecode" id="homecode" value="<?php echo $row['homecode'] ;?>"  />
    <br>
	</div>
   
    <div id="adres_div">
		<div id="label_adres_newkaku">
		<label for="name">Address: (1 until 255)</label>
		</div>
		<div id="label_adres_kaku">
		<label for="name">Address: (1 until 16)</label>
		</div>
		<input type="text" name="address" id="address" value="<?php echo $row['address'] ;?>"  />
		<br>
	</div>
		
		
		    
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

