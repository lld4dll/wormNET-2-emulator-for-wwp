<?php
/*
This page used to register new users
*/
	header('Content-type: text/html; charset=utf-8');
	require "connect_to_mysql.php";	
	
	function CheckEmail($email)
	{ 
		return filter_var($email, FILTER_VALIDATE_EMAIL);
	}
	
	function AddUser($user,$pass,$mail)
	{
		global $con;
		$user = $con->real_escape_string($user);
		$pass = $con->real_escape_string($pass);
		$mail = $con->real_escape_string($mail);	
		if(CheckEmail($mail) == false || strlen($user) < 3 || empty($pass) || strlen($pass) < 5)
			return 0;
		$command = "SELECT UserName FROM `USERS` WHERE UserName='$user'";
		$res = $con->query($command) or die($con->error);
		$userCnt = $res->num_rows;
		if($userCnt == 0)
		{
			$command = "INSERT INTO `USERS` (UserName, Password, Email)
									VALUES('$user','$pass','$mail')";
			$con->query($command) or die($con->error);	
			header("Error: User created");
			$folderPath = "wormsProject";			
			header("Location: /".$folderPath."/WelcomeLoginForm.php");		
			die();		
		}	
		else
		{
			header("Error: User is exist");
			$folderPath = "wormsProject";
			header("Location: /".$folderPath."/WelcomeLoginForm.php");		
			die();
		}
		return 1;
	}
	
	if(empty($_GET))
		echo "<SHOWNEWUSERENTRY>\n";
	else
	{
		$user = htmlspecialchars($_GET['Username']);
		$pass = htmlspecialchars($_GET['Password']);
		$mail = htmlspecialchars($_GET['Email']);
		if(AddUser($user,$pass,$mail) == 0)
		{		
			header("Error: Invalid data");
			$folderPath = "wormsProject";
			header("Location: /".$folderPath."/WelcomeLoginForm.php");	
			die();
		}
		/* used for debugging
		$fp = fopen('logs.txt', 'a');
		fwrite($fp,"CreateUserAccount.php\r\n");
		fwrite($fp, print_r($_GET, TRUE));
		fclose($fp);	
		*/
	}
	/*
	[Username] => d4d
    [Password] => 12345
    [Email] => sdsdsd
    [Surname] => abcddf
    [Address] => 
	*/
?>
 