<?php
        require_once('mods/random.php');
        require_once('mods/security.php');
        require_once('mods/db.php');
	require_once('mods/sys.php');
	
	$sys = new SYS();

	$procs_daemon_list 	= $sys->listPROCbyNAME(DAEMON_PROCESS);
	$procs_agent_list	= $sys->listPROCbyNAME(AGENT_PROCESS);


?>

<!DOCTYPE html PUBLIC "-//W3C//DTD HTML 4.01 Strict//EN" "http://www.w3.org/TR/html4/strict.dtd">
<html>
<head>
	<meta http-equiv="Content-Type" content="text/html; charset=ISO-8859-15">
	<link href="./css/main.css" type="text/css" rel="stylesheet" />
	<meta name="viewport" content="width=device-width, initial-scale=1">
	<link rel="stylesheet" href="./css/w3.css">
	<link rel="stylesheet" href="./css/bootstrap.min.css">
	<link rel="stylesheet" href="./css/search.css" type="text/css">
	<link rel="stylesheet" href="./css/search_result.css" type="text/css">
	<link rel="stylesheet" href="./css/main_tables.css" type="text/css">
	<script src="./js/jquery.min.js"></script>
	<script src="./js/bootstrap.min.js"></script>
	<script src="./js/raphael-2.1.4.min.js"></script>
	<script src="./js/justgage.js"></script>
	<script src="./js/morris.min.js"></script>
	<title>DiscoveryWorld</title>
	
</head>
<body>
	<div class="header">
		<div class="w3-right toptext w3-wide">
			<span id="jsec_head">DiscoveryWorld</span><br />
			<span style="color:white;">NETWORK DISCOVERY TOOL</span>
		</div>
			<img id="jsec_img_head" src="img/logo001.jpg" alt="JennySec">
		</div>

	<div class="container">
		<ul class="nav nav-pills w3-topnav">
		  <li><a id="menu_link" href="main.php">Main</a></li>
		  <li><a id="menu_link" href="search.php">Search</a></li>
		  <li class="active"><a id="menu_link" href="configuration.php">Configuration</a></li>
		  <li><a id="menu_link" href="about.php">About</a></li>
		</ul>

		<div class="result" id="result_main">
        <?php
                $html_process_head              =      	'<h3 class="subtitle">Process Status</h3>'	.
							'<div class="JennysecTables"><table><tr>'	.
                                                        '<td>Process</td><td>PID</td></tr>';
                $html_process_row_format        =       '<tr><td>%s</td><td>%s</td></tr>';
                $html_process_foot              =       '</table></div>';
                $html = $html_process_head;
                for($i=0;$i<count($procs_daemon_list);$i++) {
                        if(empty($procs_daemon_list[$i]))
                                continue;

                        $proc = split('[:;#]', $procs_daemon_list[$i]);
                        $html = $html . sprintf($html_process_row_format,$proc[0],$proc[1]);
                }
                for($i=0;$i<count($procs_agent_list);$i++) {
                        if(empty($procs_agent_list[$i]))
                                continue;

                        $proc = split('[:;#]', $procs_agent_list[$i]);
                        $html = $html . sprintf($html_process_row_format,$proc[0],$proc[1]);
                }
                $html = $html . $html_process_foot;
                echo($html);
        ?>
	
	

		</div>
	</div><!-- 	END CONTAINER	-->
	
	<div class="footer">
		CopyLeft JennySec GPL (JennyCorp)
	</div>
</body>
</html>
