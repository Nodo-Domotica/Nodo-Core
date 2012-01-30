<!DOCTYPE html> 
<html> 

<head>
	<meta charset="utf-8">
	<meta name="viewport" content="width=device-width, initial-scale=1"> 
	<title>Activiteit verwijderen</title> 
	<link rel="stylesheet" href="http://code.jquery.com/mobile/1.0rc2/jquery.mobile-1.0rc2.min.css" />
	<script src="http://code.jquery.com/jquery-1.6.4.min.js"></script>
	<script src="http://code.jquery.com/mobile/1.0rc2/jquery.mobile-1.0rc2.min.js"></script>
</head> 

<body> 

<div data-role="page" data-theme="c">

	<div data-role="header">
		<h1>Activiteit verwijderen?</h1>
	</div><!-- /header -->

	<div data-role="content">	
		<h2>Verwijderen?</h2>
		<br>
		<a href="activities_delete.php?id=<?php echo $_GET["id"];?> " data-role="button" data-inline="true" data-ajax="false">Ja</a>
		<a href="activities.php" data-role="button" data-inline="true" data-ajax="false">Nee</a>
	</div><!-- /content -->
	
	
	
	
</div><!-- /page -->

</body>
</html>
