<?php require_once('connections/tc.php'); ?>
<?php require_once('include/auth.php'); ?>
<?php require_once('include/theme.php'); ?>



<!DOCTYPE html> 
<html> 

<head>
	<meta charset="utf-8">
	<meta name="viewport" content="width=device-width, initial-scale=1"> 
	<title>Metingen</title> 
	<link rel="stylesheet" href="http://code.jquery.com/mobile/1.0/jquery.mobile-1.0.min.css" />
	<script src="http://code.jquery.com/jquery-1.6.4.min.js"></script>
	<script src="http://code.jquery.com/mobile/1.0/jquery.mobile-1.0.min.js"></script>
	 <script language="javascript" type="text/javascript" src="js/flot/jquery.js"></script>
    <script language="javascript" type="text/javascript" src="js/flot/jquery.flot.js"></script>
</head> 

<body> 

<div data-role="page" data-theme="<?php echo $theme?>">

	<div data-role="header" data-theme="<?php echo $theme_header?>">
		<h1>Metingen</h1>
		<div data-role="navbar">
		<ul>
			<li><a href="index.php" data-icon="grid" data-ajax="false">Apparaten</a></li>
			<li><a href="activities.php" data-icon="grid" data-ajax="false">Activiteiten</a></li>
			<li><a href="measurements.php" data-icon="info" data-ajax="false">Metingen</a></li>
		</ul>
		</div>
	</div><!-- /header -->

	<div data-role="content">	
				
		<div id="placeholder" style="max-width:1024px;height:300px"></div>
		
		<script type="text/javascript">
$(function () {
    var d1 = [];
    for (var i = 0; i < 14; i += 0.5)
        d1.push([i, Math.sin(i)]);

    var d2 = [[0, 3], [4, 8], [8, 5], [9, 13]];

    var d3 = [];
    for (var i = 0; i < 14; i += 0.5)
        d3.push([i, Math.cos(i)]);

    var d4 = [];
    for (var i = 0; i < 14; i += 0.1)
        d4.push([i, Math.sqrt(i * 10)]);
    
    var d5 = [];
    for (var i = 0; i < 14; i += 0.5)
        d5.push([i, Math.sqrt(i)]);

    var d6 = [];
    for (var i = 0; i < 14; i += 0.5 + Math.random())
        d6.push([i, Math.sqrt(2*i + Math.sin(i) + 5)]);
                        
    $.plot($("#placeholder"), [
        {
            data: d1,
            lines: { show: true, fill: true }
        },
        {
            data: d2,
            bars: { show: true }
        },
        {
            data: d3,
            points: { show: true }
        },
        {
            data: d4,
            lines: { show: true }
        },
        {
            data: d5,
            lines: { show: true },
            points: { show: true }
        },
        {
            data: d6,
            lines: { show: true, steps: true }
        }
    ]);
});
</script>


	</div><!-- /content -->
	
	<div data-role="footer" data-theme="<?php echo $theme_header?>">
		
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