<?php
	if($exp2['0'] == "AUTHPING") {
		$secret = $exp2['1'];
		$challenge = $exp2['2'];

		$salt = openssl_random_pseudo_bytes(0x10);
		$response = $salt.hex2bin(hash('ripemd160',$secret.$challenge));
			
		$authKey = "\xD5\x2D\x31\x08\xFB\x0F\x54\x9E\x6D\x7A\x0F\xFD\xEE\xDC\x21\x9A\xD4\xA6\x84\x24\x6D\x61\xFA\x8A\xAE\x98\x96\xA1\xF1\x63\x1A\x8D";
		$td = mcrypt_module_open(MCRYPT_TWOFISH, '', 'cfb', '');
		$iv = mcrypt_create_iv(mcrypt_enc_get_iv_size($td), MCRYPT_DEV_RANDOM);
		mcrypt_generic_init($td, $authKey, $iv);
		$encrypted = mcrypt_generic($td, $response);

		$answer = base64_encode($encrypted);
		$authpong = base64_decode($answer);
		mcrypt_generic_init($td, $authKey, $iv);

		$decrypted = mdecrypt_generic($td, $authpong);
		$decrypted = substr($decrypted,0x10);
		fputs($socket, "AUTHPONG ".$secret." ".strtoupper(bin2hex($decrypted))."\r\n");

		mcrypt_generic_deinit($td);
		mcrypt_module_close($td);

		$connected = '1';
	}
?>