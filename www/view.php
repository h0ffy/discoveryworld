<?php
	require_once('mods/random.php');
	require_once('mods/security.php');
	require_once('mods/db.php');


	if(!is_numeric($_GET['tid']))
		s_alert();
		
	$tid = intval(s_input($_GET['tid']));

	
		
	/*
	$tid = 13;
	$ip = "8.8.8.8";
	$geo = "US";
	$date = "01/01/2001";
*/

	$db = new db(DB_TYPE_INT);
	$targets_list = $db->search_targets($db->getIPbyTID($tid), "0",0,1);
	$scans_list = $db->search_scans(intval($targets_list[0][0]));
	$domains_list = $db->search_domains(intval($targets_list[0][0]));
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
	<link rel="stylesheet" href="./css/search_tables.css" type="text/css">
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
		  <li><a id="menu_link" href="configuration.php">Configuration</a></li>
		  <li><a id="menu_link" href="about.php">About</a></li>
		</ul>

<div id="wrapper" style="margin-top:5%;">
        <form method="get" action="search.php">
                <input type="text" id="df" name="q" value=""/>
                <input type="submit" value="Search" />
        </form>
</div><!-- #wrapper -->

		<div class="result" id="result_main">
			<div id="serv_data">
				<ul>
					<li id="ip"><a  href="./view.php?tid=<?php echo($targets_list[0][0]); ?>"><?php echo($targets_list[0][1]); ?></a></li>
					<li id="geo"><img width="15px" height="10px" src="img/flags/<?php echo(strtolower($targets_list[0][2])); ?>.png">Spain</li>
					<li id="scan_date">Added on <?php echo($targets_list[0][3]); ?></li>
					<br>
					<li id="os">Linux 3.0.15</li>
					<li id="services">
						<img width="25px" height="25px" src="img/icons/ssh.png" alt="SSH">
						<img width="25px" height="25px" src="img/icons/http.png" alt="HTTP">
						<img width="25px" height="25px" src="img/icons/https.png" alt="HTTPS">
						<img width="25px" height="25px" src="img/icons/tor.png" alt="TOR">
					</li>
				</ul>
			</div>
		<h2 class="subtitle">Discovery</h2>
		<?php
				// TARGET TABLE
                                if(count($targets_list)) {
                                        $out =  '<div class="JennysecTables">' .
                                                '<table><tr><td>IP</td><td>GEO</td>' .
                                                '<td>N.Ports</td><td>Vulns</td><td>DATE</td></tr>';
                                        $foot = '</table></div>';

                                        $row_format =   '<td><a href="view.php?tid=%s">%s</a></td>' 			.
                                                        '<td><img class="flag" src="img/flags/%s.png"></img></td>' 	.
                                                        '<td>%d</td>' 							.
                                                        '<td>0</td>' 							.
                                                        '<td>%s</td></tr>';
					$rows = "";
                                        $rows = $rows   .       sprintf($row_format,    $targets_list[0][0],
                                                                                        $targets_list[0][1],
                                                                                        strtolower($targets_list[0][2]),
											intval($db->getNPORTSbyTID($targets_list[0][0])),
											$targets_list[0][3]);
					$out = $out . $rows . $foot;
					echo($out);
				}
		?>

		<?php
				//DOMAINS TABLE
				$html_domains_head		=	'<h2 class="subtitle" style="margin-left:19.7%;">'	.
									'Domains</h2><div class="JennysecTables"><table><tr>'	.
									'<td>DOMAIN</td><td>IP</td></tr>';
				$html_domains_row_format	=	'<tr><td>%s</td><td>%s</td></tr>';
				$html_domains_foot		=	'</table></div>';

				
				$html = $html_domains_head;

				for($i=0;$i<count($domains_list);$i++) {
					if(empty($domains_list[$i][0]))
						continue;

					$html = $html . sprintf(	$html_domains_row_format, $domains_list[$i][0] ,
									$targets_list[0][1]);
				}
				$html = $html . $html_domains_foot;

				echo($html);
		?>


			<?php

				$html_finger_format = 	'<div id="serv_finger">' 					.
							'<h3 class="subtitle">%s (%s)</h3>' 					.
							'<div id="finger_type">Reply</div>' 				.
							'<textarea id="finger">%s</textarea>'		 		.
							'</div>';

				$html_cert_format =	'<div id="serv_finger">' 					.
							'<div id="finger_type">Cert</div>' 				.
							'<textarea id="finger">%s</textarea>' 				.
							'</div>';
							
				for($i=0;$i<count($scans_list);$i++) {
					if(empty($scans_list[$i][0]))
						continue;

					$html = sprintf($html_finger_format, 	$scans_list[$i][0], $scans_list[$i][4],
									 	$scans_list[$i][1]);

					if(!empty($scans_list[$i][2]))
						$html = $html . sprintf($html_cert_format, $scans_list[$i][2]);


					echo($html);
					$html = "";
				}		
					

			?>

		</div>
	</div>

	<div class="footer">
		CopyLeft JennySec GPL (JennyCorp)
	</div>
</body>
</html>
