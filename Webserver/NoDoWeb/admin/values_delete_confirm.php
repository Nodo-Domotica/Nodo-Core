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
?>
<!DOCTYPE html> 
<html> 

<head>
	<meta charset="utf-8">
	<meta name="viewport" content="width=device-width, initial-scale=1"> 
	<title>Delete values</title> 
	<link rel="stylesheet" href="http://code.jquery.com/mobile/1.0rc2/jquery.mobile-1.0rc2.min.css" />
	<script src="http://code.jquery.com/jquery-1.6.4.min.js"></script>
	<script src="http://code.jquery.com/mobile/1.0rc2/jquery.mobile-1.0rc2.min.js"></script>
</head> 

<body> 

<div data-role="page" data-theme="c">

	<div data-role="header">
		<h1></h1>
	</div><!-- /header -->

	<div data-role="content">	
		<h2>Delete value?</h2>
		<h4>The historical data of this value will also be deleted!</h4>
		<br>
		<a href="values_delete.php?id=<?php echo $_GET["id"];?> " data-role="button" data-inline="true" data-ajax="false">Yes</a>
		<a href="values.php" data-role="button" data-inline="true" data-ajax="false">No</a>
	</div><!-- /content -->
	
	
	
	
</div><!-- /page -->

</body>
</html>
