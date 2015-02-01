<?php
/*
This page used for log into wwp server from WWP
*/
	session_start();
	header('Content-type: text/html; charset=utf-8');
	require "connect_to_mysql.php";
	
	function setUserInfo($user, $pass, $IPAddress)
	{
		global $con;
		$user = $con->real_escape_string($user);
		$pass = $con->real_escape_string($pass);
		$IPAddress = $con->real_escape_string($IPAddress);
		$command = "SELECT * FROM `USERS` WHERE UserName='$user' and Password='$pass'";
		$res = $con->query($command) or die($con->error);
		$userCnt = $res->num_rows;
		if($userCnt == 1)
		{
			$UserData = $res->fetch_assoc();
			header("SetUserId: ".$UserData['UserID']);
			header("SetPlayerType: ".$UserData['PlayerType']);
			header("PlayerLevel: ".$UserData['PlayerLevel']);
			header("SetPlayerScore: ".$UserData['PlayerScore']);
			$command = "UPDATE `USERS` SET IPAddress = '$IPAddress' WHERE UserName='$user'";
			$con->query($command) or die($con->error);
		}
		else
			return 0;
		if($UserData['Banned'] == 1)
		{
			header("Error: User is banned");
			$folderPath = "wormsProject";
			header("Location: /".$folderPath."/WelcomeLoginForm.php");
			die();
			return 0;
		}
		return 1;
	}

	$user = htmlspecialchars($_GET['UserName']);
	$pass = $_GET['Password'];
	$ipAddr = htmlspecialchars($_GET['IPAddress']);
	if(setUserInfo($user, $pass, $ipAddr) == 1)
	{
		$_SESSION['Username'] = $user;
		$_SESSION['Password'] = $pass;
		$_SESSION['IPAddr'] = $_SERVER['REMOTE_ADDR'];
		$ircServer ="185.25.149.169"; //"our IRC server";
		$port = "6667";
		echo "<CONNECT ".$ircServer." IRCPORT=".$port." IRCUSER=".$user." IRCPASS=ELSILRACLIHP>";
	}
	else
	{
		header("Error: Wrong Username or Password");
		$folderPath = "wormsProject";
		header("Location: /".$folderPath."/WelcomeLoginForm.php");
	}
	/*	for debugging usage
	$fp = fopen('logs.txt', 'a');
	fwrite($fp,"Login.php\r\n");
	fwrite($fp, print_r($_GET, TRUE));
	fclose($fp);
	*/
?>