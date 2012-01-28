<?php require_once('../connections/tc.php'); 
require_once('../include/auth.php'); 
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
 mysql_query("UPDATE NODO_tbl_switch SET naam='$naam', address='$address', type='$type', homecode='$homecode', dim='$dim' WHERE id='$id' AND user_id='$userId'") or die(mysql_error());   
 // once saved, redirect back to the view page 
 header("Location: switch.php");  
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
	<title>Schakelaar aanpassen</title> 
	<link rel="stylesheet" href="http://code.jquery.com/mobile/1.0rc2/jquery.mobile-1.0rc2.min.css" />
	<script src="http://code.jquery.com/jquery-1.6.4.min.js"></script>
	<script src="http://code.jquery.com/mobile/1.0rc2/jquery.mobile-1.0rc2.min.js"></script>
</head> 
 
<body> 

 
<div data-role="page">
 
	<div data-role="header">
		<h1>Schakelaar aanpassen</h1>
		<a href="/admin/index.php" data-icon="gear" class="ui-btn-right" data-ajax="false">Setup</a>
		<a href="/index.php" data-icon="home" class="ui-btn-left" data-ajax="false" data-iconpos="notext">Home</a>
	</div><!-- /header -->
 
	<div data-role="content">	

	<form action="switch_edit.php" data-ajax="false" method="post"> 
	
	 
		
		<input type="hidden" name="id" id="id" value="<?php echo $row['id'] ;?>"  />
	
	<br>
	
		
			<label for="select-choice-0" class="select">Type schakelaar:</label>
		    <select name="type" id="type" data-native-menu="false">
				<option value="1"<?php if ($row['type'] == 1) {echo 'selected="selected"';}?>>Kaku</option>
				<option value="2" <?php if ($row['type'] == 2) {echo 'selected="selected"';}?>>New kaku</option>
			</select>
	
	<br>
	
		<label for="name">Schakelaar naam:</label>
		<input type="text" name="naam" id="naam" value="<?php echo $row['naam'] ;?>"  />
	
	<br>
	
		<label for="select-choice-1" class="select" >Dim mogelijkheid:</label>
		<select name="dim" id="dim" data-placeholder="true" data-native-menu="false">
			<option value="0"<?php if ($row['dim'] == 0) {echo 'selected="selected"';}?>>Nee</option>
			<option value="1"<?php if ($row['dim'] == 1) {echo 'selected="selected"';}?>>Ja - Knoppen</option>
			<option value="2"<?php if ($row['dim'] == 2) {echo 'selected="selected"';}?>>Ja - Slider</option>
		</select>
   
   <br>
    
		<label for="name">Home code: (A t/m P)</label>
		<input type="text" name="homecode" id="homecode" value="<?php echo $row['homecode'] ;?>"  />
   
   <br>
   
		<label for="name">Address:</label>
		<input type="text" name="address" id="address" value="<?php echo $row['address'] ;?>"  />
	
	<br>
     
		<input type="submit" name="submit" value="Schakelaar aanpassen" >

		
	
	</form> 

		
	</div><!-- /content -->
	
	<div data-role="footer">
		<h4></h4>
	</div><!-- /footer -->
	
</div><!-- /page -->
 
</body>
</html>

