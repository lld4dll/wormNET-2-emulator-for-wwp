<?php	
	require "CheckUser.php";
	require "errMsg.php";
	function CheckEmail($email)
	{ 
		return filter_var($email, FILTER_VALIDATE_EMAIL);
	}
	
	function getUsers()
	{
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
			<div class="userSettings">
				<div class="header">
				List of Users
				</div>	
				<div class="modesInfoDialog">
				<br/>
				<?php
				global $con;
				$command = "SELECT * FROM `USERS`";
				$res = $con->query($command) or die($con->error);
				$userCnt = $res->num_rows;
				?>
						<div style="margin:1%;padding-left:18%;float:left;text-align:center;" onmouseover="document.getElementById('a').style.opacity=1.0;document.getElementById('a').filters.alpha.opacity=100" onmouseout="document.getElementById('a').style.opacity=0.4;document.getElementById('a').filters.alpha.opacity=40">
							<div>
							<a href="index.php?page=main">
							<img id="a" src="images/home.png" title="main" alt="main" />
							</a>
							</div>
							<div>
							<a href="index.php?page=main">main</a>
							</div>
						</div>									
						<br/>
						<table style="clear:both;margin-left:auto;margin-right:auto;">
							<tr>
							<th>UserID</th>
							<th>UserName</th>
							<th>Email</th>
							<th>PlayerType</th>
							<th>PlayerScore</th>
							<th>PlayerLevel</th>
							<th>Banned</th>
							<th>Access</th>
							<th>Edit</th>
							<th>Delete</th>
							</tr>
							<?php
							while($userData = $res->fetch_assoc())
							{
								echo "<tr>\n<td>".$userData['UserID']."</td>\n";
								echo "<td>".$userData['UserName']."</td>\n";
								echo "<td>".$userData['Email']."</td>\n";
								echo "<td>".$userData['PlayerType']."</td>\n";
								echo "<td>".$userData['PlayerScore']."</td>\n";
								echo "<td>".$userData['PlayerLevel']."</td>\n";
								echo "<td>".$userData['Banned']."</td>\n";
								echo "<td>".$userData['Access']."</td>\n";
								echo "<td><a href=\"index.php?page=userSettings&amp;action=edit&amp;uid=".$userData['UserID']."\"><img src='images/edit.png' title='edit' alt='edit' /></a></td>\n";
								echo "<td><a href=\"index.php?page=userSettings&amp;action=delete&amp;uid=".$userData['UserID']."\"><img src='images/delete.png' title='delete' alt='delete'/></a></td>\n</tr>\n";
							}
							?>
						</table>
				</div>
			</div>
		</body>
		</html>			
		<?php
	}
	
	function editUsers($uid)
	{
		global $con;
		$command = "SELECT * FROM `USERS` WHERE UserID='$uid' LIMIT 1";
		$res = $con->query($command) or die($con->error);
		$userData = $res->fetch_assoc();
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
			<div class="modesItem">
				<div class="header">
				Edit User Settings
				</div>	
				<div class="modesInfoDialog">
					<br/>
					<div style="margin:1%;margin-left:0%;float:left;text-align:center;" onmouseover="document.getElementById('a').style.opacity=1.0;document.getElementById('a').filters.alpha.opacity=100" onmouseout="document.getElementById('a').style.opacity=0.4;document.getElementById('a').filters.alpha.opacity=40">
						<div>
						<a href="index.php?page=main">
						<img id="a" src="images/home.png" title="main" alt="main" style="opacity:0.4;"/>
						</a>
						</div>
						<div>
						<a href="index.php?page=main">main</a>
						</div>
					</div>		
				<div style="margin:1%;float:left;width:12%;text-align:center;" onmouseover="document.getElementById('b').style.opacity=1.0;document.getElementById('b').filters.alpha.opacity=100" onmouseout="document.getElementById('b').style.opacity=0.4;document.getElementById('b').filters.alpha.opacity=40">
					<div>
					<a href="index.php?page=userSettings&amp;action=view">
					<img id="b" src="images/view.png" title="View" alt="View" style="opacity:0.4;"/>
					</a>
					</div>
					<div>
					<a href="index.php?page=userSettings&amp;action=view">View</a>
					</div>
				</div>						
					<form method="post" action="">
					<div>
						<div style="padding-top:1%;width:15%;float:left;clear:both;">
						<label>UserName:</label>
						</div>
						<div style="padding-top:1%;padding-left:4%;float:left;">
							<input type="text" name="UserName" value="<?php echo $userData['UserName'];?>"/>
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
						<div style="padding-top:1%;width:15%;float:left;clear:both;">
						<label>Email:</label>
						</div>
						<div style="padding-top:1%;padding-left:4%;float:left;">
							<input type="text" name="mail" value="<?php echo $userData['Email'];?>"/>
						</div>
					</div>							
					<div>
						<div style="padding-top:1%;width:15%;float:left;clear:both;">
						<label>PlayerType:</label>
						</div>
						<div style="padding-top:1%;padding-left:4%;float:left;">
							<input type="text" name="playerType" value="<?php echo $userData['PlayerType'];?>"/>
						</div>
					</div>	
					<div>
						<div style="padding-top:1%;width:15%;float:left;clear:both;">
						<label>PlayerScore:</label>
						</div>
						<div style="padding-top:1%;padding-left:4%;float:left;">
							<input type="text" name="playerScore" value="<?php echo $userData['PlayerScore'];?>"/>
						</div>
					</div>						
					<div>
						<div style="padding-top:1%;width:15%;float:left;clear:both;">
						<label>PlayerLevel:</label>
						</div>
						<div style="padding-top:1%;padding-left:4%;float:left;">
							<input type="text" name="playerLevel" value="<?php echo $userData['PlayerLevel'];?>"/>
						</div>
					</div>		
					<div>
						<div style="padding-top:1%;width:15%;float:left;clear:both;">
						<label>Banned:</label>
						</div>
						<div style="padding-top:1%;padding-left:4%;float:left;">
							<select name="Banned">
								<?php
								if($userData['Banned'] == "1")
									echo "<option value=\"1\" checked>Yes</option>\n
									<option value=\"0\">No</option>";
								else
									echo "<option value=\"0\" checked>No</option>\n
									<option value=\"1\">Yes</option>";
								?>
							</select>
						</div>
					</div>						
					<div>
						<div style="padding-top:1%;width:15%;float:left;clear:both;">
						<label>Access:</label>
						</div>
						<div style="padding-top:1%;padding-left:4%;float:left;">
							<select name="Access">
								<?php
								if($userData['Access'] == "1")
									echo "<option value=\"1\" checked>Admin</option>\n
									<option value=\"0\">User</option>";
								else
									echo "<option value=\"0\" checked>User</option>\n
									<option value=\"1\">Admin</option>";
								?>
							</select>
						</div>
					</div>													
					<div>
						<div style="float:left;clear:both;">
						<input type="hidden" name="check" value="1" />
						<input type="submit" value="Edit user"/>	
						</div>
					</div>						
					</form>
				</div>
			</div>
		</body>
		</html>		
	<?php
	}
	
	function updateUser($uid)
	{
		global $con;
		$username = $con->real_escape_string($_POST['UserName']);
		$username = htmlspecialchars($username);
		$pwd = $con->real_escape_string($_POST['Pwd']);
		$pwd1 = $con->real_escape_string($_POST['Pwd1']);
		$mail = htmlspecialchars($con->real_escape_string($_POST['mail']));
		$playerType = preg_replace("/[^0-9]/", "", $_POST['playerType']);			
		$playerScore = preg_replace("/[^0-9]/", "", $_POST['playerScore']);
		$playerLevel = preg_replace("/[^0-9]/", "", $_POST['playerLevel']);
		$access = preg_replace("/[^0-9]/", "", $_POST['Access']);
		$banned = preg_replace("/[^0-9]/", "", $_POST['Banned']);
		
		$mail = $con->real_escape_string($mail);	
		if(CheckEmail($mail) == false || strlen($username) < 3)
			return 0;	
		if($uid == 1)
		{
			$access = 1;
			$banned = 0;
		}
		if(empty($pwd) && empty($pwd1))
		{
			$command = "UPDATE `USERS` SET UserName='$username', Email='$mail', PlayerType='$playerType', PlayerScore='$playerScore', 
						PlayerLevel='$playerLevel', Banned ='$banned', Access='$access' WHERE UserID='$uid'";
			$con->query($command) or die($con->error);
			return 1;
		}		
		if(strcmp($pwd,$pwd1) == 0)
		{
			if(empty($pwd) || strlen($pwd) < 5)
				return 0;
			$command = "UPDATE `USERS` SET UserName='$username', Password='$pwd', Email='$mail', PlayerType='$playerType', PlayerScore='$playerScore', 
						PlayerLevel='$playerLevel', Banned ='$banned', Access='$access' WHERE UserID='$uid'";
			$con->query($command) or die($con->error);			
		}
		return 1;
	}
	
	function deleteUser($uid)
	{
		global $con;
		if($uid == 1)
		{
			setErrorMsg("cant delete main Admin<br/>", "failed delete admin", "index.php?page=userSettings&amp;action=view", "index.php?page=main", "try to delete other user?");
			die();
			return 0;
		}
		$command = "DELETE FROM `USERS` WHERE UserID='$uid'";
		$con->query($command) or die($con->error);
		return 1;
	}
	
if($isAdmin == 1)
{				
	if(isset($_POST['check']))
	{	
		switch($_GET['action'])
		{		
			case "edit":
				$uid = preg_replace("/[^0-9]/", "", $_GET['uid']);
				$ok = updateUser($uid);			
				if ($ok == 1)
					setSuccessMsg("User has been updated<br/>", "succeed edit User", "index.php?page=userSettings&amp;action=view", "index.php?page=main", "Update more user?");
				else
					setErrorMsg("failed to update user<br/>", "failed update user", "index.php?page=userSettings&amp;action=view", "index.php?page=main", "try to update other user?");
			break;
		}
	}
	else
	{
		switch($_GET['action'])
		{
			case "view":
				getUsers();
			break;
			case "edit":
				$uid = preg_replace("/[^0-9]/", "", $_GET['uid']);
				editUsers($uid);
			break;
			case "delete":
				$uid = preg_replace("/[^0-9]/", "", $_GET['uid']);
				$ok = deleteUser($uid);
				if($ok == 1)
					setSuccessMsg("User has been removed<br/>", "succeed remove User", "index.php?page=userSettings&amp;action=view", "index.php?page=main", "Delete more user?");
				else
					setErrorMsg("cant delete user<br/>", "failed delete user", "index.php?page=userSettings&amp;action=view", "index.php?page=main", "try to delete other user?");
			break;
		}
	}
}
else
{
	header("location: login.php");
	die();
}	
?>