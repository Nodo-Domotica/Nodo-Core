<?php require_once('../Connections/tc.php'); 
require_once('../Include/auth.php'); 
// check if the form has been submitted. If it has, process the form and save it to the database if 

if (isset($_POST['submit'])) 
{  
 
 // get form data, making sure it is valid 
 $theme = mysql_real_escape_string(htmlspecialchars($_POST['theme'])); 
 $theme_header = mysql_real_escape_string(htmlspecialchars($_POST['theme_header']));  
 $title = mysql_real_escape_string(htmlspecialchars($_POST['title'])); 
 
 
  
 // save the data to the database 
 mysql_select_db($database_tc, $tc);
 mysql_query("UPDATE NODO_tbl_setup SET theme='$theme', theme_header='$theme_header', title='$title' WHERE user_id='$userId'") or die(mysql_error());   
 // once saved, redirect back to the view page 
 header("Location: setup_nodoweb.php");   }
 
else 
{
mysql_select_db($database_tc, $tc);
$result = mysql_query("SELECT * FROM NODO_tbl_setup WHERE user_id='$userId'") or die(mysql_error());  
$row = mysql_fetch_array($result);



if($row['theme_header'] == "") {

$theme_header = "c";

}
else
{
$theme_header = $row['theme_header'];
}

if($row['theme'] == "") {
$theme = "c";

}
else
{
$theme = $row['theme'];
}


}?>




<!DOCTYPE html> 
<html> 
 
<head>
	<meta charset="utf-8">
	<meta name="viewport" content="width=device-width, initial-scale=1"> 
	<title>Setup NoDo WebApp</title> 
	<link rel="stylesheet" href="http://code.jquery.com/mobile/1.0rc2/jquery.mobile-1.0rc2.min.css" />
	<script src="http://code.jquery.com/jquery-1.6.4.min.js"></script>
	<script src="http://code.jquery.com/mobile/1.0rc2/jquery.mobile-1.0rc2.min.js"></script>
</head> 
 
<body> 

 
<div data-role="page" data-theme="c">
 
	<div data-role="header" data-theme="a">
		<h1>Setup NoDo WebApp</h1>
		<a href="/admin/index.php" data-icon="gear" class="ui-btn-right" data-ajax="false">Setup</a>
		<a href="/index.php" data-icon="home" class="ui-btn-left" data-ajax="false" data-iconpos="notext">Home</a>
	</div><!-- /header -->
 
	<div data-role="content">	

	<form action="setup_nodoweb.php" data-ajax="false" method="post"> 
	
			
			<label for="title">Titel:</label>
			<input type="text" name="title" id="title" value="<?php echo $row['title'];?>"  />
			
			<br>
	 
			<label for="theme" class="select">Thema:</label>
		    <select name="theme" id="theme" data-placeholder="true" data-native-menu="false">
				<option value="a" <?php if ($row['theme'] == "a") {echo 'selected="selected"';}?>>Zwart</option>
				<option value="b" <?php if ($row['theme'] == "b") {echo 'selected="selected"';}?>>Blauw</option>
				<option value="c" <?php if ($row['theme'] == "c") {echo 'selected="selected"';}?>>Grijs</option>
				<option value="d" <?php if ($row['theme'] == "d") {echo 'selected="selected"';}?>>Wit</option>
				<option value="e" <?php if ($row['theme'] == "e") {echo 'selected="selected"';}?>>Geel</option>
			</select>
			
			<label for="theme_header" class="select">Thema balken:</label>
		    <select name="theme_header" id="theme_header" data-placeholder="true" data-native-menu="false">
				<option value="a" <?php if ($row['theme_header'] == "a") {echo 'selected="selected"';}?>>Zwart</option>
				<option value="b" <?php if ($row['theme_header'] == "b") {echo 'selected="selected"';}?>>Blauw</option>
				<option value="c" <?php if ($row['theme_header'] == "c") {echo 'selected="selected"';}?>>Licht Grijs</option>
				<option value="d" <?php if ($row['theme_header'] == "d") {echo 'selected="selected"';}?>>Donker Grijs</option>
				<option value="e" <?php if ($row['theme_header'] == "e") {echo 'selected="selected"';}?>>Geel</option>
			</select>
	
	
   
        
		<input type="submit" name="submit" value="Aanpassen" >

		
	
	</form> 

	

	


	
	</div><!-- /content -->
	
	<div data-role="footer" data-theme="a">
		<h4></h4>
	</div><!-- /footer -->
	
</div><!-- /page -->
 
</body>
</html>

