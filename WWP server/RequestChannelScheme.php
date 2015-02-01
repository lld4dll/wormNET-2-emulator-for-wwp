<?php
/*
This page set the modes on our channel based on the data stored in the database
*/
session_start();
header('Content-type: text/html; charset=utf-8');
require "connect_to_mysql.php";

header("UserLevel: 0");
header("UserServerIdent: 1");
header("Counter:". time());

$channel = $_GET['Channel'];
$channel = $con->real_escape_string($channel);
$command = "SELECT modes FROM `Schemes` WHERE Channel='$channel'";
$res = $con->query($command) or die($con->error);
$cnt = $res->num_rows;
// if channel is not on the list, AnythingGoes modes would be set by default
if($cnt == 1)
{
	$ChannelData = $res->fetch_assoc();
	echo $ChannelData['modes'];
}
else
{
	echo "<SCHEME=Pf,Be>\n";
}
$fp = fopen('logs.txt', 'a');
fwrite($fp,"RequestChannelScheme.php\r\n");
fwrite($fp, print_r($_GET, TRUE));
fclose($fp);
/*	
modes=(ha) for item:46 - crate shower
modes=(fs) for item:44 - crate spy
modes=(fq) for item:42 - invisible
modes=(eq) for item:38 - armageddon
modes=(cq) for item:24 - girder pack
modes=(fb) for item:33 - carpet bomb
modes=(dy) for item:36 - donkey
modes=(ba) for item:0a - earth quake
modes=(ga) for item:3c - freeze
modes=(gb) for item:3d - magic bullet
modes=(dq) for item:2e - MB bomb
modes=(da) for item:1e - mine strike
modes=(dt) for item:31 - Ming Vase
modes=(db) for item:1f - Mole Squadron
modes=(dz) for item:37 - Nuclear Test
modes=(bt) for item:1d - Mail Strike
modes=(cz) for item:2d - Salvation Army
modes=(eb) for item:29 - Scales Of Justice
modes=(et) for item:3b - select worm
modes=(fa) for item:32 - sheep strike
modes=(bj) for item:13 - Suicide Bomber
modes=(ec) for item:2a - super banana bomb
*/
?>

