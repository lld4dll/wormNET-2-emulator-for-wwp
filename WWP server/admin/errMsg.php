<?php
	function setErrorMsg($err, $title, $page, $back, $question)
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
			<div class="modesItem">
				<div class="header">
				<?php echo $title;?>
				</div>
				<div class="dialogMsg">
				<?php echo $err; ?>
				<br/>
				<?php echo $question; ?>
				<br/>
				<input type="button" value="Yes" onclick="javascript:location.href='<?php echo $page ?>'" />
				<input type="button" value="No" onclick="javascript:location.href='<?php echo $back ?>'" />
				</div>	
			</div>
		</div>
		</body>
		</html>
	<?php
	}
	
	function setSuccessMsg($success, $title, $page, $back, $question)
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
			<div class="modesItem">
				<div class="header">
				<?php echo $title;?>
				</div>
				<div class="dialogMsg">
				<?php echo $success; ?>
				<br/>
				<?php echo $question; ?>
				<br/>
				<input type="button" value="Yes" onclick="javascript:location.href='<?php echo $page ?>'" />
				<input type="button" value="No" onclick="javascript:location.href='<?php echo $back ?>'" />
				</div>	
			</div>
		</div>
		</body>
		</html>
	<?php
	}	

	function setInstallErrMsg($err, $title, $back, $question)
	{
	?>
		<!DOCTYPE html PUBLIC "-//W3C//DTD XHTML 1.0 Transitional//EN"
		"http://www.w3.org/TR/xhtml1/DTD/xhtml1-transitional.dtd">
		<html xmlns="http://www.w3.org/1999/xhtml">
		<head>
			<meta http-equiv="Content-type" content="text/html; charset=UTF-8" />
			<link rel="stylesheet" type="text/css" href="admin/style.css" />
			<title>Admin Panel</title>
		</head>
		<body>	
		<?php require "header.php"; ?>
		<div class="window">
			<div class="modesItem">
				<div class="header">
				<?php echo $title;?>
				</div>
				<div class="dialogMsg">
				<?php echo $err; ?>
				<br/>
				<?php echo $question; ?>
				<br/>
				<input type="button" value="Try Again" onclick="javascript:location.href='<?php echo $back ?>'" />
				</div>	
			</div>
		</div>
		</body>
		</html>
	<?php
	}	

	function setInstallSucessMsg($success, $title, $question)
	{
	?>
		<!DOCTYPE html PUBLIC "-//W3C//DTD XHTML 1.0 Transitional//EN"
		"http://www.w3.org/TR/xhtml1/DTD/xhtml1-transitional.dtd">
		<html xmlns="http://www.w3.org/1999/xhtml">
		<head>
			<meta http-equiv="Content-type" content="text/html; charset=UTF-8" />
			<link rel="stylesheet" type="text/css" href="admin/style.css" />
			<title>Admin Panel</title>
		</head>
		<body>	
		<?php require "header.php"; ?>
		<div class="window">
			<div class="modesItem">
				<div class="header">
				<?php echo $title;?>
				</div>
				<div class="dialogMsg">
				<?php echo $err; ?>
				<br/>
				<?php echo $question; ?>
				<br/>
				<input type="button" value="Go to Login" onclick="javascript:location.href='admin/login.php'" />
				</div>	
			</div>
		</div>
		</body>
		</html>
	<?php
	}		
?>
