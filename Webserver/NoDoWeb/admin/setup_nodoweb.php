<?php 
require_once('../connections/tc.php'); 
require_once('../include/auth.php'); 

$page_title = "Setup: Look & Feel";


if (isset($_POST['submit'])) 
{  
 

 $theme = mysql_real_escape_string(htmlspecialchars($_POST['theme'])); 
 $theme_header = mysql_real_escape_string(htmlspecialchars($_POST['theme_header']));  
 $title = mysql_real_escape_string(htmlspecialchars($_POST['title'])); 
 
 
  
 // save the data to the database 
 mysql_select_db($database_tc, $tc);
 mysql_query("UPDATE nodo_tbl_users SET webapp_theme='$theme', webapp_theme_header='$theme_header', webapp_title='$title' WHERE id='$userId'") or die(mysql_error());   
 // once saved, redirect back to the view page 
 header("Location: setup_nodoweb.php#saved");   }
 
else 
{
mysql_select_db($database_tc, $tc);
$result = mysql_query("SELECT * FROM nodo_tbl_users WHERE id='$userId'") or die(mysql_error());  
$row = mysql_fetch_array($result);



if($row['webapp_theme_header'] == "") {

$theme_header = "c";

}
else
{
$theme_header = $row['webapp_theme_header'];
}

if($row['webapp_theme'] == "") {
$theme = "c";

}
else
{
$theme = $row['webapp_theme'];
}


}?>




<!DOCTYPE html> 
<html> 
 
<head>
	<meta charset="utf-8">
	<meta name="viewport" content="width=device-width, initial-scale=1"> 
	<title>Setup NoDo WebApp</title> 
	<?php require_once('../include/jquery_mobile.php'); ?>
</head> 
 
<body> 

 
<div data-role="page" pageid="main">
 
	<?php require_once('../include/header_admin.php'); ?>
 
	<div data-role="content">	

	<form action="setup_nodoweb.php" data-ajax="false" method="post"> 
	
			
			<label for="title">Title:</label>
			<input type="text" name="title" id="title" value="<?php echo $row['webapp_title'];?>"  />
			
			<br>
	 
			<label for="theme" class="select">Theme:</label>
		    <select name="theme" id="theme" data-placeholder="true" data-native-menu="false">
				<option value="a" <?php if ($row['webapp_theme'] == "a") {echo 'selected="selected"';}?>>Black</option>
				<option value="b" <?php if ($row['webapp_theme'] == "b") {echo 'selected="selected"';}?>>Blue</option>
				<option value="c" <?php if ($row['webapp_theme'] == "c") {echo 'selected="selected"';}?>>Grey</option>
				<option value="d" <?php if ($row['webapp_theme'] == "d") {echo 'selected="selected"';}?>>White</option>
				<option value="e" <?php if ($row['webapp_theme'] == "e") {echo 'selected="selected"';}?>>Yellow</option>
			</select>
			
			<label for="theme_header" class="select">Theme headers:</label>
		    <select name="theme_header" id="theme_header" data-placeholder="true" data-native-menu="false">
				<option value="a" <?php if ($row['webapp_theme_header'] == "a") {echo 'selected="selected"';}?>>Black</option>
				<option value="b" <?php if ($row['webapp_theme_header'] == "b") {echo 'selected="selected"';}?>>Blue</option>
				<option value="c" <?php if ($row['webapp_theme_header'] == "c") {echo 'selected="selected"';}?>>Light grey</option>
				<option value="d" <?php if ($row['webapp_theme_header'] == "d") {echo 'selected="selected"';}?>>Dark grey</option>
				<option value="e" <?php if ($row['webapp_theme_header'] == "e") {echo 'selected="selected"';}?>>Yellow</option>
			</select>
			<br>
	        
		<input type="submit" name="submit" value="Save" >

			
	</form> 

		
	</div><!-- /content -->
	
	<?php require_once('../include/footer_admin.php'); ?>
	
</div><!-- /main page -->

<!-- Start of saved page: -->
<div data-role="dialog" id="saved">

	<div data-role="header">
		<h1></h1>
	</div><!-- /header -->

	<div data-role="content">	
		<h2>Settings saved</h2>
				
		<p><a href="#main" data-rel="back" data-role="button" data-inline="true" data-icon="back">Ok</a></p>	
	</div><!-- /content -->
	

</div><!-- /page saved -->
 
</body>
</html>

