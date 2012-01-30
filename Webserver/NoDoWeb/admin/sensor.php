<?php require_once('../Connections/tc.php'); ?>

<!DOCTYPE html> 
<html> 
 
<head>
	<meta charset="utf-8">
	<meta name="viewport" content="width=device-width, initial-scale=1"> 
	<title>Sensors aanpassen/toevoegen</title> 
	<?php require_once('../include/jquery_mobile.php'); ?>
</head> 
 
<body> 

 
<div data-role="page">
 
	<div data-role="header">
		<h1>Sensors aanpassen/toevoegen</h1>
		<a href="/admin/index.php" data-icon="gear" class="ui-btn-right" data-ajax="false">Setup</a>
		<a href="/index.php" data-icon="home" class="ui-btn-left" data-ajax="false" data-iconpos="notext">Home</a>
	</div><!-- /header -->
 
	<div data-role="content">	

<form action="switch_add.php" data-ajax="false" method="post"> 
		 
	<div data-role="collapsible">
			
		<h3>Toevoegen</h3>
			

	<br>
	
		<label for="name">Sensor naam:</label>
		<input type="text" name="naam" id="naam" value=""  />
	
		
	<br>
	
		<label for="factor">Factor:</label>
		<input type="text" name="factor" id="factor" value="1"  />
	
		
	<br>
	
		<label for="offset">Offset:</label>
		<input type="text" name="offset" id="offset" value="0"  />
	
	<br>
	
		
		<label for="afronding">Afronding:</label>
		<input type="text" name="afronding" id="afronding" value="1"  />
	
	<br>
	Uit te lezen waarde:
	<fieldset data-role="controlgroup" data-type="horizontal">
	<label for="select-choice-1" class="ui-hidden-accessible">Unit nr.</label>
		    <select name="unit" id="unit" data-native-menu="false" >
				<option value="0" data-placeholder="true">Unit nr.</option>
				<option value="1">Unit 1</option>
				<option value="2">Unit 2</option>
				<option value="3">Unit 3</option>
				<option value="4">Unit 4</option>
				<option value="5">Unit 5</option>
				<option value="6">Unit 6</option>
				<option value="7">Unit 7</option>
				<option value="8">Unit 8</option>
				<option value="9">Unit 9</option>
				<option value="10">Unit 10</option>
				<option value="11">Unit 11</option>
				<option value="12">Unit 12</option>
				<option value="13">Unit 13</option>
				<option value="14">Unit 14</option>
				<option value="15">Unit 15</option>
			</select>
	
	    
	 <label for="select-choice-2" class="ui-hidden-accessible" >Parameter:</label>
		    <select name="parameter" id="parameter" data-native-menu="false" >
				<option value="0" data-placeholder="true">Parameter</option>
				<option value="wiredanalog">WiredAnalog</option>
				<option value="variable">Variabele</option>
			</select>
			
			<label for="select-choice-2" class="ui-hidden-accessible" >Parameter nr.</label>
		    <select name="parameter_nr" id="parameter_nr" data-native-menu="false" >
				<option value="0" data-placeholder="true">Parameter nr.</option>
				<option value="1">1</option>
				<option value="2">2</option>
				<option value="3">3</option>
				<option value="4">4</option>
				<option value="5">5</option>
				<option value="6">6</option>
				<option value="7">7</option>
				<option value="8">8</option>
				<option value="9">9</option>
				<option value="10">10</option>
				<option value="11">11</option>
				<option value="12">12</option>
				<option value="13">13</option>
				<option value="14">14</option>
				<option value="15">15</option>
			</select>
			
	 </fieldset>
	 	
	<br>
	<input type="submit" value="Sensor toevoegen" >

</form> 

	</div>

	<div data-role="collapsible">
		<h3>Aanpassen</h3>
		<?php
		// get results from database        
		mysql_select_db($database_tc, $tc);
		$result = mysql_query("SELECT * FROM NODO_tbl_switch") or die(mysql_error());  
				               
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



</body>
</html>

