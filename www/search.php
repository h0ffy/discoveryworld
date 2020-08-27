<?php
        require_once('mods/random.php');
        require_once('mods/security.php');
        require_once('mods/db.php');



        function parse_search($query) {

	        $list_args = split(' ', $query);
		
		if(count($list_args)==0)
			$list_args[0] = $query;

		$ret = array(	"country" 	=> "",
				"ip"		=> "",
				"port"		=> "");
	        $i=0;
	        for($i=0;$i<count($list_args);$i++) {
        	        $full = split('[=:]', $list_args[$i]);
        	        $var = $full[0];
			$val = $full[1];
        	       	 switch(strtolower($var)) {
        	       	        case "country":
				case "geo":
					 if(strtoupper($val)!=s_input(strtoupper($val)))
					    s_alert();
					 else
					    $ret["country"] = s_input(strtoupper($val));
        	       	                 break;
        	       	        case "ip":
				case "ipv4":
        	       	                 if(s_inputip($val)==0)
					    s_alert();
					 else
					    $ret["ip"]=s_inputip($val);
        	       	        	break;
        	       	        case "port":
					if(!is_numeric($val))
					    s_alert();
					else
					    $ret["port"]=intval($val);
               		        	break;
               		 }

        	}
		
		if(empty($ret["country"]))
			$ret["country"]="0";
		if(empty($ret["ip"]))
			$ret["ip"]="0";
		if(empty($ret["port"]))
			$ret["port"]="0";
	

		return($ret);

	}


	// MAIN
	$query = $_GET['q'];
	$page = intval($_GET['page']);
	$ntargets = 0;
	//$page=0;
	//$query = "GEO=us";
	$result_ok = false;
	$search_array = parse_search($query);


	if(count($search_array)!=0)
		$result_ok = true;

	
	if($result_ok == true) {
		$db = new db(DB_TYPE_INT);
		$targets_list = $db->search_targets($search_array["ip"], $search_array["country"],$page,100);
		$ntargets = $db->getNTargets();
	}
	
	
	
	
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
		  <li class="active"><a id="menu_link" href="search.php">Search</a></li>
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
			<h3 class="subtitle">Search Results:</h3>
			<?php 
				if(count($targets_list)) {
					$out = 	'<div class="JennysecTables">' .
						'<table><tr><td>IP</td><td>GEO</td>' .
						'<td>N.Ports</td><td>Vulns</td><td>DATE</td></tr>';
					$foot = '</table></div>';
					
					$i=0;
					$rows="";
					for($i=0;$i<count($targets_list);$i++) {
						if(	empty($targets_list[$i][0]) ||
							empty($targets_list[$i][1]) ||
							empty($targets_list[$i][2]) ||
							empty($targets_list[$i][3]) ||
							$i>=100)
								continue;

						$row_format = 	'<td><a href="view.php?tid=%s">%s</a></td>' 	.
								'<td><img class="flag" src="img/flags/%s.png"></img></td>'	.
                                                                '<td>%d</td>' 					.
                                                                '<td>0</td>' 					.
                                                                '<td>%s</td></tr>';
							
						$rows = $rows	. 	sprintf($row_format, 	$targets_list[$i][0],
												$targets_list[$i][1],
												strtolower($targets_list[$i][2]),
												intval($db->getNPORTSbyTID($targets_list[$i][0])),
												$targets_list[$i][3]);
										
					}	
					
					$out = $out . $rows . $foot;
					echo($out);				
				}
				else {
					echo('<h4>No Results</h4>');
				}

			?>
		</div>
		 <div class="w3-center">
		 <ul class="w3-pagination w3-white w3-border">
  			<li><a href="search.php?q=<?php s_echo($query); ?>&page=<?php echo(0); ?>">&laquo;</a></li>
  			<li><a href="search.php?q=<?php s_echo($query); ?>&page=<?php s_echo($page); ?>"><?php s_echo($page+1); ?></a></li>
  			<li><a href="search.php?q=<?php s_echo($query); ?>&page=<?php s_echo($page+1); ?>"><?php s_echo($page+2); ?></a></li>
  			<li><a href="search.php?q=<?php s_echo($query); ?>&page=<?php s_echo($page+2); ?>"><?php s_echo($page+3); ?></a></li>
  			<li><a href="search.php?q=<?php s_echo($query); ?>&page=<?php s_echo($page+3); ?>"><?php s_echo($page+4); ?></a></li>
  			<li><a href="search.php?q=<?php s_echo($query); ?>&page=<?php s_echo($page+4); ?>"><?php s_echo($page+5); ?></a></li>			
			<li><a href="search.php?q=<?php s_echo($query); ?>&page=<?php s_echo($page+1); ?>">&raquo;</a></li>
		</ul>
		</div>
	</div><!-- 	END CONTAINER	-->

	<div class="footer">
		CopyLeft JennySec GPL (JennyCorp)
	</div>
</body>
</html>
