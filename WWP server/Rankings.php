<?php
/*
This page is unused, because we are not implement rankings look on ReadME file for further.
*/
	$fp = fopen('logs.txt', 'a');
	fwrite($fp,"Rankings.php\r\n");
	fwrite($fp, print_r($_GET, TRUE));
	fclose($fp);
?>