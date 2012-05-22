<?php
/***********************************************************************************************************************
"Nodo Web App" Copyright © 2012 Martin de Graaf

This program is free software: you can redistribute it and/or modify
it under the terms of the GNU Affero General Public License as
published by the Free Software Foundation, either version 3 of the
License, or (at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU Affero General Public License for more details.

You should have received a copy of the GNU Affero General Public License
along with this program.  If not, see <http://www.gnu.org/licenses/>.
*************************************************************************************************************************/
?>
<style type="text/css">

           .ui-icon-measurements {
            background-image: url(../media/measurements.png);
            background-repeat: no-repeat;

        }
		
			.ui-icon-devices {
            background-image: url(../media/devices.png);
            background-repeat: no-repeat;

        }

			.ui-icon-measurements, .ui-icon-devices {
			background-position: 0 50%;

       }

</style>

	<div data-role="header" data-theme="<?php echo $theme_header?>">
		<h1><?php echo $page_title;?></h1>
		<div data-role="navbar" data-iconpos="top">
		<ul>
			<li><a href="../devices.php" data-icon="devices" data-ajax="false">Devices</a></li>
			<li><a href="../activities.php" data-icon="grid" data-ajax="false">Activities</a></li>
			<li><a href="../values.php" data-icon="measurements" data-ajax="false">Values</a></li>
		</ul>
		</div>
	</div><!-- /header -->