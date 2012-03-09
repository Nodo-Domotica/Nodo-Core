<?php 

require_once('connections/tc.php'); 
require_once('include/auth.php'); 
require_once('include/user_settings.php'); 

$page_title = "Devices";


mysql_select_db($database_tc, $tc);
$query_RSdevices = "SELECT * FROM nodo_tbl_devices WHERE user_id='$userId'";
$RSdevices = mysql_query($query_RSdevices, $tc) or die(mysql_error());

?>

<!DOCTYPE html> 
<html> 

<head>
	
	<meta charset="utf-8">
	<meta name="viewport" content="width=device-width, initial-scale=1"> 
	<title><?php echo $title ?></title> 
	
	<?php require_once('include/jquery_mobile.php'); ?>
	
	<!-- Get device state -->
	<script src="js/get_device_state.js"></script>
	<!-- /Get device state -->

	<?php require_once('include/send_event.php'); ?>
	
</head> 

<body> 

<div data-role="page" data-theme="<?php echo $theme?>" data-title="<?php echo $title ?>">

<?php require_once('include/header.php'); ?>
	<div data-role="content">	
	  
	<?php 
	//if (mysql_num_rows($RSdevices) == 0) {echo "<h4>No devices available to show.<br>You can add a device via <a href=\"admin\"\>setup</a></h4>";} ?>
	
	
	<?php while ($row_RSdevices = mysql_fetch_assoc($RSdevices)){ ?>
        

    	<div data-role="collapsible" data-content-theme="<?php echo $theme?>">
		 
		<h3><span id='switch_<?php echo $row_RSdevices['id']; ?>'></span><?php echo $row_RSdevices['naam']; ?></h3>
		
		 <p>
          
		  <?php $id = $row_RSdevices['id']; ?>
          
		  

<?php 


switch ($row_RSdevices['type'])
			{
			case "1":
			
			// On/Off kaku buttons
			echo "<a href=\"javascript:send_event(&quot;sendkaku " . $row_RSdevices['homecode'] . $row_RSdevices['address'] . ",on&quot;)\" data-role=\"button\" data-icon=\"check\" >On</a>\n";
            echo "<a href=\"javascript:send_event(&quot;sendkaku " . $row_RSdevices['homecode'] . $row_RSdevices['address'] . ",off&quot;)\" data-role=\"button\" data-icon=\"delete\" >Off</a>\n";
			break;  
						
			case "2":
			// On/Off newkaku buttons 
			echo "<a href=\"javascript:send_event(&quot;sendnewkaku " . $row_RSdevices['address'] . ",on&quot;)\" data-role=\"button\" data-icon=\"check\" >On</a>\n";
			echo "<a href=\"javascript:send_event(&quot;sendnewkaku " . $row_RSdevices['address'] . ",off&quot;)\" data-role=\"button\"  data-icon=\"delete\" >Off</a>\n";
			echo "<br>\n";
						
			if ( $row_RSdevices['dim'] == 1 ) { 

				//Dim buttons
				for ($i=1; $i<=10; $i++) {
				
					   $dim_percentage = $i * 10;
					   $dim_level = round($i * 1.6,0);
					   
						echo "<a href=\"javascript:send_event(&quot;sendnewkaku ".$row_RSdevices['address'].",".$dim_level."&quot;)\" data-role=\"button\" data-inline=\"true\">".$dim_percentage."%</a>\n";
				}
			}		   
 		
			if ( $row_RSdevices['dim'] == 2 ) { ?>
			
			
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

			<?php } break;    
			
			case 3:?>
			<!-- On/Off WiredOut buttons -->
			<a href="javascript:send_event(&quot;wiredout <?php echo $row_RSdevices['address']; ?>,on;sendstatus wiredout,<?php echo $row_RSdevices['address']; ?>&quot;)" data-role="button"  data-icon="check" >On</a>
			<a href="javascript:send_event(&quot;wiredout <?php echo $row_RSdevices['address']; ?>,off;sendstatus wiredout,<?php echo $row_RSdevices['address']; ?>&quot;)" data-role="button"  data-icon="delete" >Off</a>
			<!-- /On/Off WiredOut buttons -->
			
			<?php  break;
			}?>

			 
		
		</p></div>
    <?php } ?>


	</div><!-- /content -->
	
<script>
//Device status loop eerste keer opstarten
Device_State();
</script>	
	
	<?php require_once('include/footer.php'); ?>
	
	
</div><!-- /page -->


</body>
</html>
