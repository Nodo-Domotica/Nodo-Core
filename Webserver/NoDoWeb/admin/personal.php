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
require_once('../include/webapp_settings.php'); 

$page_title = "Setup: Personal";

if (isset($_POST['submit'])) {

	$theme = mysql_real_escape_string(htmlspecialchars($_POST['theme'])); 
	$theme_header = mysql_real_escape_string(htmlspecialchars($_POST['theme_header']));  
	$default_page = mysql_real_escape_string(htmlspecialchars($_POST['default_page']));  
	$title = mysql_real_escape_string(htmlspecialchars($_POST['title'])); 
	$first_name = mysql_real_escape_string(htmlspecialchars($_POST['first_name']));
	$last_name = mysql_real_escape_string(htmlspecialchars($_POST['last_name']));
	$password = mysql_real_escape_string(htmlspecialchars($_POST['password']));


	if ($password == "") {

		// save the data to the database 
		mysql_select_db($database, $db);
		mysql_query("UPDATE nodo_tbl_users SET webapp_theme='$theme', webapp_theme_header='$theme_header', default_page='$default_page', first_name='$first_name', last_name='$last_name', webapp_title='$title' WHERE id='$userId'") or die(mysql_error());   
		// once saved, redirect back to the view page 
		header("Location: personal.php#saved");   

	}

	else {

		$password_encoded = md5($salt.$password);
		// save the data to the database 
		mysql_select_db($database, $db);
		mysql_query("UPDATE nodo_tbl_users SET user_password='$password_encoded', webapp_theme='$theme', webapp_theme_header='$theme_header', default_page='$default_page', first_name='$first_name', last_name='$last_name', webapp_title='$title' WHERE id='$userId'") or die(mysql_error());   
		// once saved, redirect back to the view page 
		header("Location: personal.php#saved");   

	}

} 

else {
	
	mysql_select_db($database, $db);
	$result = mysql_query("SELECT * FROM nodo_tbl_users WHERE id='$userId'") or die(mysql_error());  
	$row = mysql_fetch_array($result);


	if($row['webapp_theme_header'] == "") {

		$theme_header = "a";

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
	<title>Setup: Personal</title> 
	<?php require_once('../include/jquery_mobile.php'); ?>
</head> 
 
<body> 

 
<div data-role="page" pageid="main" data-theme="<?php echo $theme?>">
 
	<?php require_once('../include/header_admin.php'); ?>
 
	<div data-role="content">	
	<?php echo $error_message; ?>
	<form action="personal.php" data-ajax="false" method="post"> 
					
					
			<label for="title">Web App title:</label>
			<input type="text" name="title" id="title" value="<?php echo $row['webapp_title'];?>"  />
			<br>
			<label for="default_page" class="select">Default page:</label>
		    <select name="default_page" id="default_page" data-placeholder="true" data-native-menu="false">
				<option value="1" <?php if ($row['default_page'] == "1") {echo 'selected="selected"';}?>>Devices</option>
				<option value="2" <?php if ($row['default_page'] == "2") {echo 'selected="selected"';}?>>Activities</option>
				<option value="3" <?php if ($row['default_page'] == "3") {echo 'selected="selected"';}?>>Values</option>
			</select>
			<br>	 
			<label for="theme" class="select">Theme:</label>
		    <select name="theme" id="theme" data-placeholder="true" data-native-menu="false">
				<option value="a" <?php if ($row['webapp_theme'] == "a") {echo 'selected="selected"';}?>>Black</option>
				<option value="b" <?php if ($row['webapp_theme'] == "b") {echo 'selected="selected"';}?>>Blue</option>
				<option value="c" <?php if ($row['webapp_theme'] == "c") {echo 'selected="selected"';}?>>Grey</option>
				<option value="d" <?php if ($row['webapp_theme'] == "d") {echo 'selected="selected"';}?>>White</option>
				<option value="e" <?php if ($row['webapp_theme'] == "e") {echo 'selected="selected"';}?>>Yellow</option>
			</select>
			<br>
			<label for="theme_header" class="select">Theme headers:</label>
		    <select name="theme_header" id="theme_header" data-placeholder="true" data-native-menu="false">
				<option value="a" <?php if ($row['webapp_theme_header'] == "a") {echo 'selected="selected"';}?>>Black</option>
				<option value="b" <?php if ($row['webapp_theme_header'] == "b") {echo 'selected="selected"';}?>>Blue</option>
				<option value="c" <?php if ($row['webapp_theme_header'] == "c") {echo 'selected="selected"';}?>>Light grey</option>
				<option value="d" <?php if ($row['webapp_theme_header'] == "d") {echo 'selected="selected"';}?>>Dark grey</option>
				<option value="e" <?php if ($row['webapp_theme_header'] == "e") {echo 'selected="selected"';}?>>Yellow</option>
			</select>
			<br>
			<label for="first_name">First name:</label>
			<input type="text" name="first_name" id="first_name" value="<?php echo $row['first_name']; ?>"  />
			<br>
			<label for="last_name">Last name:</label>
			<input type="text" name="last_name" id="last_name" value="<?php echo $row['last_name']; ?>"  />
			<br>
			<label for="password">Web App password: (leave empty to keep current password)</label>
			<input type="password" name="password" id="password" value=""  />
			<br>
			<label for="password2">Verify Web App password:</label>
			<input type="password" name="password2" id="password2" value=""  />
			
			<br>
			
	    <a href="index.php" data-role="button" data-ajax="false">Cancel</a>     
		<input type="submit" id="submit" name="submit" value="Save" >

			
	</form> 

		
	</div><!-- /content -->
	
	<?php require_once('../include/footer_admin.php'); ?>
	
</div><!-- /main page -->

<!-- Start of saved page: -->
<div data-role="dialog" id="saved" data-theme="<?php echo $theme?>">

	<div data-role="header" data-theme="<?php echo $theme_header?>">
		<h1></h1>
	</div><!-- /header -->

	<div data-role="content">	
		<h2>Settings saved</h2>
				
		<p><a href="personal.php" data-rel="back" data-role="button" data-inline="true" data-icon="back">Ok</a></p>	
	</div><!-- /content -->
	

</div><!-- /page saved -->
 <script>	
$(document).ready(function() {
      $('#submit').click(function() {
		  $(".error").hide();
		  var hasError = false;
		  
		  
		 var passwordVal = $("#password").val();
		 var password2Val = $("#password2").val();
		 if(passwordVal != password2Val) {
		 $("#password").before('<span class="error"><b>Passwords does not match<b></span>');
		 hasError = true;
		 }
		  
		 		 
		 
		 if(hasError == true) { return false; }
	 });
	 
	 
		  });
</script>
</body>
</html>

