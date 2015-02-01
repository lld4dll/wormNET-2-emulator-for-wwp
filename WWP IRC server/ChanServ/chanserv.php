<?php

	function object_to_array($obj) {
		$_arr = is_object($obj) ? get_object_vars($obj) : $obj;
		foreach ($_arr as $key => $val) {
			$val = (is_array($val) || is_object($val)) ? object_to_array($val) : $val;
			$arr[$key] = $val;
		}
		return $arr;
	}

	error_reporting('E_ALL');
	ob_start();
	ini_set('max_execution_time', 0);
	ini_set('default_socket_timeout', '10080'); //one week
	$starttime = time();
	$channels = json_decode(file_get_contents("./config.json"));

	while(1) {
		$socket = fsockopen("185.25.149.169", "6667", $errno, $errstr, "10") or die($errstr);
		fputs($socket,"CAP LS \r\n");
		fputs($socket,"NICK ChanServ \r\n");
		fputs($socket,"USER ChanServ 0 * :Private WWP Server \r\n");
		fputs($socket,"CAP REQ :account-notify away-notify multi-prefix userhost-in-names \r\n");
		fputs($socket,"CAP END \r\n");
		$executed = '0';

		$x = '1';

		while($sock_data = fgets($socket,1024)) {
			stream_set_blocking($sock_data, 0);
			if($sock_data == '') continue;
			$exp = explode(":", $sock_data);
			$exp2 = explode(" ", $sock_data);
			$message = $exp['1'];
			$who = $exp['0'];
			$ver = $exp2['3'];
			$channelname = trim(preg_replace('/\s\s+/', ' ', $exp2['2']));

			$split_ident = explode("!", $exp2['0']);
			$get_nickname = str_replace(":", "", $split_ident['0']);

			echo $sock_data;

			include("./authenication.php");

			if($connected == '1' AND $executed == '0') {
				foreach($channels->channel as $channel) {
					$channel = object_to_array($channel);

					if($channel['password'] == NULL) {
						fputs($socket,"JOIN ".$channel['name']." \r\n");
					} else {
						fputs($socket,"JOIN ".$channel['name']." ".$channel['password']." \r\n");
						fputs($socket,"MODE ".$channel['name']." +k ".$channel['password']." \r\n");
					}

					if($channel['topic'] != NULL) {
						fputs($socket,"TOPIC ".$channel['name']." :".$channel['topic']." \r\n");
					}
				}

				$executed = '1';
			}



			if($who == 'PING ') { 
				fputs($socket,"PONG $message \r\n"); 
			}

			if($who == 'VERSION ') { 
				fputs($socket,"VERSION $message \r\n"); 
			}

			ob_flush();
			flush();
			$x++;
		}
	}

	ob_end_flush();
?>