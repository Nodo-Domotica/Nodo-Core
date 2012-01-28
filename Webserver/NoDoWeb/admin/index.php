<!DOCTYPE html> 
<html> 

<head>
	
	<meta charset="utf-8">
	<meta name="viewport" content="width=device-width, initial-scale=1"> 
	<title>Setup</title> 
	
	<link rel="stylesheet" href="http://code.jquery.com/mobile/1.0rc2/jquery.mobile-1.0rc2.min.css" />
	<script src="http://code.jquery.com/jquery-1.6.4.min.js"></script>
	<script src="http://code.jquery.com/mobile/1.0rc2/jquery.mobile-1.0rc2.min.js"></script>
	
</head> 

<body> 

<div data-role="page">

	<div data-role="header" >
		<h1>Setup</h1>
		<a href="/index.php" data-icon="home" class="ui-btn-right" data-ajax="false" data-iconpos="notext">Home</a>
	</div><!-- /header -->

	<div data-role="content">	
	  <p><a href="setup_connection.php" data-role="button" data-ajax="false">Communicatie</a></p>
	  <p><a href="switch.php" data-role="button" data-ajax="false">Apparaten</a></p>
	  <p><a href="activities.php" data-role="button" data-ajax="false">Activiteiten</a></p>
   	  <p><a href="setup_nodoweb.php" data-role="button" data-ajax="false">NoDoWeb</a></p>
	</div><!-- /content -->
	
	<div data-role="footer">
		<h4></h4>
	</div><!-- /footer -->
	
</div><!-- /page -->

</body>
</html>