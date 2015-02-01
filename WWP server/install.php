<?php
/*
This is our installation for the WWP server, There is admin panel to set more info such as channels
*/
	header('Content-type: text/html; charset=utf-8');	
	require "connect_to_mysql.php";
	require "admin/errMsg.php";	
	
	$success = "";
	function CheckEmail($email)
	{ 
		return filter_var($email, FILTER_VALIDATE_EMAIL);
	}	
	
	function CreateUsersTable()
	{
		global $con;
		$command = "CREATE TABLE IF NOT EXISTS `USERS`  (
					`UserID` int(11) NOT NULL auto_increment,
					`PlayerType`  tinyint(2) NOT NULL default 0,
					`PlayerScore` int(8) NOT NULL default 0,
					`PlayerLevel`  tinyint(2) NOT NULL default 0,
					`Banned`  boolean NOT NULL DEFAULT 0,
					`UserName`  varchar(20) NOT NULL default '0',
					`Password`  varchar(255) NOT NULL,
					`IPAddress` varchar(255) NOT NULL default '0',
					`Email` varchar(255) NOT NULL default '',
					`Access` boolean NOT NULL DEFAULT  '0',
					 PRIMARY KEY  (`UserID`),
					UNIQUE KEY `UserName` (`UserName`),
					UNIQUE KEY `Email` (`Email`)
					)ENGINE=MyISAM  DEFAULT CHARSET=utf8";
		$con->query($command) or die($con->error);		
	}
	
	function AddAdminUser($user, $pass, $mail, $playerType, $access)
	{
		global $con;
		$user = htmlspecialchars($con->real_escape_string($user));
		$pass = $con->real_escape_string($pass);
		$mail = htmlspecialchars($con->real_escape_string($mail));	
		if(CheckEmail($mail) == false || strlen($user) < 3 || empty($pass) || strlen($pass) < 5)
			return 0;
		$command = "SELECT UserID FROM `USERS`";
		$res = $con->query($command) or die($con->error);
		$userCnt = $res->num_rows;
		if($userCnt == 0)
		{
			$command = "INSERT INTO `USERS` (UserName, Password, Email,PlayerType, Access)
									VALUES('$user', '$pass', '$mail', '$playerType', '$access')";
			$con->query($command) or die($con->error);		
		}	
		else
		{
			setInstallErrMsg("User is exist", "system has been installed", "install.php", "install.php can be removed");
			die("");
		}
		$success .= "USERS table created<br/>";
		return 1;
	}
	
	function CreateHostGamesTable()
	{
		global $con;
		$command = "CREATE TABLE IF NOT EXISTS `HostGames`  (
					`GameID` int(15) NOT NULL auto_increment,
					`UserID` int(15) NOT NULL,
					`GuestID` int(15) NOT NULL,
					`Nick`  varchar(20) NOT NULL default '',
					`HostIP` varchar(255) NOT NULL default '',
					`Name` varchar(20) NOT NULL default '',
					`Loc`  tinyint(2) NOT NULL default 0,
					`Type`  tinyint(2) NOT NULL default 0,
					`ServerUsage` boolean NOT NULL DEFAULT  1,
					`Pwd` boolean NOT NULL DEFAULT  0,
					`Chan`  varchar(255) NOT NULL default '',
					`Time` int(15),
					PRIMARY KEY  (`GameID`)
					)ENGINE=MyISAM  DEFAULT CHARSET=utf8";
		$con->query($command) or die($con->error);	
		$success .= "HostGames table created<br/>";
	}
	
	function CreateEncryptdGamesTable()
	{
		global $con;
		$command = "CREATE TABLE IF NOT EXISTS `EncryptedHost`  (
					`id` int(15) NOT NULL auto_increment,
					`EncryptedGame`  varchar(512) NOT NULL default '',
					`GameID` int(15) NOT NULL,
					`Channel` varchar(255) NOT NULL default '',
					`Time` int(15),
					PRIMARY KEY  (`id`)
					)ENGINE=MyISAM  DEFAULT CHARSET=utf8";
		$con->query($command) or die($con->error);		
	$success .= "EncryptdGames table created<br/>";		
	}
	
	function CreateSchemesRequest()
	{
		global $con;
		$command = "CREATE TABLE IF NOT EXISTS `Schemes`  (
					`id` int(15) NOT NULL auto_increment,
					`Channel` varchar(255) NOT NULL default '',
					`modes` TEXT NOT NULL default '',
					PRIMARY KEY  (`id`),
					UNIQUE KEY (`Channel`)
					)ENGINE=MyISAM  DEFAULT CHARSET=utf8";
		$con->query($command) or die($con->error);		
	$success .= "Schemes table created<br/>";		
	}

	
	if(isset($_POST['check']))
	{
		
		if(isset($_POST['UserName']) && isset($_POST['Pwd']) && isset($_POST['Pwd1']) && isset($_POST['mail']))
		{		
			$user = htmlspecialchars($_POST['UserName']);
			$pass = $_POST['Pwd'];
			$cpass = $_POST['Pwd1'];
			$mail = htmlspecialchars($_POST['mail']);	
			if (strcmp($pass,$cpass) == 0)
			{
				CreateUsersTable();
				if(AddAdminUser($user, $pass, $mail, 2, 1))
				{
					CreateHostGamesTable();
					CreateEncryptdGamesTable();
					CreateSchemesRequest();
					setInstallSucessMsg($success, "System is installed", "Click Below to Login");
					die("");
				}
				else
				{
					setInstallErrMsg("Mail or password or user are invalid", "Invalid info data", "install.php", "try again to set valid info");
					die("");
				}
			}
			else
			{
				setInstallErrMsg("Passwords are not matched", "passwords are not matched", "install.php", "try again to set valid password");
				die("");
			}
		}
		else
		{
			setInstallErrMsg("One of the fields is empty", "fields are empty", "install.php", "Try again to set valid info");
			die("");
		}
	}
	
?>

<!DOCTYPE html PUBLIC "-//W3C//DTD XHTML 1.0 Transitional//EN"
"http://www.w3.org/TR/xhtml1/DTD/xhtml1-transitional.dtd">

<html xmlns="http://www.w3.org/1999/xhtml">
<head>
	<meta http-equiv="Content-type" content="text/html; charset=UTF-8" />
	<link rel="stylesheet" type="text/css" href="admin/style.css" />
	<title>Install System</title>
</head>
<body>
	<div class="modesItem">
		<div class="header">
		Install WormNET2 System
		</div>	
		<div class="modesInfoDialog">
			<form method="post" action="">
			<div>
				<div style="padding-top:1%;width:15%;float:left;clear:both;">
				<label>UserName:</label>
				</div>
				<div style="padding-top:1%;padding-left:4%;float:left;">
					<input type="text" name="UserName" />
				</div>
			</div>		
			<div>
				<div style="padding-top:1%;width:15%;float:left;clear:both;">
				<label>Email:</label>
				</div>
				<div style="padding-top:1%;padding-left:4%;float:left;">
					<input type="text" name="mail" value="<?php echo $userData['Email'];?>"/>
				</div>
			</div>						
			<div>
				<div style="padding-top:1%;width:15%;float:left;clear:both;">
				<label>Password:</label>
				</div>
				<div style="padding-top:1%;padding-left:4%;float:left;">
					<input type="password" name="Pwd" />
				</div>
			</div>			
			<div>
				<div style="padding-top:1%;width:15%;float:left;clear:both;">
				<label>Retype Password:</label>
				</div>
				<div style="padding-top:1%;padding-left:4%;float:left;">
					<input type="password" name="Pwd1" />
				</div>
			</div>																									
			<div>
				<div style="float:left;clear:both;">
					<input type="hidden" name="check" value="1">
					<input type="submit" value="Install system">		
				</div>
			</div>						
			</form>
		</div>
	</div>
</body>
</html>