<?php

	function s_echo($str) {
		$str = htmlspecialchars($str);
		echo($str);
	}

	function s_input($str) {
		$str = trim(strip_tags($str));
		$str = escapeshellcmd($str);
		if(get_magic_quotes_gpc())
			$str=stripslashes($str);		
		return($str);
	}



	function s_alert() {
		echo	('<html><head><title>Im not your enemy!</title>' .
			'<body style="background:red;color:white;font-weight:bold;">' .
			'<h1>Security Alert</h1><br>' .
			'<h2><b>Whom you triying to fuck??</b></h2></body></html>'
			);
		exit(0);	
	}
	
	
	//no IPV6 implementation in v1
	function s_inputip($ip) {
	    $pattern =  '/(\d{1,3})\.(\d{1,3})\.(\d{1,3})\.(\d{1,3})/';
	    $parts = array();
	    if(preg_match($pattern,$ip,$parts)==1) {
		if(inet_pton($ip) !== false)
		  return($ip);
	    }
	    return(0);
	}

?>
