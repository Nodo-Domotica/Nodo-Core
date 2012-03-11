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
	  
	
<?php while ($row_RSdevices = mysql_fetch_assoc($RSdevices)){ ?>

    	<div data-role="collapsible" data-content-theme="<?php echo $theme?>" <?php if($row_RSdevices['collapsed']==1) {echo "data-collapsed=\"false\"";}?> >
		 
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
					
				//Dim buttons		
				if ( $row_RSdevices['dim'] == 1 ) { 
					
					for ($i=1; $i<=10; $i++) {
					
						   $dim_percentage = $i * 10;
						   $dim_level = round($i * 1.6,0);
						   
							echo "<a href=\"javascript:send_event(&quot;sendnewkaku ".$row_RSdevices['address'].",".$dim_level."&quot;)\" data-role=\"button\" data-inline=\"true\">".$dim_percentage."%</a>\n";
					}
				}		   
 		
				//Dim slider
				if ( $row_RSdevices['dim'] == 2 ) { 
								
					
					echo "<script>\n";
						echo "var t".$row_RSdevices['id'].";\n";
						echo "function update_distance_timer_".$row_RSdevices['id']."()\n";
							echo "{\n";
							echo "clearTimeout(t".$row_RSdevices['id'].");\n";
							echo "t".$row_RSdevices['id']."=setTimeout(\"update_distance_".$row_RSdevices['id']."()\",200);\n";
							echo "}\n";
						echo "function update_distance_".$row_RSdevices['id']."()\n";
							echo "{\n";
							echo "var val".$row_RSdevices['id']." = $('#distSlider".$row_RSdevices['id']. "').val();\n";
							echo "send_event('sendnewkaku ".$row_RSdevices['address'].",' + val".$row_RSdevices['id'].")\n";
							echo "}\n";
					echo "</script>\n";			
					
					echo "<label id=\"distSlider-label\" for=\"distSlider\">Dim: </label>\n";
					echo "<input name=\"distSlider\" id=\"distSlider".$row_RSdevices['id']."\" value=\"".$row_RSdevices['dim_value']."\" min=\"1\" max=\"16\" data-type=\"range\" onChange='update_distance_timer_".$row_RSdevices['id']."()'>\n";
					
					

				 } 
			break;    
			
			case 3:
			// On/Off WiredOut buttons
			echo "<a href=\"javascript:send_event(&quot;wiredout ".$row_RSdevices['address'].",on;sendstatus wiredout,".$row_RSdevices['address']."&quot;)\" data-role=\"button\" data-icon=\"check\" >On</a>\n";
			echo "<a href=\"javascript:send_event(&quot;wiredout ".$row_RSdevices['address'].",off;sendstatus wiredout,".$row_RSdevices['address']."&quot;)\" data-role=\"button\" data-icon=\"delete\" >Off</a>\n";
			
        	 break;
			}
	
			echo "</p></div>";
     } ?>


	</div><!-- /content -->
	
<script>
//Device status loop eerste keer opstarten
Device_State();
</script>	
	
	<?php require_once('include/footer.php'); ?>
	
	
</div><!-- /page -->


</body>
</html>
