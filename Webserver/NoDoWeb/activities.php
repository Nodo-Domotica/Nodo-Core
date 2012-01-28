<?php require_once('connections/tc.php'); ?>
<?php require_once('include/auth.php'); ?>
<?php require_once('include/theme.php'); ?>


<!DOCTYPE html> 
<html> 

<head>
	<meta charset="utf-8">
	<meta name="viewport" content="width=device-width, initial-scale=1"> 
	<title>Activiteiten</title> 
	<link rel="stylesheet" href="//code.jquery.com/mobile/1.0/jquery.mobile-1.0.min.css" />
	<script src="http://code.jquery.com/jquery-1.6.4.min.js"></script>
	<script src="//code.jquery.com/mobile/1.0/jquery.mobile-1.0.min.js"></script>
	
	<script type="text/javascript">
function Ajax_forward2_eg(event)
//stuurt opdrachten door naar eventghost webserver
			{
				var
					$http,
					$self = arguments.callee;

				if (window.XMLHttpRequest) {
					$http = new XMLHttpRequest();
				} else if (window.ActiveXObject) {
					try {
						$http = new ActiveXObject('Msxml2.XMLHTTP');
					} catch(e) {
						$http = new ActiveXObject('Microsoft.XMLHTTP');
					}
				}

				if ($http) {

<?php
//Haal de waarde op uit de setup tabel om te bepalen hoe een event verstuurt dient te worden	
$howto_send_command = $row_RSsetup['howto_send_command'];		
switch ($howto_send_command)
 {
 case 1:
 ?>  
				$http.open('GET', 'forward_tcp.php' + '?event=' + event + '&unique=' + new Date().getTime(), true);
<?php   
   break;
 case 2:
 ?>
 
				$http.open('GET', 'forward_http.php' + '?event=' + event + '&unique=' + new Date().getTime(), true);
<?php
   break;
 
 } 
?>				
				$http.send(null);
				}

			}


</script>
	
</head> 

<body> 

<div data-role="page" data-theme="<?php echo $theme?>">

	<div data-role="header" data-theme="<?php echo $theme_header?>">
		<h1>Activiteiten</h1>
		<div data-role="navbar">
		<ul>
			<li><a href="index.php" data-icon="grid" data-ajax="false">Apparaten</a></li>
			<li><a href="activities.php" data-icon="grid" data-ajax="false">Activiteiten</a></li>
			<li><a href="measurements.php" data-icon="info" data-ajax="false">Metingen</a></li>
		</ul>
		</div>
	</div><!-- /header -->

	<div data-role="content">	
		<?php
		
          
		   
		  
//Lees eigen opdrachten uit
mysql_select_db($database_tc, $tc);

$RSactivities = mysql_query("SELECT * FROM nodo_tbl_activities WHERE user_id='$userId'") or die(mysql_error());
?>

<?php		
		while($row = mysql_fetch_array($RSactivities)) 
		{                                
?>		           
		<a href="javascript:Ajax_forward2_eg(&quot;<?php echo $row['command']; ?>,1&quot;)" data-role="button" data-icon="star" ><?php echo $row['naam']; ?></a>               
		
<?php		
		}         
		?>
		

	</div><!-- /content -->
	
	<div data-role="footer" data-theme="<?php echo $theme_header?>">
		<div data-role="navbar" data-iconpos="top">
		<ul>
			<li><a href="index.php" data-icon="info"  data-ajax="false">Info</a></li>
			<li><a href="admin" data-icon="gear" data-ajax="false">Setup</a></li>
			
			
		</ul>
		</div>
	</div><!-- /footer -->
	
</div><!-- /page -->

</body>
</html>