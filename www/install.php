<?php
	require_once("config.php");
	require_once("mods/security.php");
	require_once("mods/db.php");

	$user	=	$argv[1];
	$pass	=	$argv[2];

	$db = new db(0);

	if($db->is_valid_user($user)==true)
		printf("The user %s exists\n", $user);
	else {
		if($db->insertUser($user,$pass)!=0)
			printf("Ok created user %s\n", $user);
		else
			printf("Error on create user\n");
	}  
	

?>
