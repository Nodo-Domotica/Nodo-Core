
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
		<div data-role="navbar" data-iconpos="top">
		<ul>
			<li><a href="login.php" data-icon="star"  data-ajax="false">Login</a></li>
			
		</ul>
		</div>
	</div><!-- /header -->

	<div data-role="content">	
		<p>Your Nodo Web App account is created.</p>		
		<p>Login details and a confirmation link has been sent to your email address.</p>	
				
		
	</div><!-- /content -->
	
	<div data-role="footer">
		<h4></h4>
	</div><!-- /footer -->
	
</div><!-- /page -->

</body>
</html>