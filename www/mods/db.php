<?php
	//require_once("depends/medoo.php");
	require_once("config.php");
	require_once("mods/security.php");

	class db {
		private $database;
		public $db_type;
				
		public function db($db_type) {
		
			switch($db_type) {
 				case 0:
				default:
					$this->database = new mysqli(DB_SERVER, DB_USER, DB_PASS, DB_NAME);
					break;
				case 1:
					//oracle
				case 2:
					//infomix
				case 3:
					//mariadb (is not compatible)
				case 4:
					//berkeley db
				case 5:
					//postgres sql
				case 6:
					//mongo db
					break;
			}
				
		}

		public function is_hosts_exists($value) {
			$i=0;
			$stmt = $this->database->prepare("SELECT TID,IP from targets WHERE IP=?");
			$stmt->bind_param("s",$out_value);
			$out_value=$value;
		
			$stmt->execute();
			$stmt->bind_result($tid,$ip);
			while($stmt->fetch())
				$i++;
				
			if($i!=0) {
				if($value==$ip)
					$i=1;
				else
					$i=0;
			}
	
			return($i);	
		}


                public function getTIDbyIP($value) {
                        $tid=-1;
                        $stmt = $this->database->prepare("SELECT TID,IP from targets WHERE IP=?");
                        $stmt->bind_param("s",$out_value);
                        $out_value=$value;

                        $stmt->execute();
                        $stmt->bind_result($tid,$ip);
                        $stmt->fetch();
			                               

                        if($tid!=-1) {
                                if($value!=$ip)
                                        $tid=-1;
                        }

			$stmt->close();
                        return($tid);
                }



                public function getIPbyTID($value) {
                        $ip=-1;
                        $stmt = $this->database->prepare("SELECT TID,IP from targets WHERE TID=?");
                        $stmt->bind_param("d",$out_value);
                        $out_value=$value;

                        $stmt->execute();
                        $stmt->bind_result($tid,$ip);
                        $stmt->fetch();


                        if($tid!=-1) {
                                if($value!=$tid)
                                        $ip=-1;
                        }

                        $stmt->close();
                        return($ip);
                }



                public function getNPORTSbyTID($value) {
                        $nports=-1;
                        $stmt = $this->database->prepare("SELECT count(*) from scans WHERE TID=?");
                        $stmt->bind_param("s",$param_value);
                        $param_value=$value;

                        $stmt->execute();
                        $stmt->bind_result($nports);
                        $stmt->fetch();
			$stmt->close();
                        return($nports);
                }

		public function getNTargets() {
			$stmt = $this->database->prepare("SELECT count(*) FROM targets");
			$stmt->execute();
			$stmt->bind_result($out_count);
			$stmt->fetch();
			$stmt->store_result();
			$stmt->close();
			return($out_count);
		}
		
                
                public function search_targets($ip,$country,$start_page,$count) {
                        $query = "SELECT TID,IP,GEO,LAST from targets ";
			$first = 0;
			$ret[100][4] = (0);

			if(!empty($country) && $country!="0") {
				$query = $query . "WHERE GEO=?";
			}
			else {
				if(!empty($ip) && $ip!=null && $ip!=0) {
					$query = $query . "WHERE IP=?";
				}
				else {
					return(0);
				}
				$first++;
			}

			$start_page*=100;

			$end_page = $start_page + $count;
			$query = $query . " LIMIT " . $start_page . " , " . $end_page;
			$stmt = $this->database->prepare($query);
			$stmt->bind_param("s", $out_value);
			if($first==0)
				$out_value = $country;
			else
                        	$out_value = $ip;
			

                        $stmt->execute();
                        $stmt->bind_result($out_tid,$out_ip,$out_country,$out_date);
                	$i=0;
		        while($stmt->fetch()) {
				if($i>100)
					continue;
				
				$ret[$i][0] = $out_tid;
				$ret[$i][1] = $out_ip;
				$ret[$i][2] = $out_country;
				$ret[$i][3] = $out_date;

				$i++;
			}

				
                        $stmt->close();
                        return($ret);
                }


		public function search_scans($tid) {
                        $query = "SELECT SERVICE,FINGER,CERT,LAST,PORT from scans WHERE TID=?";
			$ret[65535][4] = (0);

			$stmt = $this->database->prepare($query);
			$stmt->bind_param("d", $out_value);
			$out_value = $tid;
			

                        $stmt->execute();
                        $stmt->bind_result($out_service,$out_finger,$out_cert,$out_date,$out_port);
                	$i=0;
		        while($stmt->fetch()) {
				if($i>65535)
					continue;

				$ret[$i][0] = $out_service;
				$ret[$i][1] = $out_finger;
				$ret[$i][2] = $out_cert;
				$ret[$i][3] = $out_date;
				$ret[$i][4] = $out_port;

				$i++;
			}

				
                        $stmt->close();
                        return($ret);
			
		}

		public function search_domains($tid) {
                        $query = "SELECT DOMAIN,LAST FROM domains WHERE TID=?";
			$ret[25000][2] = (0);

			$stmt = $this->database->prepare($query);
			$stmt->bind_param("d", $out_value);
			$out_value = $tid;
			

                        $stmt->execute();
                        $stmt->bind_result($out_domain,$out_date);
                	$i=0;
		        while($stmt->fetch()) {
				if($i>25000)
					continue;

				$ret[$i][0] = $out_domain;
				$ret[$i][1] = $out_date;
				$i++;
			}

				
                        $stmt->close();
                        return($ret);
			
		}

		public function is_exists_table($table) {
			$table = s_input($table);
			$prep = $this->database->prepare("SHOW TABLES LIKE ?");
			$prep->execute($table);
			if($this->database->affected_rows>=0) {
				return(1);
			}
			return(0);
		}
	
		public function list_databases() { 
			$prep = $this->database->prepare("SHOW DATABASES");
			$prep->bind_result($dblist);
			$prep->execute();
			if($this->database->affected_rows>=0) {
				return 0;						
			}

			return 1;
		}

		
		
		public function is_exists_database($dbname) {
			$dbname = s_input($dbname);
			$prep = $this->database->prepare("SHOW DATABASES LIKE ?");
			$prep->execute($dbname);
			if($this->database->affected_rows>=0) {
				return(1);
			}
	
			return(0);
		}
	
		public function input_security($str) {
			return($this->database->real_escape_string($str));
			
		}


		public function insertUser($user,$pass) {
                        $uid=0;
			$stmt = $this->database->prepare("INSERT INTO users SET USER=?,PASS=?,TOKEN=?");
                        $stmt->bind_param("sss",$out_user,$out_pass,$out_token);
                        $out_user=$user;
			$out_pass=md5($pass);
			$out_token=md5(	(md5($user) . md5(md5($pass)) . md5("AAAAA")) );

                        $stmt->execute();
                        $stmt->fetch();
			$this->database->commit();
			$stmt->reset();
			$stmt->prepare("SELECT UID FROM users WHERE USER=? AND PASS=?");
			$stmt->bind_param("ss", $out2_user, $out2_pass);
			$out2_user=$user;
			$out2_pass=$pass;
			$stmt->execute();
			$stmt->bind_result($uid);
                        $stmt->fetch();
			$stmt->close();
			echo($uid);                   
                        return($uid);
		}

		public function insertSession($uid) {
			$stmt = $this->database->prepare("INSERT INTO sessions (UID,STATUS,SESSION_TOKEN) VALUES ?,?,?");
                        $stmt->bind_param("dbs",$out_uid,$out_status,$out_token);
                        $out_uid=$uid;
			$out_status=true;
			$out_token=md5(	md5($uid) . md5(str(get_date()))	);
			$stmt->execute();
			$stmt->fetch();
			$stmt->close();
			return($out_token);
		}

		
		public function is_valid_user($user) {
			$stmt = $this->database->prepare("SELECT UID FROM users WHERE USER=?");
			$stmt->bind_param("s", $out_user);
			$out_user=$user;
			$stmt->execute();
			$stmt->fetch();
			$stmt->store_result();
			$ret = false;
		
			if($stmt->num_rows==1)
				$ret=true;
	
	
			$stmt->close();
			return($ret);
		}


                public function getUIDbyUser($user) {
                        $stmt = $this->database->prepare("SELECT UID,USER from users WHERE USER=?");
                        $stmt->bind_param("s", $out_user);
                        $out_user=$user;
                        $stmt->execute();
			$uid=-1;
			$res_user="";
			$stmt->bind_result($uid,$res_user);
                        $stmt->fetch();
                        $stmt->store_result();
			$stmt->close();
                        if($res_user==$user)
                                return($uid);

                        return(0);

                }

                public function is_valid_pass($uid,$user,$pass) {
                        $stmt = $this->database->prepare("SELECT PASS from users WHERE USER=? AND UID=?");
                        $stmt->bind_param("sd", $out_user,$out_uid);
                        $out_user=$user;
			$out_uid=$uid;
                        $stmt->execute();
			$res_pass="";
			$stmt->bind_result($res_pass);
                        $stmt->fetch();
			$stmt->store_result();
                        $stmt->close();
                        if($res_pass==$pass)
                                return(true);

                        return(false);
                }




		public function is_session($uid) {
			$stmt = $this->database->prepare("SELECT STATUS from users WHERE UID=?");
			$stmt->bind_param("d", $out_uid);
			$out_uid=$uid;
			$stmt->execute();
			$stmt->bind_result($status);
			$stmt->fetch();
			$stmt->store_result();
			$stmt->close();
			return(status);		
		}

	


	}


?>
