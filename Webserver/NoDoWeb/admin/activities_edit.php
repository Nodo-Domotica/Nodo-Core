<?php 

require_once('../connections/tc.php'); 
require_once('../include/auth.php'); 
require_once('../include/settings.php'); 

$page_title="Setup: Edit activity";



 // check if the form has been submitted. If it has, process the form and save it to the database if 
 
 if (isset($_POST['submit'])) 
 {  
 // confirm that the 'id' value is a valid integer before getting the form data 
 if (is_numeric($_POST['id'])) 
 { 
 // get form data, making sure it is valid 
 $id = $_POST['id']; 
 $name = mysql_real_escape_string(htmlspecialchars($_POST['name'])); 
 $events = mysql_real_escape_string(htmlspecialchars($_POST['events']));  
 

 
  
 // save the data to the database 
 mysql_select_db($database_tc, $tc);
 mysql_query("UPDATE NODO_tbl_activities SET name='$name', events='$events' WHERE id='$id' AND user_id='$userId'") or die(mysql_error());   
 // once saved, redirect back to the view page 
 header("Location: activities.php#saved");  
 } 
 } 
 else {
 mysql_select_db($database_tc, $tc);
 $id = $_GET['id']; $result = mysql_query("SELECT * FROM nodo_tbl_activities WHERE id=$id AND user_id='$userId'") or die(mysql_error());  
 $row = mysql_fetch_array($result);  
 
  }?>

 <!DOCTYPE html> 
<html> 
 
<head>
	<meta charset="utf-8">
	<meta name="viewport" content="width=device-width, initial-scale=1"> 
	<title><?php echo $title;?></title> 
	<?php require_once('../include/jquery_mobile.php'); ?>
</head> 
 
<body> 

 
<div data-role="page">
 
	<?php require_once('../include/header_admin.php'); ?>
 
	<div data-role="content">	

	<form action="activities_edit.php" data-ajax="false" method="post"> 
	
	 
		
		<input type="hidden" name="id" id="id" value="<?php echo $row['id'] ;?>"  />
	
	<br>
	
		<br>
	
		<label for="name">Name: </label>
		<input type="text" name="name" id="name" value="<?php echo $row['name'] ;?>"  />
		<br>
		<label for="name">Event(s): (example: userevent 100,100;sendkaku a1,on)</label>
		<input type="text" name="events" id="events" value="<?php echo $row['events'] ;?>"  />
	
	   
        
		<input type="submit" name="submit" value="Edit" >

		
	
	</form> 

		
	</div><!-- /content -->
	
	<?php require_once('../include/footer_admin.php'); ?>
	
</div><!-- /page -->

<script>	

<?php if ($row['type'] == 1) {

echo "$('#label_adres_newkaku').hide();"; }

else {

echo "$('#label_adres_kaku').hide();"; }

?>




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

