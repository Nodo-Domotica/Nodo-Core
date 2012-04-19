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

require_once('connections/db_connection.php');
require_once('include/webapp_settings.php'); 


if (isset($_POST['submit'])) 
{


			$email = mysql_real_escape_string(htmlspecialchars($_POST['email'])); 

			mysql_select_db($database, $db);
			$result = mysql_query("SELECT id,user_login_name FROM nodo_tbl_users WHERE user_login_name='$email'") or die(mysql_error());  
			$row = mysql_fetch_array($result);

			$id = $row['id'];
				
				//Als we een record terug krijgen dat is er een account met het ingevulde e-mail adres
				if($id > 0) {
				
				
				
					//Nieuw wachtwoord genereren
					$password_length = 7;
					$possible_chars = "#%*!@&=+1234567890ABCDEFGHIJKLMNPQRSTUVWXYZabcdefghijkmnpqrstuvwxyz";

					
					$i = 0;
					
						while ($i < $password_length) {

							//Get random character from $possible_chars
							$char = substr($possible_chars, mt_rand(0, strlen($possible_chars)-1), 1);

							$password .= $char;

							$i++;

						}
						
					$password_encoded = md5($salt.$password);
					
					
					//Gegevens in de database opslaan
					mysql_select_db($database, $db);
					mysql_query("UPDATE nodo_tbl_users SET user_password='$password_encoded' WHERE id='$id'") or die(mysql_error());
					
					//Verificatie e-mail sturen.
					 $to = $email;
					 $subject = "New password for your Nodo Web App account";
					 $message="Your Nodo Web App login details \r\n";
					 $message.="Username: $email\r\n";
					 $message.="Password: $password\r\n";
					 $from = "webapp@nodo-domotica.nl";
					 $headers = "From:" . $from;
					 
					 $sentmail = mail($to,$subject,$message,$headers);

					 if($sentmail){
						header("Location: lost_password_ok.php");  
					}
					
					else {
						die('Sorry an error occured!!!');
					}
				
 
				}
				else {
						$error_message = "Sorry we have no e-mail address: $email in our database.";
				}


} 
?> 

<!DOCTYPE html> 
<html>

<head>
	<meta charset="utf-8">
	<meta name="viewport" content="width=device-width, initial-scale=1"> 
	<title>Nodo Web App lost password</title> 
	<?php require_once('include/jquery_mobile.php'); ?>
</head> 

<body> 

<div data-role="page">

	<div data-role="header">
		<h1>Nodo Web App lost password</h1>
		<div data-role="navbar" data-iconpos="top">
		<ul>
			<li><a href="login.php" data-icon="star"  data-ajax="false">Login</a></li>
		</ul>
	</div>
	</div><!-- /header -->

	<div data-role="content">	
	
	<?php echo $error_message; ?>
		
		<form action="lost_password.php" data-ajax="false" method="post">		
			
						
			<label for="email">Enter your e-mail address:</label>
			<input type="text" name="email" id="email" value=""  />
			<br>
				
			<input type="submit" name="submit" id="submit" value="Send" >
		</form>
				
		
	</div><!-- /content -->
	
	<div data-role="footer">
		<h4></h4>
	</div><!-- /footer -->
	
</div><!-- /page -->
<script>	
$(document).ready(function() {
      $('#submit').click(function() {
		  $(".error").hide();
		  var hasError = false;
		  var emailReg = /^([\w-\.]+@([\w-]+\.)+[\w-]{2,4})?$/;
		  var emailaddressVal = $("#email").val();
		  if(emailaddressVal == '') {
			  $("#email").before('<span class="error"><b>Please enter your email address.</b></span>');
			  hasError = true;
			  }
		  else if(!emailReg.test(emailaddressVal)) {
			  $("#email").before('<span class="error"><b>Enter a valid email address.</b></span>');
			  hasError = true;
		  }
		  
		 		 
		 if(hasError == true) { return false; }
	 });
	 
	 
		  });
</script>
</body>
</html>