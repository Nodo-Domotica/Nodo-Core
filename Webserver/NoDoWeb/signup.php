<?php 




require_once('connections/tc.php');
require_once('include/webapp_settings.php'); 



$message = "";

if (isset($_POST['submit'])) 
{

$first_name = mysql_real_escape_string(htmlspecialchars($_POST['first_name']));
$last_name = mysql_real_escape_string(htmlspecialchars($_POST['last_name'])); 
$email = mysql_real_escape_string(htmlspecialchars($_POST['email'])); 
//$password = mysql_real_escape_string(htmlspecialchars($_POST['password'])); 

mysql_select_db($database_tc, $tc);
$result = mysql_query("SELECT id,user_login_name FROM nodo_tbl_users WHERE user_login_name='$email'") or die(mysql_error());  
$row = mysql_fetch_array($result);

$id = $row['id'];
	
	//Als we een record terug krijgen dat is er al een account met hetzelfde e-mail adres
	if($id > 0) {
		$error_message = "<h4>Sorry this e-mail address is already in use</h4>";
	}
	
	else
	
	{ 
	
	/************************************************************************************************
Generate NoDo ID													
*************************************************************************************************/ 
//Only generate ID when ID in database is empty
if ($nodo_id == "")  { 
   
   
	//ID Lenght
	$unique_ref_length = 8;


	$unique_ref_found = false;

	
	$possible_chars = "1234567890ABCDEFGHIJKLMNPQRSTUVW";

	//Until we find a unique id
	while (!$unique_ref_found) {

	
		$unique_ref = "";

		
		$i = 0;

	
		while ($i < $unique_ref_length) {

			//Get random character from $possible_chars
			$char = substr($possible_chars, mt_rand(0, strlen($possible_chars)-1), 1);

			$unique_ref .= $char;

			$i++;

		}

	
		//ID generated. Check if ID exists in Database
		mysql_select_db($database_tc, $tc);
		$result = mysql_query("SELECT 'nodo_id' FROM nodo_tbl_users WHERE nodo_id='$unique_ref'") or die(mysql_error());  
		$row = mysql_fetch_array($result);
		
		if (mysql_num_rows($result)==0) {

			//We have a unique ID
			$unique_ref_found = true;

		}
	
	
	}

$nodo_id = $unique_ref;
 }  
   
 else{ 
   
  $nodo_id = mysql_real_escape_string(htmlspecialchars($_POST['nodo_id'])); 
  
}
/************************************************************************************************
END Generate NoDo ID													
*************************************************************************************************/ 
	
	
	
		
	//Unieke verificatie code genereren
	$confirm_code=md5(uniqid(rand()));
	
	//Tijdelijk wachtwoord genereren
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
	mysql_select_db($database_tc, $tc);
    mysql_query("INSERT INTO nodo_tbl_users (user_login_name, first_name, last_name, confirm_code, user_password, nodo_id) VALUES ('$email','$first_name','$last_name','$confirm_code','$password_encoded','$nodo_id')") or die(mysql_error());
	
	//Verificatie e-mail sturen.
	 $to = $email;
	 $subject = "Confirm your Nodo Web App account";
	 $message="Dear $first_name $last_name, \r\n\r\n";
	 $message.="Before U can use Nodo Web App please click on this link to activate your account \r\n";
     $message.="http://$WEBAPP_HOST/confirmation.php?passkey=$confirm_code\r\n";
	 $message.="\r\n";
	 $message.="After confirmation your can login with:\r\n";
	 $message.="Username: $email\r\n";
	 $message.="Password: $password\r\n";
	 $message.="\r\n"; 
	 $message.="Your Nodo ID: $nodo_id\r\n";
	 $from = "webapp@nodo-domotica.nl";
	 $headers = "From:" . $from;
	 
	 $sentmail = mail($to,$subject,$message,$headers);

	 if($sentmail){
		header("Location: signup_ok.php");  
	}
	
	else {
		die('Sorry an error occured!!!');
	}

	
	
	
	}
 
} 
?> 
  

<!DOCTYPE html> 
<html> 

<head>
	<meta charset="utf-8">
	<meta name="viewport" content="width=device-width, initial-scale=1"> 
	<title>Nodo Web App Sign up</title> 
	<?php require_once('include/jquery_mobile.php'); ?>



</head> 

<body> 

<div data-role="page">

	<div data-role="header">
		<h1>Nodo Web App Sign up</h1>
		<div data-role="navbar" data-iconpos="top">
		<ul>
			<li><a href="login.php" data-icon="star"  data-ajax="false">Login</a></li>
			
		</ul>
	</div>
	</div><!-- /header -->

	<div data-role="content">	
		
		<?php echo $error_message; ?>
		
		
		<form action="signup.php" data-ajax="false" method="post">		
			<label for="first_name">First name:</label>
			<input type="text" name="first_name" id="first_name" value="<?php echo $first_name ?>"  />
			
			<label for="last_name">Last name:</label>
			<input type="text" name="last_name" id="last_name" value="<?php echo $last_name ?>"  />
			
			<label for="email">Email address:</label>
			<input type="text" name="email" id="email" value="<?php echo $email ?>"  />
			<br>
				
			<input type="submit" name="submit" id="submit" value="Signup" >
		</form>
		 
		 
		 
	</div><!-- /content -->
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
		  
		 var nameVal = $("#first_name").val();
		 if(nameVal == '') {
		 $("#first_name").before('<span class="error"><b>Please enter your first name.<b></span>');
		 hasError = true;
		 }
		  
		 var nameVal = $("#last_name").val();
		 if(nameVal == '') {
			 $("#last_name").before('<span class="error"><b>Please enter your last name.<b></span>');
			 hasError = true;
		 }
		 
		 
		 if(hasError == true) { return false; }
	 });
	 
	 
		  });
</script>
	
	<div data-role="footer">
		<h4></h4>
	</div><!-- /footer -->
	
</div><!-- /page -->

</body>
</html>