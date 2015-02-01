<?php
/*
This is our login/register page
*/
	header("UserLevel: 0");
	header("UserServerIdent: 1");
	header("Counter:". time());
	$folderPath = "wormsProject";
	echo "<WEBADDRESS /".$folderPath."/>\n
	<EXTENSION .php>\n
	<FONT Size=1 Colour=0>                               Welcome to WormNet2<BR></FONT>\n
	<br><br><FONT Size=0 Colour=0>                                 You can use any username / password to log in<BR></FONT>\n
	<FONT Size=3 Colour=3>                                                                Your IP address is detected as: ".$_SERVER['REMOTE_ADDR']."<BR></FONT>\n
	<br>\n                                                                      <a href=\"/".$folderPath."/LoginForm.php?ServerId=1\"><FONT Size=0>Login<BR></FONT>\n
	<br></a>\n
                                                                      <a href=\"/".$folderPath."/CreateUserAccount.php\"><FONT Size=0>Register<BR></FONT>\n
	<br></a>\n";

?>