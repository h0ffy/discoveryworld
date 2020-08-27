<?php
	require_once('mods/random.php');
	require_once('mods/security.php');
	require_once('mods/db.php');



//	$option = intval(s_input($_GET['option']));
//	$data 	= s_input(base64_decode($_GET['data']));
		

	/*
		if(!is_numeric($_GET['option']))
			s_alert();
		if($data != base64_decode($_GET['data']))
			s_alert();
	*/

	$data = "8.8.8.4";
	$option = 2;

	if(empty($data))
		$option=0x00;


	switch($option) {
		case 0x00: 						//MAKE RANDOM IP
			s_echo($iprandom->rpc_makeips(10));
			break;
		case 0x01:						//HOST EXISTS
			$db = new db(DB_TYPE_INT);
			$ret = $db->is_hosts_exists($data);
			s_echo($ret);
			break;	
		case 0x02:						//Get TID by IP
			$db = new db(DB_TYPE_INT);
			if($db->is_hosts_exists($data)==1) {
				if(($tid = $db->getTIDbyIP($data))!=-1)
					s_echo($tid);
				else
					s_echo("error");
			}
			break;
		case 0x03:
			
		default:
			s_echo("Invalid option");
			break;
	}


?>


