<?php
// connect to the server
$myuser = "gamers_wwp";
$mypass = "2NjZUQrL";
$mydb = "gamers_wwp";
$con = new mysqli('localhost', $myuser, $mypass, $mydb);

if ($con->connect_error) {
    die('Connect Error: ' . $con->connect_error);
}
$con->query("SET NAMES 'utf8'");
?>