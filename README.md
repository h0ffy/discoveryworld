Aplicacion de descubrimiento y analisis de maquinas de internet


	/daemon: Demonio en C para el descubrimiento, fingerprint y portscan
	/framework: Scripts en python para la extracción de dominios y otras utilidades de la aplicacion
	/web: Entorno web PHP/HTML/JavaScript para la visualización de la informacion
	db.txt: Informacion sobre la estructura de la base de datos




*daemon:
	
	*features:
		*	IPV4
		*	Adquisition IPV4 random IP and file list input
		*	Multithread
		*	Multiprocess
		* 	Port scan tcp connect and tcp syn
		* 	Scan and connect over VPN
		* 	Multiport scan and figer
		* 	Multiservice (HTTP/S, SSH, FTP/S, TELNET, GENERIC/S)
		*	Geo IP location
		*	Savee data into MySQL database
		*	Comunication with agent
		*	Configuration file 
		*	Save scan state with resume possiblity
		*	Max thread 75, Max process 10 
	
	*roadmap:
		*	Add correct mysql output (conver output.cpp in monitor thread) 
		*	Multi network interfaces
		*	Multi agents output
		*	Multi threads tcp syn scan
		*	PF or BPF socket (case in study)
		*	Port scan UDP
		*	UDP protocols (no disclosured all)
		*	Scan and connect over TOR
		*	IPV6 (not disclosured) 
		*	Geo IP location city and ISP infomation
		*	Adquisition IPV6 (not disclosured)
		*	Adquisition method MySQL and socket
		*	Most tcp scan methods
		*	Most service finger methods (not disclosured all)
		*	Automatic protocol detection (not disclosured)
		*	Connection over jennyproxy


*agent:

	*features:
		*	Socket listen
		*	Save into MySQL database
		*	IP to Domain
		*	Web fingerprint (whatweb)
		*	CMS version detect
		*	DNS information and brute forcing
		*	Whois IP 
		*	Discoveryword lite version

	*roadmap:
		*	IPV6
		*	Output over http/s with PHP RPC



