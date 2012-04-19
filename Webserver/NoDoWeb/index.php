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

//Controleren of er ingelogd is. Hierna redirecten naar de door de gebruiker ingestelde standaard pagina.

require_once('connections/db_connection.php'); 



session_start();
if(!isset($_SESSION['userId'])) {
        //die('You need to be logged in!!!');
	header("Location: login.php");
} 

else {

require_once('include/auth.php');
require_once('include/user_settings.php');

$default_page = $row_RSsetup['default_page'];

	switch ($default_page) {

		case 1:
		header("Location: devices.php");
		break;
		case 2:
		header("Location: activities.php");
		break;
		case 3:
		header("Location: values.php");
		break;
		default:
		header("Location: devices.php");
		break;

	}

}
?>