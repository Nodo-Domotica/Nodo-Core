<?php 


require_once('connections/tc.php'); 
require_once('include/webapp_settings.php');



$message = "";

if (isset($_POST['submit'])) 
{


$username = mysql_real_escape_string(htmlspecialchars($_POST['username'])); 
$password = $_POST['password']; 
$password = md5($salt.$password);


mysql_select_db($database_tc, $tc);
$result = mysql_query("SELECT id,user_login_name,user_password,active FROM nodo_tbl_users WHERE user_login_name='$username' AND user_password='$password'") or die(mysql_error());  
$row = mysql_fetch_array($result);
$id = $row['id'];
$active = $row['active'];

	if($id > 0 && $active == 1) {
		//log in the user
		session_start();
		$_SESSION['userId'] = $id;
		header("Location: devices.php");
	}
	
	else
	
	{ 
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
		
		<form action="login.php" data-ajax="false" method="post">		
			<label for="username">Username:</label>
			<input type="text" name="username" id="username" value=""  />
			<label for="password">Password:</label>
			<input type="password" name="password" id="password" value=""  />
			<br>
			<input type="submit" name="submit" value="Login" >
		</form>
		 
		 
		 
	</div><!-- /content -->
	
	<div data-role="footer">
		<h4></h4>
		</div>
	</div><!-- /footer -->
	
</div><!-- /page -->

</body>
</html>