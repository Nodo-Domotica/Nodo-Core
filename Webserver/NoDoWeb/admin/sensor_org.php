<?php require_once('../Connections/tc.php'); ?><?php
if (!function_exists("GetSQLValueString")) {
function GetSQLValueString($theValue, $theType, $theDefinedValue = "", $theNotDefinedValue = "") 
{
  $theValue = get_magic_quotes_gpc() ? stripslashes($theValue) : $theValue;

  $theValue = function_exists("mysql_real_escape_string") ? mysql_real_escape_string($theValue) : mysql_escape_string($theValue);

  switch ($theType) {
    case "text":
      $theValue = ($theValue != "") ? "'" . $theValue . "'" : "NULL";
      break;    
    case "long":
    case "int":
      $theValue = ($theValue != "") ? intval($theValue) : "NULL";
      break;
    case "double":
      $theValue = ($theValue != "") ? "'" . doubleval($theValue) . "'" : "NULL";
      break;
    case "date":
      $theValue = ($theValue != "") ? "'" . $theValue . "'" : "NULL";
      break;
    case "defined":
      $theValue = ($theValue != "") ? $theDefinedValue : $theNotDefinedValue;
      break;
  }
  return $theValue;
}
}

$editFormAction = $_SERVER['PHP_SELF'];
if (isset($_SERVER['QUERY_STRING'])) {
  $editFormAction .= "?" . htmlentities($_SERVER['QUERY_STRING']);
}

if ((isset($_POST["MM_insert"])) && ($_POST["MM_insert"] == "form1")) {
  $insertSQL = sprintf("INSERT INTO tbl_sensor (sensor_type, sensor_naam, sensor_factor, sensor_offset, sensor_parameter, sensor_eenheid, sensor_afronding) VALUES (%s, %s, %s, %s, %s, %s, %s)",
                       GetSQLValueString($_POST['sensor_type'], "int"),
                       GetSQLValueString($_POST['sensor_naam'], "text"),
                       GetSQLValueString($_POST['sensor_factor'], "double"),
                       GetSQLValueString($_POST['sensor_offset'], "double"),
                       GetSQLValueString($_POST['sensor_parameter'], "text"),
                       GetSQLValueString($_POST['sensor_eenheid'], "text"),
                       GetSQLValueString($_POST['sensor_afronding'], "int"));

  mysql_select_db($database_tc, $tc);
  $Result1 = mysql_query($insertSQL, $tc) or die(mysql_error());
}

mysql_select_db($database_tc, $tc);
$query_RSsensor = "SELECT * FROM NODO_tbl_sensor";
$RSsensor = mysql_query($query_RSsensor, $tc) or die(mysql_error());
$row_RSsensor = mysql_fetch_assoc($RSsensor);
$totalRows_RSsensor = mysql_num_rows($RSsensor);

mysql_select_db($database_tc, $tc);
$query_RSsensortype = "SELECT * FROM NODO_tbl_sensor_type";
$RSsensortype = mysql_query($query_RSsensortype, $tc) or die(mysql_error());
$row_RSsensortype = mysql_fetch_assoc($RSsensortype);
$totalRows_RSsensortype = mysql_num_rows($RSsensortype);

mysql_select_db($database_tc, $tc);
$query_RSsensorparameters = "SELECT * FROM NODO_tbl_sensor_parameters";
$RSsensorparameters = mysql_query($query_RSsensorparameters, $tc) or die(mysql_error());
$row_RSsensorparameters = mysql_fetch_assoc($RSsensorparameters);
$totalRows_RSsensorparameters = mysql_num_rows($RSsensorparameters);
?>
<script language="JavaScript" type="text/JavaScript">
<!--
function BRB_PHP_DelWithCon(deletepage_url,field_name,field_value,messagetext) { //v1.0 - Deletes a record with confirmation
  if (confirm(messagetext)==1){
  	location.href = eval('\"'+deletepage_url+'?'+field_name+'='+field_value+'\"');
  }
}
//-->
</script><!DOCTYPE html PUBLIC "-//W3C//DTD XHTML 1.0 Transitional//EN" "http://www.w3.org/TR/xhtml1/DTD/xhtml1-transitional.dtd">
<html xmlns="http://www.w3.org/1999/xhtml">
<head>
<meta http-equiv="Content-Type" content="text/html; charset=utf-8" />
<title>Sensoren toevoegen / aanpassen</title>
<link href="../CSS/style.css" rel="stylesheet" type="text/css" />
<style type="text/css">
<!--
.stijl2 {color: #CCCCCC}
-->
</style>
</head>

<body>
<table width="750" border="0" align="center" cellpadding="25" cellspacing="0" bgcolor="#000000">
  <tr>
    <th scope="col"><h2 align="left">Sensoren</h2>
      <table width="100%" border="0" cellpadding="0" cellspacing="0">
      <tr class="Text">
        <th bgcolor="#666666" scope="col"><span class="stijl2">Type</span></th>
        <th bgcolor="#666666" scope="col"><span class="stijl2">Naam</span></th>
        <th bgcolor="#666666" scope="col"><span class="stijl2">Factor</span></th>
        <th bgcolor="#666666" scope="col"><span class="stijl2">Offset</span></th>
        <th bgcolor="#666666" scope="col"><span class="stijl2">Parameter</span></th>
        <th bgcolor="#666666" scope="col"><span class="stijl2">Eenheid</span></th>
        <th bgcolor="#666666" scope="col"><span class="stijl2">Afronding</span></th>
        <th bgcolor="#666666" scope="col">&nbsp;</th>
      </tr>
      <?php do { ?>
      <tr bgcolor="#333333" class="Text">
        <th scope="col"><?php echo $row_RSsensor['sensor_type']; ?></th>
        <th scope="col"><?php echo $row_RSsensor['sensor_naam']; ?></th>
        <th scope="col"><?php echo $row_RSsensor['sensor_factor']; ?></th>
        <th scope="col"><?php echo $row_RSsensor['sensor_offset']; ?></th>
        <th scope="col"><?php echo $row_RSsensor['sensor_parameter']; ?></th>
        <th scope="col"><?php echo $row_RSsensor['sensor_eenheid']; ?></th>
        <th scope="col"><?php echo $row_RSsensor['sensor_afronding']; ?></th>
        <th scope="col"><a href="javascript:BRB_PHP_DelWithCon('sensor_delete.php','id',<?php echo $row_RSsensor['id']; ?>,'Zeker weten?');">verwijderen</a></th>
      </tr>
      <?php } while ($row_RSsensor = mysql_fetch_assoc($RSsensor)); ?>
    </table>
      <p>&nbsp;</p>
      <form action="<?php echo $editFormAction; ?>" method="post" name="form1" id="form1">
        <table align="left" cellspacing="5" bgcolor="#333333">
          <tr valign="baseline">
            <td align="right" nowrap="nowrap" class="Text"><div align="left">Type:</div></td>
            <td><div align="left">
              <select name="sensor_type">
                <?php
do {  
?>
                <option value="<?php echo $row_RSsensortype['sensor_id']?>"><?php echo $row_RSsensortype['sensor_type']?></option>
                <?php
} while ($row_RSsensortype = mysql_fetch_assoc($RSsensortype));
  $rows = mysql_num_rows($RSsensortype);
  if($rows > 0) {
      mysql_data_seek($RSsensortype, 0);
	  $row_RSsensortype = mysql_fetch_assoc($RSsensortype);
  }
?>
              </select>
            </div></td>
          </tr>
          <tr valign="baseline">
            <td align="right" nowrap="nowrap" class="Text"><div align="left">Naam:</div></td>
            <td><div align="left">
              <input type="text" name="sensor_naam" value="" size="32" />
            </div></td>
          </tr>
          <tr valign="baseline">
            <td align="right" nowrap="nowrap" class="Text"><div align="left">Factor:</div></td>
            <td><div align="left">
              <input type="text" name="sensor_factor" value="1" size="32" />
            </div></td>
          </tr>
          <tr valign="baseline">
            <td align="right" nowrap="nowrap" class="Text"><div align="left">Offset:</div></td>
            <td><div align="left">
              <input type="text" name="sensor_offset" value="0" size="32" />
            </div></td>
          </tr>
          <tr valign="baseline">
            <td align="right" nowrap="nowrap" class="Text"><div align="left">Parameter:</div></td>
            <td><div align="left">
              <select name="sensor_parameter" id="sensor_parameter">
                <?php
do {  
?>
                <option value="<?php echo $row_RSsensorparameters['parameter']?>"><?php echo $row_RSsensorparameters['parameter']?></option>
                <?php
} while ($row_RSsensorparameters = mysql_fetch_assoc($RSsensorparameters));
  $rows = mysql_num_rows($RSsensorparameters);
  if($rows > 0) {
      mysql_data_seek($RSsensorparameters, 0);
	  $row_RSsensorparameters = mysql_fetch_assoc($RSsensorparameters);
  }
?>
              </select>
            </div></td>
          </tr>
          <tr valign="baseline">
            <td align="right" nowrap="nowrap" class="Text"><div align="left">Eenheid:</div></td>
            <td><label>
              <div align="left">
                <select name="sensor_eenheid" id="sensor_eenheid_id">
                  <option value="°C">°C</option>
                  <option value=" " selected="selected">Geen</option>
                </select>              
                </label>
            </div></td>
          </tr>
          <tr valign="baseline">
            <td align="right" nowrap="nowrap" class="Text"><div align="left">Afronding:</div></td>
            <td><div align="left">
              <input type="text" name="sensor_afronding" value="1" size="32" />
            </div></td>
          </tr>
          <tr valign="baseline">
            <td align="right" nowrap="nowrap" class="Text"><div align="left"></div></td>
            <td><div align="left">
              <input type="submit" value="Sensor invoegen" />
            </div></td>
          </tr>
        </table>
        <input type="hidden" name="MM_insert" value="form1" />
      </form>
    <p>&nbsp;</p>
    <p>&nbsp;</p>
    <p>&nbsp;</p>
    <p>&nbsp;</p>
    <p>&nbsp;</p>
    <p>&nbsp;</p>
    <p>&nbsp;</p>
    <p align="left"><a href="index.php">Terug</a></p></th>
  </tr>
</table>
</body>
</html>
<?php
mysql_free_result($RSsensor);

mysql_free_result($RSsensortype);

mysql_free_result($RSsensorparameters);
?>
