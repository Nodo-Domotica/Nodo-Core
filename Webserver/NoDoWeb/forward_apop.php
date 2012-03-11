<?php

require_once('connections/tc.php'); 
require_once('include/auth.php'); 
require_once('include/user_settings.php'); 
  
function sendMsg($args)
{
    $fp = fsockopen($args["host"],$args["port"], $errno, $errstr, 10);
    
    if (!$fp)
    { //Something didn't work....
        echo "ERROR: $errno - $errstr\n";
        return FALSE;

    } else {

        // First wake up the server, for security reasons it does not
        // respond by it self it needs this string, why this odd word ?
        // well if someone is scanning ports "connect" would be very obvious
        // this one you'd never guess :-)
        
        fputs($fp, "quintessence\n\r");
        
        // The server now returns a cookie, the protocol works like the
        // APOP protocol. The server gives you a cookie you add :<password>
        // calculate the md5 digest out of this and send it back
        // if the digests match you are in.
        // We do this so that noone can listen in on our password exchange
        // much safer then plain text.
        
        $cookie = fgets($fp, 400);      
        
        // Trim all enters and whitespaces off
        $cookie = trim($cookie);
        
        // Combine the token <cookie>:$args[pword]
        $token = $cookie . ":" . $args["password"];
        
        // Calculate the digest
        $digest = md5($token);
        
        // add the enters
        $digest = $digest . "\n";
                
        // Send it to the server      
        fputs($fp, $digest );
        
        // Get the answer
        $res = fgets($fp, 400);
        
        // If the password was correct and you are allowed to connect
        // to the server, you'll get "accept"
        if ( trim($res) != "accept" )
        {
             fclose($fp);
             return FALSE;
        }
        
        if (isset($args['payload']))
        {
            if (is_array($args['payload']))
            {
                foreach($args['payload'] as $key => $value)
                    fputs($fp, "payload " . $value . "\n");
            }else
            {
                if ( $args["payload"] <> "" )
                    fputs($fp, "payload ".$args["payload"]."\n");      
            }      
        }
        // now just pipe those commands to the server
        fputs($fp, $args["eventstring"]."\n");
        
        // tell the server that we are done nicely.
        fputs($fp, "close\n");
        
        fclose($fp);
            
        return TRUE;
    }
}


if (isset($_GET))
{   
    if(isset($_GET['host']))
        $args['host'] = urldecode($_GET['host']);
    else
        $args['host'] = $nodo_ip;
    $args['port'] = $nodo_port;
    $args['password'] = $nodo_password;
    $args['eventstring'] =  urldecode($_GET['event']);
    foreach($_GET as $key => $value)
        if(strcasecmp(substr($key, 0, 3), 'pld') == 0)
            $args['payload'][] = urldecode($value);
    sendMsg($args);
    //if(!isset($_GET['REFERER']))
    //    $_GET['REFERER'] = "index.php";
    //header("Location: http://".$_SERVER['HTTP_HOST']
    //       . dirname($_SERVER['PHP_SELF'])
    //       ."/".$_GET["REFERER"]);
}
?>

