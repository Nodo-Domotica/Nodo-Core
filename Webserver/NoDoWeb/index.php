<?php 

require_once('connections/tc.php'); 
require_once('include/auth.php'); 
require_once('include/settings.php'); 

$page_title = "Apparaten";


//Lees schakelaars uit
mysql_select_db($database_tc, $tc);
$query_RSswitch = "SELECT * FROM nodo_tbl_devices WHERE user_id='$userId'";
$RSswitch = mysql_query($query_RSswitch, $tc) or die(mysql_error());
$row_RSswitch = mysql_fetch_assoc($RSswitch);
$totalRows_RSswitch = mysql_num_rows($RSswitch);


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
		 
		<h3><span id='switch_<?php echo $row_RSswitch['id']; ?>'></span><?php echo $row_RSswitch['naam']; ?></h3>
		
		 <p>
          <?php $id = $row_RSswitch['id']; ?>
          
		  

<?php $type = $row_RSswitch['type'];
	  $dim = $row_RSswitch['dim'];


switch ($type)
			{
			case "1":?>
			
			<!-- aan/uit kaku buttons -->
			<a href="javascript:send_event(&quot;sendkaku <?php echo $row_RSswitch['homecode'] . $row_RSswitch['address'] ;?>,on&quot;)" data-role="button" data-icon="check" >Aan</a>
			<a href="javascript:send_event(&quot;sendkaku <?php echo $row_RSswitch['homecode'] . $row_RSswitch['address'] ;?>,off&quot;)" data-role="button" data-icon="delete">Uit</a>
			<!-- /aan/uit kaku buttons -->
			
			<?php  break; ?>  
			
			//newkaku
			<?php case "2":?>
			
			<!-- aan/uit newkaku buttons -->
			<a href="javascript:send_event(&quot;sendnewkaku <?php echo $row_RSswitch['address']; ?>,on&quot;)" data-role="button"  data-icon="check" >Aan</a>
			<a href="javascript:send_event(&quot;sendnewkaku <?php echo $row_RSswitch['address']; ?>,off&quot;)" data-role="button"  data-icon="delete" >Uit</a>
			<!-- /aan/uit newkaku buttons -->
			
			<br>
			
			<?php if ( $dim == "1" ) { ?>
	
			<!-- Dim buttons -->
			<a href="javascript:send_event(&quot;sendnewkaku <?php echo $row_RSswitch['address']; ?>,1&quot;)" data-role="button" data-inline="true">10%</a>
			<a href="javascript:send_event(&quot;sendnewkaku <?php echo $row_RSswitch['address']; ?>,2&quot;)" data-role="button" data-inline="true">20%</a>
			<a href="javascript:send_event(&quot;sendnewkaku <?php echo $row_RSswitch['address']; ?>,4&quot;)" data-role="button" data-inline="true">30%</a>
			<a href="javascript:send_event(&quot;sendnewkaku <?php echo $row_RSswitch['address']; ?>,6&quot;)" data-role="button" data-inline="true">40%</a>
			<a href="javascript:send_event(&quot;sendnewkaku <?php echo $row_RSswitch['address']; ?>,8&quot;)" data-role="button" data-inline="true">50%</a>
			<a href="javascript:send_event(&quot;sendnewkaku <?php echo $row_RSswitch['address']; ?>,10&quot;)" data-role="button" data-inline="true">60%</a>
			<a href="javascript:send_event(&quot;sendnewkaku <?php echo $row_RSswitch['address']; ?>,12&quot;)" data-role="button" data-inline="true">70%</a>
			<a href="javascript:send_event(&quot;sendnewkaku <?php echo $row_RSswitch['address']; ?>,14&quot;)" data-role="button" data-inline="true">80%</a>
			<a href="javascript:send_event(&quot;sendnewkaku <?php echo $row_RSswitch['address']; ?>,15&quot;)" data-role="button" data-inline="true">90%</a>
			<a href="javascript:send_event(&quot;sendnewkaku <?php echo $row_RSswitch['address']; ?>,16&quot;)" data-role="button" data-inline="true">100%</a>
			<!-- /Dim buttons -->
			<?php } 
			
			if ( $dim == "2" ) { ?>
			
			
			<!-- Dim slider -->
			<script>
				var t<?php echo $row_RSswitch['id']; ?>;
				function update_distance_timer_<?php echo $row_RSswitch['id']; ?>()
					{
					clearTimeout(t<?php echo $row_RSswitch['id']; ?>);
					t<?php echo $row_RSswitch['id']; ?>=setTimeout("update_distance_<?php echo $row_RSswitch['id']; ?>()",200);
					}
				function update_distance_<?php echo $row_RSswitch['id']; ?>()
					{
					var val<?php echo $row_RSswitch['id']; ?> = $('#distSlider<?php echo $row_RSswitch['id']; ?>').val();
					//alert(val);
					send_event('sendnewkaku <?php echo $row_RSswitch['address']; ?>,' + val<?php echo $row_RSswitch['id']; ?>)
					}
			</script>
			
			<div  data-role="fieldcontain" id="element-distSlider">
			<label  id="distSlider-label" for="distSlider">Dim: </label>
			<input  name="distSlider" id="distSlider<?php echo $row_RSswitch['id']; ?>" value="0" min="0" max="16" data-type="range" onChange='update_distance_timer_<?php echo $row_RSswitch['id']; ?>()'>
			</div>
			<!-- /Dim slider -->

			<?php } break; }?>   

			 
		
		</p></div>
    <?php } while ($row_RSswitch = mysql_fetch_assoc($RSswitch)); ?>


	</div><!-- /content -->
	
	
	<?php require_once('include/footer.php'); ?>
	
	
</div><!-- /page -->


</body>
</html>
