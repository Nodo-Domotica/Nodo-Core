<?php 
require_once('connections/tc.php');
require_once('include/auth.php');
require_once('include/settings.php'); 

$page_title = "Activities";

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
		
          
		   
		  
//Lees eigen opdrachten uit
mysql_select_db($database_tc, $tc);

$RSactivities = mysql_query("SELECT * FROM nodo_tbl_activities WHERE user_id='$userId'") or die(mysql_error());
?>


<?php		
		while($row = mysql_fetch_array($RSactivities)) 
		{                                
?>		           
		<a href="javascript:send_event(&quot;<?php echo $row['events']; ?>&quot;)" data-role="button" data-icon="star" ><?php echo $row['name']; ?></a>               
		
<?php		
		}         
		?>
		

	</div><!-- /content -->
	
	<?php require_once('include/footer.php'); ?>
	
</div><!-- /page -->

</body>
</html>