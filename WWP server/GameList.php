<?php
/*
This page used to show all the games on wwp game
*/
	require "connect_to_mysql.php";
	header('Content-type: text/html; charset=utf-8');
	require "cleanOldGames.php";
	
	echo "<GAMELISTSTART>\r\n";		
	if(isset($_GET['Channel']))
	{		
		$channel = $_GET['Channel'];
		$command = "SELECT EncryptedGame from `EncryptedHost` WHERE Channel='$channel' ORDER BY GameID DESC";
		$res = $con->query($command) or die($con->error);
		while ($hostData = $res->fetch_assoc()) 
		{
			echo "<GAME ".$hostData['EncryptedGame'].">\r\n";
		}
		
	}
	echo "<GAMELISTEND>";	
?>