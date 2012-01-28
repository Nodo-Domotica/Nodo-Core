<?php require_once('../connections/tc.php'); ?>
<?php require_once('../include/auth.php'); ?>

<!DOCTYPE html> 
<html> 
 
<head>
	<meta charset="utf-8">
	<meta name="viewport" content="width=device-width, initial-scale=1"> 
	<title>Apparaten aanpassen/toevoegen</title> 
	<link rel="stylesheet" href="http://code.jquery.com/mobile/1.0rc2/jquery.mobile-1.0rc2.min.css" />
	<script src="http://code.jquery.com/jquery-1.6.4.min.js"></script>
	<script src="http://code.jquery.com/mobile/1.0rc2/jquery.mobile-1.0rc2.min.js"></script>
</head> 
 
<body> 

 
<div data-role="page" id="main">
 
	<div data-role="header">
		<h1>Apparaten aanpassen/toevoegen</h1>
		<a href="/admin/index.php" data-icon="gear" class="ui-btn-right" data-ajax="false">Setup</a>
		<a href="/index.php" data-icon="home" class="ui-btn-left" data-ajax="false" data-iconpos="notext">Home</a>
	</div><!-- /header -->
 
	<div data-role="content">	

<form action="switch_add.php" data-ajax="false" method="post"> 
		 
	<div data-role="collapsible">
			
		<h3>Toevoegen</h3>
			<label for="select-choice-0" class="select" >Type apparaat:</label>
		    <select name="type" id="type" data-native-menu="false" >
				<option value="0" data-placeholder="true">Kies apparaat....</option>
				<option value="2">New Kaku</option>
				<option value="1">Kaku</option>
			</select>
	<br>
		
		<div id="name_div"> 		
		<label for="name">Naam:</label>
		<input type="text" name="naam" id="naam" value=""  />
		</div>
	
	<br>
		
		<div id="dim_div">
			<label for="select-choice-1" class="select">Dim mogelijkheid:</label>
			<select name="dim" id="dim" data-native-menu="false">
				<option value="0">Nee</option>
				<option value="1">Ja - Knoppen</option>
				<option value="2">Ja - Slider</option>
			</select>
			<br>
		</div>
     
		<div id="homecode_div">
		<label for="name" >Home code: (A t/m P)</label>
		<input type="text" name="homecode" id="homecode" value=""  />
		<br>
		</div>
   
		<div id="adres_div">
		<div id="label_adres_newkaku">
		<label for="name">Adres: (1 t/m 255)</label>
		</div>
		<div id="label_adres_kaku">
		<label for="name">Adres: (1 t/m 16)</label>
		</div>
		<input type="text" name="address" id="address" value=""  />
		<br>
		</div>
     
		<div id="submit_div">
		<input type="submit" value="Toevoegen" >
		</div>
		
</form> 

	</div>

	<div data-role="collapsible">
		<h3>Aanpassen</h3>
		<?php
		// get results from database        
		mysql_select_db($database_tc, $tc);
		$result = mysql_query("SELECT * FROM nodo_tbl_devices WHERE user_id='$userId'") or die(mysql_error());  
				               
		echo '<ul data-role="listview" data-split-icon="delete" data-split-theme="a">';
  
		echo '<br>';   
		// loop through results of database query, displaying them in the table        
		while($row = mysql_fetch_array($result)) 
		{                                
		           
		echo '<li><a href="switch_edit.php?id=' . $row['id'] . '" title=Aanpassen data-ajax="false">'. $row['naam'] .'</a>';                
		echo '<a href="switch_delete_confirm.php?id=' . $row['id'] . '" data-rel="dialog">Verwijderen</a></li>';
		
		}         
		?>
	</div>
	
	</div><!-- /content -->
	
	<div data-role="footer">
		<h4></h4>
	</div><!-- /footer -->
	
</div><!-- /page -->


<script>	

$(document).ready(function() {

$('#name_div').hide();   
$('#dim_div').hide();   
$('#homecode_div').hide();
$('#adres_div').hide();
$('#submit_div').hide();

	
	
});


$('#type').change(function() 
{

if ($(this).attr('value')==1) {   

$('#name_div').show();  
$('#adres_div').show();
$('#dim_div').hide();
$('#homecode_div').show();
$('#submit_div').show();

$('#label_adres_kaku').show();
$('#label_adres_newkaku').hide();
}
   //alert('Value change to ' + $(this).attr('value'));
if ($(this).attr('value')==2) {   

$('#name_div').show();  
$('#adres_div').show();
$('#dim_div').show();
$('#homecode_div').hide();
$('#submit_div').show();

$('#label_adres_kaku').hide();
$('#label_adres_newkaku').show();

}
   
});
</script>

</body>
</html>

