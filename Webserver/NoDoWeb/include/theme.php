<?php

//Lees setup waarden uit de database
mysql_select_db($database_tc, $tc);
$RS_setup = mysql_query("SELECT * FROM nodo_tbl_setup WHERE user_id='$userId'") or die(mysql_error());  
$row_RSsetup = mysql_fetch_array($RS_setup);

//Thema ophalen uit database

$theme = $row_RSsetup['theme'];
$theme_header = $row_RSsetup['theme_header'];

?>