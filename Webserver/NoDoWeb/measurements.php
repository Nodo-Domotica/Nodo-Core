<?php 
require_once('connections/tc.php'); 
require_once('include/auth.php'); 
require_once('include/settings.php'); 

$page_title = "Metingen";

?>



<!DOCTYPE html> 
<html> 

<head>
	<meta charset="utf-8">
	<meta name="viewport" content="width=device-width, initial-scale=1"> 
	<title><?php echo $title ?></title> 
	<?php require_once('include/jquery_mobile.php'); ?>
	
</head> 

<body> 

<div data-role="page" data-theme="<?php echo $theme?>">

<?php require_once('include/header.php'); ?>

	<div data-role="content">	
	
	
		

</div><!-- /content -->
<?php require_once('include/footer.php'); ?>
	
</div><!-- /page -->

</body>
</html>