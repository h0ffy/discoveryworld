<?php
        require_once('mods/random.php');
        require_once('mods/security.php');
        require_once('mods/db.php');
      
	if(($db = new db(0)))
	  $ntargets = $db->getNTargets();
        else
	  $ntargets = 0;

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
		  <li class="active"><a href="main.php">Main</a></li>
		  <li><a id="menu_link" href="search.php">Search</a></li>
		  <li><a id="menu_link" href="configuration.php">Configuration</a></li>
		  <li><a id="menu_link" href="about.php">About</a></li>
		</ul>

<!--
<div id="wrapper">
	<form method="get" action="">
		<input type="text" id="df" value=""/> 
		<input type="submit" value="Search" />
	</form>
</div> #wrapper -->

		<div class="result" id="result_main">
			<h3 class="subtitle">Top 20 of last scanned of (Total: <?php echo(strval($ntargets)); ?>)</h3>
			<div class="JennysecTables">
				<table>
					<tr>
						<td>DATE</td>
						<td>IP</td>
						<td>GEO</td>
						<td>N.Ports</td>
						<td>Vulns</td>
					</tr>
					<tr><td>01/01/01</td><td>8.8.8.8</td><td>US</td><td>2</td><td>20 (Critical)</td></tr>
					<tr><td>01/01/01</td><td>8.8.8.8</td><td>US</td><td>2</td><td>20 (Critical)</td></tr>
					<tr><td>01/01/01</td><td>8.8.8.8</td><td>US</td><td>2</td><td>20 (Critical)</td></tr>
					<tr><td>01/01/01</td><td>8.8.8.8</td><td>US</td><td>2</td><td>20 (Critical)</td></tr>
				</table>
			</div>
		</div>
	

	<div class="footer">
		CopyLeft JennySec GPL (JennyCorp)
	</div>
</body>
</html>
