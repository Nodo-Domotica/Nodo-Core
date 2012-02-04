<?php 

require_once('connections/tc.php'); 
require_once('include/auth.php'); 
require_once('include/settings.php'); 

$page_title = "Apparaten";


//Lees schakelaars uit
mysql_select_db($database_tc, $tc);
$query_RSdevices = "SELECT * FROM nodo_tbl_devices WHERE user_id='$userId'";
$RSdevices = mysql_query($query_RSdevices, $tc) or die(mysql_error());
$row_RSdevices = mysql_fetch_assoc($RSdevices);
$totalRows_RSdevices = mysql_num_rows($RSdevices);


?>

<!DOCTYPE html> 
<html> 

<head>
	
	<meta charset="utf-8">
	<meta name="viewport" content="width=device-width, initial-scale=1"> 
	<title><?php echo $title ?></title> 
	
	<?php require_once('include/jquery_mobile.php'); ?>
	
	<!-- NoDoWebapp client side java -->
	<script src="js/get_device_state.js"></script>
	<!-- /NoDoWebapp client side java -->
	
	
	<?php require_once('include/send_event.php'); ?>
	
</head> 

<body> 

<div data-role="page" data-theme="<?php echo $theme?>" data-title="<?php echo $title ?>">

<?php require_once('include/header.php'); ?>
	
	
	<div data-role="content" >	
	  
	
	<?php do { ?>
         <div data-role="collapsible">
		 
		<h3><span id='switch_<?php echo $row_RSdevices['id']; ?>'></span><?php echo $row_RSdevices['naam']; ?></h3>
		
		 <p>
          <?php $id = $row_RSdevices['id']; ?>
          
		  

<?php $type = $row_RSdevices['type'];
	  $dim = $row_RSdevices['dim'];


switch ($type)
			{
			case "1":?>
			
			<!-- aan/uit kaku buttons -->
			<a href="javascript:send_event(&quot;sendkaku <?php echo $row_RSdevices['homecode'] . $row_RSdevices['address'] ;?>,on&quot;)" data-role="button" data-icon="check" >Aan</a>
			<a href="javascript:send_event(&quot;sendkaku <?php echo $row_RSdevices['homecode'] . $row_RSdevices['address'] ;?>,off&quot;)" data-role="button" data-icon="delete">Uit</a>
			<!-- /aan/uit kaku buttons -->
			
			<?php  break; ?>  
			
			//newkaku
			<?php case "2":?>
			
			<!-- aan/uit newkaku buttons -->
			<a href="javascript:send_event(&quot;sendnewkaku <?php echo $row_RSdevices['address']; ?>,on&quot;)" data-role="button"  data-icon="check" >Aan</a>
			<a href="javascript:send_event(&quot;sendnewkaku <?php echo $row_RSdevices['address']; ?>,off&quot;)" data-role="button"  data-icon="delete" >Uit</a>
			<!-- /aan/uit newkaku buttons -->
			
			<br>
			
			<?php if ( $dim == "1" ) { ?>
	
			<!-- Dim buttons -->
			<a href="javascript:send_event(&quot;sendnewkaku <?php echo $row_RSdevices['address']; ?>,1&quot;)" data-role="button" data-inline="true">10%</a>
			<a href="javascript:send_event(&quot;sendnewkaku <?php echo $row_RSdevices['address']; ?>,2&quot;)" data-role="button" data-inline="true">20%</a>
			<a href="javascript:send_event(&quot;sendnewkaku <?php echo $row_RSdevices['address']; ?>,4&quot;)" data-role="button" data-inline="true">30%</a>
			<a href="javascript:send_event(&quot;sendnewkaku <?php echo $row_RSdevices['address']; ?>,6&quot;)" data-role="button" data-inline="true">40%</a>
			<a href="javascript:send_event(&quot;sendnewkaku <?php echo $row_RSdevices['address']; ?>,8&quot;)" data-role="button" data-inline="true">50%</a>
			<a href="javascript:send_event(&quot;sendnewkaku <?php echo $row_RSdevices['address']; ?>,10&quot;)" data-role="button" data-inline="true">60%</a>
			<a href="javascript:send_event(&quot;sendnewkaku <?php echo $row_RSdevices['address']; ?>,12&quot;)" data-role="button" data-inline="true">70%</a>
			<a href="javascript:send_event(&quot;sendnewkaku <?php echo $row_RSdevices['address']; ?>,14&quot;)" data-role="button" data-inline="true">80%</a>
			<a href="javascript:send_event(&quot;sendnewkaku <?php echo $row_RSdevices['address']; ?>,15&quot;)" data-role="button" data-inline="true">90%</a>
			<a href="javascript:send_event(&quot;sendnewkaku <?php echo $row_RSdevices['address']; ?>,16&quot;)" data-role="button" data-inline="true">100%</a>
			<!-- /Dim buttons -->
			<?php } 
			
			if ( $dim == "2" ) { ?>
			
			
			<!-- Dim slider -->
			<script>
				var t<?php echo $row_RSdevices['id']; ?>;
				function update_distance_timer_<?php echo $row_RSdevices['id']; ?>()
					{
					clearTimeout(t<?php echo $row_RSdevices['id']; ?>);
					t<?php echo $row_RSdevices['id']; ?>=setTimeout("update_distance_<?php echo $row_RSdevices['id']; ?>()",200);
					}
				function update_distance_<?php echo $row_RSdevices['id']; ?>()
					{
					var val<?php echo $row_RSdevices['id']; ?> = $('#distSlider<?php echo $row_RSdevices['id']; ?>').val();
					//alert(val);
					send_event('sendnewkaku <?php echo $row_RSdevices['address']; ?>,' + val<?php echo $row_RSdevices['id']; ?>)
					}
			</script>
			
			
			<label  id="distSlider-label" for="distSlider">Dim: </label>
			<input  name="distSlider" id="distSlider<?php echo $row_RSdevices['id']; ?>" value="<?php echo $row_RSdevices['dim_value'];?>" min="1" max="16" data-type="range" onChange='update_distance_timer_<?php echo $row_RSdevices['id']; ?>()'>
			
			<!-- /Dim slider -->

			<?php } break; }?>   

			 
		
		</p></div>
    <?php } while ($row_RSdevices = mysql_fetch_assoc($RSdevices)); ?>


	</div><!-- /content -->
	
	
	<?php require_once('include/footer.php'); ?>
	
	
</div><!-- /page -->


</body>
</html>
