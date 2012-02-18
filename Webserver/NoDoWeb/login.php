<?php require_once('Connections/tc.php'); ?>


<?php 

$message = "";

if (isset($_POST['submit'])) 
{

$username = mysql_real_escape_string(htmlspecialchars($_POST['username'])); 
$password = mysql_real_escape_string(htmlspecialchars($_POST['password'])); 

mysql_select_db($database_tc, $tc);
$result = mysql_query("SELECT * FROM nodo_tbl_users WHERE user_name='$username' AND user_passwd='$password'") or die(mysql_error());  
$row = mysql_fetch_array($result);
$id = $row['id'];

	if($id > 0) {
		//log in the user
		session_start();
		$_SESSION['userId'] = $id;
		header("Location: /");
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

	<div data-role="header">
		<h1>Nodo Web App Login</h1>
	</div><!-- /header -->

	<div data-role="content">	
		
		<?php echo $message ?>
		
		<form action="login.php" data-ajax="false" method="post">		
			<label for="username">Username:</label>
			<input type="text" name="username" id="username" value=""  />
			<label for="password">Password:</label>
			<input type="password" name="password" id="password" value=""  />
			<input type="submit" name="submit" value="Login" >
		</form>
		 
		 
		 
	</div><!-- /content -->
	
	<div data-role="footer">
		<h4></h4>
	</div><!-- /footer -->
	
</div><!-- /page -->

</body>
</html>