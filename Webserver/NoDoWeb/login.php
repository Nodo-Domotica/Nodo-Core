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


require_once('connections/tc.php'); 
require_once('include/webapp_settings.php');

$message = "";

if (isset($_GET['logout']) == 1) {

	$message = "<h4>You are now logged out.</h4>";
	
}


//Als er een cookie met de juiste gegevens bestaat dan redirecten naar de index pagina.
if (isset($_COOKIE['username']) && isset($_COOKIE['password'])) {

	$username = mysql_real_escape_string($_COOKIE['username']);
	$password = mysql_real_escape_string($_COOKIE['password']);
	
	mysql_select_db($database_tc, $tc);
	$result = mysql_query("SELECT id,user_login_name,user_password,active FROM nodo_tbl_users WHERE user_login_name='$username' AND user_password='$password'") or die(mysql_error());  
	$row = mysql_fetch_array($result);
	$id = $row['id'];
	$active = $row['active'];

	if($id > 0 && $active == 1) {
		
		//De gebruiker inloggen
		session_start();
		$_SESSION['userId'] = $id;
		
		header("Location: index.php");
	}
	
}


if (isset($_POST['submit'])) 
{


	$username = mysql_real_escape_string($_POST['username']); 
	$password = mysql_real_escape_string($_POST['password']); 
	$password = md5($salt.$password);


	if (isset($_POST['rememberme'])) {

		 /* Cookie blijft 1 jaar geldig */
		 setcookie('username', $username, time()+60*60*24*365);
		 setcookie('password', $password, time()+60*60*24*365);
	}


	mysql_select_db($database_tc, $tc);
	$result = mysql_query("SELECT id,user_login_name,user_password,active FROM nodo_tbl_users WHERE user_login_name='$username' AND user_password='$password'") or die(mysql_error());  
	$row = mysql_fetch_array($result);
	$id = $row['id'];
	$active = $row['active'];

	if($id > 0 && $active == 1) {
			
		//De gebruiker inloggen
		session_start();
		$_SESSION['userId'] = $id;
		header("Location: index.php");
	}
		
	else { 
		
		$message = "<h4>Username or password is incorrect.</h4>";
		
	}
 
} 
?> 
  

<!DOCTYPE html> 
<html> 

<head>
	<meta charset="utf-8">
	<meta name="viewport" content="width=device-width, initial-scale=1"> 
	<title>Nodo Web App Login</title> 
	<?php require_once('include/jquery_mobile.php'); ?>
</head> 

<body> 

<div data-role="page">

	<div data-role="header" data-theme="a">
		<h1>Nodo Web App Login</h1>
		<div data-role="navbar" data-iconpos="top">
		<ul>
			<li><a href="signup.php" data-icon="star"  data-ajax="false">Sign up</a></li>
			<li><a href="lost_password.php" data-icon="back" data-ajax="false">Lost password</a></li>
		</ul>
	</div>
		
	</div><!-- /header -->
	

	<div data-role="content">	
		
		<?php echo $message ?>
		<?php $_POST['rememberme']; ?>
		
		<form action="login.php" data-ajax="false" method="post">		
			<label for="username">Username:</label>
			<input type="text" name="username" id="username" value=""  />
			<label for="password">Password:</label>
			<input type="password" name="password" id="password" value=""  />
			<br>
			<input type="checkbox" name="rememberme" id="rememberme_1" class="custom" />
			<label for="rememberme_1">Remember me</label>
			<br>
			<input type="submit" name="submit" value="Login" >
		</form>
		 
		 
		 
	</div><!-- /content -->
	
	<div data-role="footer">
		<h4></h4>
		</div>
	</div><!-- /footer -->
	
</div><!-- /page -->
<script>
$(document).ready(function(){
   $("#username").focus();
});


</script>
</body>
</html>