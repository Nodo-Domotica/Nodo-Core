<?php 
require_once('connections/tc.php');
require_once('include/auth.php');
require_once('include/settings.php'); 



$page_title = "Informatie";
 



?>


<!DOCTYPE html> 
<html> 

<head>
	<meta charset="utf-8">
	<meta name="viewport" content="width=device-width, initial-scale=1"> 
	<title><?php echo $title ?></title> 
	<?php require_once('include/jquery_mobile.php'); ?>
	<?php require_once('include/send_event.php'); ?>
</head> 

<body> 

<div data-role="page" data-theme="<?php echo $theme?>">

<?php require_once('include/header.php'); ?>

	<div data-role="content">	
		<?php
		
          
		   
		  

mysql_select_db($database_tc, $tc);

$RSevent_log = mysql_query("SELECT * FROM (SELECT * FROM nodo_tbl_event_log WHERE user_id='$userId' ORDER BY id DESC LIMIT 25)x ORDER BY id") or die(mysql_error());
?>

	<div data-role="collapsible" data-collapsed="false">
	<h3>Laatste 25 events</h3>


<?php		
		while($row = mysql_fetch_array($RSevent_log)) 
		{                                
?>		           
		<?php echo $row['timestamp'];?> - <?php echo $row['event']; ?> <br>		
		
<?php		
		}         
		?>
	<br>
	
	
	</div>	

	<a href="export_csv.php" data-role="button" data-inline="true" data-ajax="false">Exporteer alle events naar csv</a>
	</div><!-- /content -->
	
	<?php require_once('include/footer.php'); ?>
	
</div><!-- /page -->

</body>
</html>