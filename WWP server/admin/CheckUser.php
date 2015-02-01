<?php
	require "../connect_to_mysql.php";	
	session_start();
	$isAdmin = 0;
	$user = $con->real_escape_string($_SESSION['Username']);
	$pass = $con->real_escape_string($_SESSION['Password']);
	$command = "SELECT Access FROM `USERS` WHERE UserName = '$user' AND Password = '$pass'";
	$res = $con->query($command) or die($con->error);
	$userCnt = $res->num_rows;
	if($userCnt == 1)
	{
		$userData = $res->fetch_assoc();
		$access = $userData['Access'];			
	}
	if($access == 1)
		$isAdmin = 1;
	else
		$isAdmin = 0;		
?>	