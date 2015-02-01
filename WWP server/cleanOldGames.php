<?php
/*
This code clean the old games on our database
*/
require "connect_to_mysql.php";
$command = "SELECT * FROM `EncryptedHost`";
$res = $con->query($command) or die($con->error);
while($hostData = $res->fetch_assoc())
{
	$curId = $hostData['id'];
	$GameID = $hostData['GameID'];
	$now = time();
	if(abs($now - $hostData['Time'])>300)
	{
		$cmd = "DELETE FROM `EncryptedHost` WHERE id='$curId'";
		$con->query($cmd) or die($con->error); 
		$cmd = "DELETE FROM `HostGames` WHERE GameID='$GameID'";
		$con->query($cmd) or die($con->error); 			
	}
}
?>