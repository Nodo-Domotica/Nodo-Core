<?php require_once('connections/tc.php'); ?>
<?php require_once('include/auth.php'); ?>
<?php require_once('include/theme.php'); ?>

<?php

//Lees setup waarden uit de database
mysql_select_db($database_tc, $tc);
$RS_setup = mysql_query("SELECT * FROM nodo_tbl_setup WHERE user_id='$userId'") or die(mysql_error());  
$row_RSsetup = mysql_fetch_array($RS_setup);


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
	<title>Nodo Due WebApp</title> 
	
	<!-- Jquery mobile -->
	<link rel="stylesheet" href="http://code.jquery.com/mobile/1.0/jquery.mobile-1.0.min.css" />
	<script src="http://code.jquery.com/jquery-1.6.4.min.js"></script>
	<script src="http://code.jquery.com/mobile/1.0/jquery.mobile-1.0.min.js"></script>
	<!-- /Jquery mobile -->
	
	<!-- NoDoWebapp client side java -->
	<script src="js/get_device_state.js"></script>
	<!-- /NoDoWebapp client side java -->
	
	
	
	
<script type="text/javascript">
function send_event(event)
//stuur opdrachten door naar de nodo
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
				$http.open('GET', 'forward_apop.php' + '?event=' + event + '&unique=' + new Date().getTime(), true);
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


<div data-role="page" data-theme="<?php echo $theme?>" data-title="<?php echo $row_RSsetup['title'] ?>">

	<div data-role="header" data-theme="<?php echo $theme_header?>" >
		<h1><?php echo $row_RSsetup['title'] ?></h1>
		<div data-role="navbar" data-iconpos="top">
		<ul>
			<li><a href="index.php" data-icon="grid"  data-ajax="false">Apparaten</a></li>
			<li><a href="activities.php" data-icon="grid" data-ajax="false">Activiteiten</a></li>
			<li><a href="measurements.php" data-icon="info" data-ajax="false">Metingen</a></li>
		</ul>
		</div>
		
		
	</div><!-- /header -->
	
	
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
	
	
	
	<div data-role="footer" data-theme="<?php echo $theme_header?>" >
		
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
