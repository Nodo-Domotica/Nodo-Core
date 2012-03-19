<?php

require_once('include/webapp_settings.php');

session_start();


$_SESSION = array();

if (ini_get("session.use_cookies")) {
    $params = session_get_cookie_params();
    setcookie(session_name(), '', time() - 42000,
        $params["path"], $params["domain"],
        $params["secure"], $params["httponly"]
    );
}

 
 session_destroy();

 setcookie('username', '', time()-3600);
 setcookie('password', '', time()-3600);


 header("Location: login.php?logout=1");

?>
