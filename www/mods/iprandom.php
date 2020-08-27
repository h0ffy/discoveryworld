<?php

class IPRANDOM {
	public function isvalid_rangea($a) {
		if($a == 127 || $a == 192 || $a == 10 || $a == 0 || $a == 172) {
			return 0;
		}
		return 1;
	}

	public function rpc_makeips($size) {
		$ip_list = array();
		$size--;

		for($i=0;$i<=$size;$i++) {
			do {
				$a = hexdec(bin2hex(openssl_random_pseudo_bytes(1)));//intval(openssl_random_pseudo_bytes(2),10);
			}while(IPRANDOM::isvalid_rangea($a)==0);
			$b = mt_rand(0,255); //intval(openssl_random_pseudo_bytes(2),10);
			$c = rand(0,255); //intval(openssl_random_pseudo_bytes(2),10);
			$d = hexdec(bin2hex(openssl_random_pseudo_bytes(1)));
	
			$ip = $a . "." . $b . "." . $c . "." . $d;
			array_push($ip_list,$ip);
		}		

		$csv="";
		for($i=0;$i<=$size-1;$i++) {
			$csv = $csv . array_pop($ip_list) . ",";
		}

		$csv = $csv . array_pop($ip_list);
		return($csv);
	}
}

?>


