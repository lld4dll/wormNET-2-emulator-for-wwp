<?php

	/* USAGE:
	 * php -f WWPCLI.php <NICKNAME> <AUTHSECRET> <AUTHCHALLENGE>
	 * Example: php -f WWPCLI.php Zexorz 1 48A0D9EA00B0F0E12B263E28929EBC671057FB4D
	 */

	function fileGetContent($url, $output = 0) {
		$opts = array(
			'http'=>array(
				'method' => "GET",
				'header' => "User-Agent: T17Client/2.0".PHP_EOL.
							"Pragma: No-Cache".PHP_EOL.
							"Cookie: PHPSESSID=f0ivu1bj13sklbl7pkdq6ik0i0".PHP_EOL.
							"FileResult: 255".PHP_EOL.
							"FileData: ".PHP_EOL.
							"UserLevel: 2".PHP_EOL.
							"UserServerIdent: 1".PHP_EOL.
							"Counter: ".time().PHP_EOL
			)
		);

		$context = stream_context_create($opts);
		$reply = file_get_contents($url, false, $context);

		if($reply != NULL) {
			echo "Connected to {$url}".PHP_EOL;
		} else {
			die("Failed to connect to {$url}".PHP_EOL);
		}

		if($output == 1) {
			return $reply;
		}
	}

	$IRCUsername = $_SERVER['argv']['1'];
	$IRCSecret = "eb9q4.team17.com$".$_SERVER['argv']['2'];
	$IRCChallenge = $_SERVER['argv']['3'];

	fileGetContent("http://eb9q4.team17.com/eb9q4web/welcomeloginform.asp");
	fileGetContent("http://eb9q4.team17.com/eb9q4web/welcomeloginform.php");
	fileGetContent("http://eb9q4.team17.com/eb9q4web/SelectServer.php");
	fileGetContent("http://eb9q4.team17.com/eb9q4web/Login.php?UserName=".$IRCUsername);
	$RealChallenge = fileGetContent("Http://eb9q4.team17.com:80/eb9q4web/RequestAuth.php?Secret=".$IRCSecret."&Challenge=".$IRCChallenge, 1);

	$RealChallenge = str_replace(array("<ANSWER ", ">"), "", $RealChallenge);

	$salt = openssl_random_pseudo_bytes(0x10);
	$response = $salt.hex2bin(hash('ripemd160',$IRCSecret.$IRCChallenge));
			
	$authKey = "\xD5\x2D\x31\x08\xFB\x0F\x54\x9E\x6D\x7A\x0F\xFD\xEE\xDC\x21\x9A\xD4\xA6\x84\x24\x6D\x61\xFA\x8A\xAE\x98\x96\xA1\xF1\x63\x1A\x8D";
	$td = mcrypt_module_open(MCRYPT_TWOFISH, '', 'cfb', '');
	$iv = mcrypt_create_iv(mcrypt_enc_get_iv_size($td), MCRYPT_DEV_RANDOM);
	mcrypt_generic_init($td, $authKey, $iv);
	$encrypted = mcrypt_generic($td, $response);

	$answer = base64_encode($encrypted);
	$authpong = base64_decode($RealChallenge);
	mcrypt_generic_init($td, $authKey, $iv);

	$decrypted = mdecrypt_generic($td, $authpong);
	$decrypted = substr($decrypted,0x10);
	
	echo PHP_EOL.PHP_EOL."AUTHPONG SEEMS TO BE: /AUTHPONG ".$IRCSecret." ".strtoupper(bin2hex($decrypted)).PHP_EOL.PHP_EOL;
	mcrypt_generic_deinit($td);
	mcrypt_module_close($td);

?>