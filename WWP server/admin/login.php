<?php
	session_start();
	header('Content-type: text/html; charset=utf-8');
	require "../connect_to_mysql.php";	
	
	function IsUserAdminLogged($user, $pass)
	{
		global $con;
		$access = 0;
		$command = "SELECT Access FROM `USERS` WHERE UserName = '$user' AND Password = '$pass'";
		$res = $con->query($command) or die($con->error);
		$userCnt = $res->num_rows;
		if($userCnt == 1)
		{
			$userData = $res->fetch_assoc();
			$_SESSION['Username'] = $user;
			$_SESSION['Password'] = $pass;
			$access = $userData['Access'];			
		}
		if($access == 1)
			return 1;	
		else
			return 0;		
	}
	
	$err = "";
	
	if(isset($_POST['check']))
	{
		$user = $con->real_escape_string($_POST['Username']);
		$pass = $con->real_escape_string($_POST['Password']);
		if(IsUserAdminLogged($user, $pass) == 1)
			header("Location: index.php?page=main");
		else
			$err = "<div style=\"color:red;\">Wrong Username/Password</div>";
	}
?>

<!DOCTYPE html PUBLIC "-//W3C//DTD XHTML 1.0 Transitional//EN"
"http://www.w3.org/TR/xhtml1/DTD/xhtml1-transitional.dtd">

<html xmlns="http://www.w3.org/1999/xhtml">
<head>
	<meta http-equiv="Content-type" content="text/html; charset=UTF-8" />
	<link rel="stylesheet" type="text/css" href="style.css" />
	<title>Admin Panel</title>
</head>
<body>
	<?php require "header.php"; ?>
	<div class="window">
	
		<div class="modesItem">
			<div class="header">
			Log In admin panel
			</div>
			<div class="dialog">
				<form method="post" action="">
					<?php echo $err; ?>
					<label>Username:</label>
					<div>
					<input type="text" name="Username" />
					</div>			
					<label>Password:</label>
					<div>
					<input type="password" name="Password" />
					</div>	
				
					<input type="hidden" name="check" value="1" />
					<input type="submit" value="Log In" />					
				</form>
			</div>	
		</div>
	</div>


</body>
</html>