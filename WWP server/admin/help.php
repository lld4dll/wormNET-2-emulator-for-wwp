<?php	
	require "CheckUser.php";
	require "errMsg.php";
	
	function getHelpPage()
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
			<div class="help">
				<div class="header">
				Help
				</div>	
				<div class="modesInfoDialog">
				<br/>
						<div style="margin:1%;float:left;text-align:center;" onmouseover="document.getElementById('a').style.opacity=1.0;document.getElementById('a').filters.alpha.opacity=100" onmouseout="document.getElementById('a').style.opacity=0.4;document.getElementById('a').filters.alpha.opacity=40">
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
				<div style="padding-top:1%;float:left;clear:both;">
				How To set Channel modes?
				<br/>
				how To set special weapons?
				</div>
				</div>
			</div>
		</body>
		</html>		
	<?php
	}
	
	if($isAdmin == 1)
	{				
		getHelpPage();
	}
	else
	{
		header("location: login.php");
		die();
	}	
	?>