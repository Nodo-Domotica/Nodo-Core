

<style type="text/css">



           .ui-icon-measurements {

            background-image: url(media/measurements.png);

            background-repeat: no-repeat;

        }
		
		.ui-icon-devices {

            background-image: url(media/devices.png);
            background-repeat: no-repeat;

        }

        .ui-icon-measurements, .ui-icon-devices {

            background-position: 0 50%;

       }

</style>

	<div data-role="header" data-theme="<?php echo $theme_header?>">
		<h1><?php echo $page_title;  ?></h1>
		<div data-role="navbar" data-iconpos="top">
		<ul>
			<li><a href="index.php" data-icon="grid"  data-ajax="false">Devices</a></li>
			<li><a href="activities.php" data-icon="grid" data-ajax="false">Activities</a></li>
			<li><a href="measurements.php" data-icon="measurements" data-ajax="false">Measurements</a></li>
		</ul>
		</div>
	</div><!-- /header -->