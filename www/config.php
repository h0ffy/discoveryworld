<?php
	define("DB_TYPE", "mysql");
	define("DB_TYPE_INT", 0);
	define("DB_SERVER", "localhost");
	define("DB_NAME", "discoveryworld");
	define("DB_USER", "root");
	define("DB_PASS", "jennylab");
	define("DB_PORT", 3306);
	define("DB_TABLE", "scans");

	define("DAEMON_PROCESS", "discoveryd");
	
	// BASH FOR DEBUG
	//define("AGENT_PROCESS", "discoveryagent");
	define("AGENT_PROCESS", "discoveryagent.py");
?>
