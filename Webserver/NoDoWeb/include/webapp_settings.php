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


//$salt word gebruikt om er voor te zorgen dat ook slecht gekozen wachtwoorden zoals 1234, abc sterk worden gehashed
$salt = "xxxxxxx"; //aanpassen naar gewenste salt

$DEFAULT_NODO_PWD = "Nodo";
$DEFAULT_NODO_ID = "";
$WEBAPP_HOST = "www.nodo-domotica.nl/webapp";

//SQL injection prevention
//$_POST = array_map("mysql_real_escape_string", $_POST);
$_GET = array_map("mysql_real_escape_string", $_GET);
$_REQUEST = array_map("mysql_real_escape_string", $_REQUEST);

?>