<?php	
	header('Content-type: text/html; charset=utf-8');
	require "CheckUser.php";
	if($isAdmin == 1)
	{
		if($_GET['page'] == "main")
		{
			require "main.php";
		}
		elseif($_GET['page']=="setModes")
		{
			require "setModes.php";
		}
		elseif($_GET['page']=="specialWeapons")
		{
			require "specialWeapons.php";
		}	
		elseif($_GET['page']=="userSettings")
		{
			require "userSettings.php";
		}		
		elseif($_GET['page']=="help")
		{
			require "help.php";
		}		
		else
		{
			require "main.php";
		}
	}
	else
	{
		header("location: login.php");
		die();
	}
?>
