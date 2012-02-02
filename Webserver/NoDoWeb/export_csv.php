<?php 
require_once('connections/tc.php');
require_once('include/auth.php');

$RSevent_log = mysql_query("SELECT * FROM nodo_tbl_event_log WHERE user_id='$userId'") or die(mysql_error());
    header("Content-Type: text/csv; charset=utf-8");
    header("Content-Disposition:attachment;filename=events.csv");
    while($row = mysql_fetch_row($RSevent_log)) {
    print '"' . stripslashes(implode('","',$row)) . "\"\n";
    }
    exit;




?>