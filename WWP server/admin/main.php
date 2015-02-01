<?php
require "CheckUser.php";
if($isAdmin == 1)
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
		<div class="window">
			<div class="menuHeader">
			MainMenu
			</div>	
			<div class="menu">
				<span class="item" onmouseover="document.getElementById('a').style.opacity=1.0;document.getElementById('a').filters.alpha.opacity=100" onmouseout="document.getElementById('a').style.opacity=0.4;document.getElementById('a').filters.alpha.opacity=40">
					<a class="link" href ="index.php?page=setModes&amp;action=view"><img id="a" src="images/scheme.png" title="Set Channel Modes" alt="Set Channel Modes"/></a>
					<br/>
					<a href="index.php?page=setModes&amp;action=view" class="title">Set Channel Modes</a>
				</span>			
				<span class="item" onmouseover="document.getElementById('b').style.opacity=1.0;document.getElementById('b').filters.alpha.opacity=100" onmouseout="document.getElementById('b').style.opacity=0.4;document.getElementById('b').filters.alpha.opacity=40">
					<a class="link" href ="index.php?page=specialWeapons"><img id="b" src="images/weapon.png" title="Set Channel Modes" alt="Set Channel Modes"/></a>
					<br/>
					<a href="index.php?page=specialWeapons" class="title">Set Special weapons</a>
				</span>		
				<span class="item" onmouseover="document.getElementById('c').style.opacity=1.0;document.getElementById('c').filters.alpha.opacity=100" onmouseout="document.getElementById('c').style.opacity=0.4;document.getElementById('c').filters.alpha.opacity=40">
					<a class="link" href ="index.php?page=userSettings&amp;action=view"><img id="c" src="images/user.png" title="Users settings" alt="Users settings"/></a>
					<br/>
					<a href="index.php?page=userSettings&amp;action=view" class="title"><center>Users Settings</center></a>
				</span>		
				<span class="item" onmouseover="document.getElementById('d').style.opacity=1.0;document.getElementById('d').filters.alpha.opacity=100" onmouseout="document.getElementById('d').style.opacity=0.4;document.getElementById('d').filters.alpha.opacity=40">
					<a class="link" href ="index.php?page=help"><img id="d" src="images/help.png" title="Help" alt="Help"/></a>
					<br/>
					<a href="index.php?page=help" class="title"><center>Help</center></a>
				</span>						
			</div>
			
		</div>
	</body>
	</html>
<?php
}
else
{
	header("location: login.php");
	die();
}
?>