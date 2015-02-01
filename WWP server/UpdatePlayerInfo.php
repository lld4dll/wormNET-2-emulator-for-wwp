<?php
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
			if($IPAddress != 0)
			{
				$command = "UPDATE `USERS` SET IPAddress = '$IPAddress' WHERE UserName='$user'";
				$con->query($command) or die($con->error);
			}
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

	$user = $_GET['Username'];
	$pass = $_GET['Password'];
	if(setUserInfo($_SESSION['Username'], $_SESSION['Password'], 0) == 1)
	{
		$_SESSION['Username'] = $user;
		$_SESSION['Password'] = $pass;
		$_SESSION['IPAddr'] = $_SERVER['REMOTE_ADDR'];
		$ircServer ="185.25.149.169"; // our IRC server
		$port = "6667";
		echo "<CONNECT ".$ircServer." IRCPORT=".$port." IRCUSER=".$user." IRCPASS=ELSILRACLIHP>";
	}
	else
	{
		$folderPath = "wormsProject";
		header("Error: Wrong Username or Password");
		header("Location: /".$folderPath."/WelcomeLoginForm.php");
	}
	/* used for debugging
	$fp = fopen('logs.txt', 'a');
	fwrite($fp,"UpdatePlayerInfo.php\r\n");
	fwrite($fp, print_r($_GET, TRUE));
	fclose($fp);
	*/
?>