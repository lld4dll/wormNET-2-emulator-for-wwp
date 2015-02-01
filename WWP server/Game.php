<?php
/*
This page used for create the games hosted on WWP server
*/
session_start();

/* This is the PRIVATE RSA key to sign our data, it is possible to change
 the key to something else, so not everyone can encrypt the host games except us.
*/
$key = <<<EOF
-----BEGIN RSA PRIVATE KEY-----
MIIEogIBAAKCAQEAne4nZgBnyEZSIoZ74dXGDOSgxsIBEfw5jbXka1d/mCrxVH78
WceG9Zp7iRWbDZaiEAwB6rpHGR+P05H/c6H5bZ1eoSgzNnF7boR3AOBscwbualr0
aHcPKZAqiobca8V8dymNLUty5+/4xFRcRlsKVHWERq70g+b8gD+Z/idf8tZTV9e9
Az+dVRYDwh4KJf8Jz2PNNyuilrw9Pmuo2VhCNgEWfLD9/sEZsXOl3+GE1d7oifu2
9eT/OYweEGNjwyBQ01HRiTkByo2X89EAJW6M8So55zUk3xvLs+EqqzaWLEan3mjf
J7Xp5mAiodKgopmdtHLdxiK+25a/zH0omNTRXQIDAQABAoIBAGu0mov25NByHNvM
6bmnyAzM81T5xC8W281oaiJGpw8I4lLk04cLC2B+fZeM85S90UKuXbROW+SnYLhC
Tnv20BvQglPYDa0okaxajKo9Y9jCtmA0LZD8+Ys39btJpBOfuHWwEcb9OJ0zTG3g
tpmIxy+4gYCPGtTF+BgiyAvLoKUEMarthN+LKuuihjQV7AY1XfRbC3FzWNANJonv
0vkWkcjtFqRZbJV6esbJ4dHvBG/f3cEBtZYJXhX5UNyplA/0dhlrIEbcGua/KfrA
M7lir3Ebi+CsOTNAtPK6rViRs6OMxLVvbQOCNVC3Ms8znEfOesnFkVMzXUIL2fPb
lGO5wIUCgYEAzyrieC38J6okZzhLEdFy3Psb9222suKdDue8fS/LuR2LzGBluzmB
9ULWnXwKb5qCowQqFE86BKapubb9V0V0k49GUbTsNElIRaZuN9BCNcq10u67tC8O
KGZhV8ka4swZA4tke153fPPfb5iFTAWyUjL895npH4bIE5PIBLfMpQsCgYEAwygl
Tu4/X4UTSpUy2HXl1HySetKE675UK5+IbciSp0iQ4GoiGJ13o5R5vDIBGWthO+DE
PB7qaxBkssXsvMrgnItI7kbdDP9awBvUJIXGtXyd4Gzpk/kw4R0aMNs6nKh9AOlb
eKiDso/y5TtQZF+ITxWGiZ1B61GGU3dyJEc5lDcCgYB+LKVQAtnfoo2XXD7EdGdb
1Yib8vNiEgkMKfGv4StX7HnXmSJ8nDLdmL6znfeRL26/MGscgYBZktQuaxfzTjcJ
JCr02pc95SAiY5IfZjTdJEAHXyVtQXeC/Y/t8mrwlNPrR7uEM+3caX+9LQyKV7Ha
xHkzqEbGz8UNv2UEEpmOLwKBgBvj4uES32+xPv6LsAaLYtspKm3vJBvkyXaqh/7Y
iUzq6/mGcXT/HBf+oh8pLksJs42xI5pL+MhwQVPjQWABxnPY3AVhjznKTJfj4j+N
9sX9Fzr5lbAWDsw9Mn8E9a1pzuOQdggaXSfptpbYETcnB/31X5LPLJqKvh2zllZk
72+lAoGAX3h3ifctwcUbI1uZe7W01LQRSfZd6KiZFGG+lgMzBmztshKO1jO57PCc
QgTkJSaewE7tj4V5+PQmHA10TEVOGqVZ8q0DB41lOcgjENRk0E3EYUAfJo5/uElu
q+0Q+24d7yVAZetNxbn2qlZQiW0tAsDXib5VEeMGh7WCDTv27Q0=
-----END RSA PRIVATE KEY-----
EOF;

	header('Content-type: text/html; charset=utf-8');
	require "connect_to_mysql.php";
	
	function userAuth($user, $pass)
	{
		global $con;
		$command = "SELECT * FROM `USERS` WHERE UserName='$user' and Password='$pass'";
		$res = $con->query($command) or die($con->error);
		$userCnt = $res->num_rows;
		if($userCnt == 1)
		{
			return 1;
		}
		else
			return 0;
	}
	
	function encryptGame($plainText)
	{
		global $key;
		$hostKey = "\x6C\x91\xA8\xEC\x79\x38\x35\xF4\x7E\x58\x26\x18\x23\x10\x19\x24\x65\x93\x90\x0F\xFE\xCA\x63\xE2\x63\x2E\xB8\x95\xB6\xF3\xAC\x4C";
		$td = mcrypt_module_open(MCRYPT_TWOFISH, '', 'cfb', '');
		$iv = mcrypt_create_iv(mcrypt_enc_get_iv_size($td), MCRYPT_DEV_RANDOM);
		mcrypt_generic_init($td, $hostKey, $iv);		
		$encrypted = mcrypt_generic($td, $plainText);
		mcrypt_generic_deinit($td);
		mcrypt_module_close($td);

		$res = openssl_get_privatekey($key,""); 
		openssl_private_encrypt($encrypted,$encryptedGame,$res); 				
		return base64_encode($encryptedGame);
	}
	
	function deleteGame($GameID, $gameName, $cmd="")
	{
		if(strcmp($_SESSION['IPAddr'], $_SERVER['REMOTE_ADDR']) == 0)
		{	
			global $con;		
			$GameID = intval($GameID);
			if(strcmp($cmd,"Failed"))
			{
				$command = "SELECT * FROM `HostGames` WHERE GameID='$GameID'";
				$res = $con->query($command) or die($con->error);
				$isFound = $res->num_rows;						
				if($isFound == 1)
				{
					$gameName = $con->real_escape_string($gameName);
					$command = "DELETE FROM `HostGames` WHERE GameID='$GameID' and Name='$gameName'";
					$con->query($command) or die($con->error);				
				}
			}
				$command = "SELECT * FROM `EncryptedHost` WHERE GameID='$GameID'";
				$res = $con->query($command) or die($con->error);
				$isFound = $res->num_rows;
				if($isFound == 1)
				{
					$command = "DELETE FROM `EncryptedHost` WHERE GameID='$GameID'";
					$con->query($command) or die($con->error);
				}
		}
		
	}
	
	function addGameToTable($gameName, $hostIP, $nick, $location, $type, $channel, $pwd, $now)
	{
		
		global $con;
		if (empty($pwd))
				$isPassword = 0;
			else
				$isPassword  = 1;		
		$gameName = $con->real_escape_string($gameName);
		$hostIP = $con->real_escape_string($hostIP);
		$nick = $con->real_escape_string($nick);
		$channel = $con->real_escape_string($channel);
		$command = "INSERT INTO `HostGames` (Name, HostIP, Nick, Type, Loc, Pwd, Time, Chan)
								VALUES('$gameName', '$hostIP', '$nick', '$type', '$location', '$isPassword', '$now', '$channel')";
		$con->query($command) or die($con->error);	
		
		$gameId = $con->insert_id;
		$serverUsage = 1; // need to be 1 for valid game, otherwise the game cant be chosed.
		$next2Hours = $now + 2*60*60;
		$data = str_pad($gameName,20,"\x00"). 
				str_pad($nick,20,"\x00").
				pack("N",ip2long($hostIP)). 
				pack("N",$gameId). 
				pack("N",$location). 
				pack("N",$serverUsage). 
				pack("N",$isPassword).
				pack("N",$type).
				pack("N",$now).
				pack("N",$next2Hours);	
		$checksum = hash('ripemd160',$data);			
		$salt = md5($gameId.$now,true);
		$game = encryptGame($salt.hex2bin($checksum).$data);	


		$command = "INSERT INTO `EncryptedHost` (EncryptedGame,GameID,Channel,Time)
										VALUES('$game', '$gameId', '$channel', '$now')";
		$con->query($command) or die($con->error);		
		
		header("SetGameId: ".$gameId); // used for show the hosted game on WWP
	}
	
	function hostCommands()
	{
		if(isset($_GET['Cmd']))
		{
			$cmd = $_GET['Cmd'];	
			if(strcmp($cmd,"Create") == 0)
			{
				$gameName = $_GET['Name'];
				$hostIP = $_GET['HostIP'];
				$nick = $_GET['Nick'];
				$location = $_GET['Loc'];
				$type = $_GET['Type'];
				$channel = $_GET['Chan'];
				$pwd = $_GET['Pwd'];		
				$now = time();	
				
				addGameToTable($gameName, $hostIP, $nick, $location, $type, $channel, $pwd, $now);
				$fp = fopen('logs.txt', 'a');
				fwrite($fp, print_r($_GET, TRUE));
				fclose($fp);
			}
			else if(strcmp($cmd,"Close") == 0)
			{
				deleteGame($_GET['GameID'], $_GET['Name']);
				/*	for debugging usage
				$fp = fopen('logs.txt', 'a');
				fwrite($fp, print_r($_GET, TRUE));
				fclose($fp);	
				*/
			}
			/*	for debugging usage
			else if(strcmp($cmd,"Open") == 0)
			{
				
				$fp = fopen('logs.txt', 'a');
				fwrite($fp, print_r($_GET, TRUE));
				fclose($fp);	
				
			}	
			*/
			else if(strcmp($cmd,"Delete") == 0)
			{
				deleteGame($_GET['GameID'], $_GET['Name']);
				/*	for debugging usage
				$fp = fopen('logs.txt', 'a');
				fwrite($fp, print_r($_GET, TRUE));
				fclose($fp);	
				*/
			}	
			else if(strcmp($cmd,"Failed") == 0)
			{
				deleteGame($_GET['GameID'], $_GET['Name'], "Failed");
				/*	for debugging usage
				$fp = fopen('logs.txt', 'a');
				fwrite($fp, print_r($_GET, TRUE));
				fclose($fp);	
				*/
			}	
			/*	for debugging usage			
			else
			{
				
				$fp = fopen('logs.txt', 'a');
				fwrite($fp, print_r($_GET, TRUE));
				fclose($fp);			
				
			}
			*/
		}
	}
	
	if(userAuth($_SESSION['Username'],$_SESSION['Password']) == 1)
	{
		hostCommands();
	}

	
	/*
	echo hash('ripemd160',$fullData) . "<br>";

	struct host
	{
		BYTE checksum[20]; // hold RIPEMD160 hash
		BYTE hostname[20];
		BYTE username[20];
		DWORD ipInDecimal;
		DWORD gameId;
		DWORD countryFlag;
		DWORD serverUsage; // store 1 if the game is set right and you can click on it,store 0 if you cant click
		BOOL isPasswordGame;
		DWORD type; // store the typed rank of the channel
		timer_t timestamp[2]; // 1 of them hold currenttime + 2 hours
	}
	*/	
?>
