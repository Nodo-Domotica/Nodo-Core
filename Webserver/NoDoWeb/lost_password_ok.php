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
		
		<p>Your login details has been sent to your email address</p>	
				
		
	</div><!-- /content -->
	
	<div data-role="footer">
		<h4></h4>
	</div><!-- /footer -->
	
</div><!-- /page -->

</body>
</html>