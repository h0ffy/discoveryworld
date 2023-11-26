<?php

class SYS {
	public function listPROCbyNAME($name) {
		$ret = shell_exec(sprintf('ps auxxw | grep -i %s | grep -v grep | awk \'{print $11 " " $12 " " $13 ":" $2}\'', escapeshellcmd($name)));
		return(explode("\n",$ret));
	}
}

?>


