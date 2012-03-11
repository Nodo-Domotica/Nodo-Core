<?php

require_once('include/webapp_settings.php');



// Initialize the session.
session_start();

// Unset all of the session variables.
$_SESSION = array();

// If it's desired to kill the session, also delete the session cookie.
// Note: This will destroy the session, and not just the session data!
if (ini_get("session.use_cookies")) {
    $params = session_get_cookie_params();
    setcookie(session_name(), '', time() - 42000,
        $params["path"], $params["domain"],
        $params["secure"], $params["httponly"]
    );
}

// Finally, destroy the session.
session_destroy();

 setcookie('username', '', time()-3600);
 setcookie('password', '', time()-3600);


 

?>

<!DOCTYPE html> 
<html> 

<head>
	<meta charset="utf-8">
	<meta name="viewport" content="width=device-width, initial-scale=1"> 
	<title>Nodo Web App</title> 
	<?php require_once('include/jquery_mobile.php'); ?>
</head> 

<body> 



<div data-role="page">

	<div data-role="header">
		<h1>Nodo Web App</h1>
	</div><!-- /header -->

	<div data-role="content">	
		<div align="center">You are now logged out...</div>
	</div><!-- /content -->
	
	<div data-role="footer">
		<h4></h4>
	</div><!-- /footer -->
	
</div><!-- /page -->

</body>
</html>