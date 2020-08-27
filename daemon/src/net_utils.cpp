/**
 * @file net_utils.c
 * @brief Network utilities
 * @author JennySec
**/
 
#include <bsd/bsd.h>
#include <bsd/stdio.h>
#include <bsd/string.h>
#include <bsd/stdlib.h>
#include <errno.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <sys/param.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <oping.h>
#include <GeoIP.h>
#include <GeoIPCity.h>
#include "config.h"
#include "misc.h"
#include "net_utils.h"



netutils::netutils(void)  {	this->sock=0; }
netutils::~netutils(void) {
	try {
		if(	this->sock!=0 &&
			this->sock!=-1)
			close(this->sock);
		}
	catch(int iResult) {
			__logging("Error on netutils::~netutils");
	}
}



/**
 * @brief ping to host
 * @param __IN	host	input host for ping he
 * @returns  	0 	if not reply to ping
 *           	1 	is reply 
 */		
int netutils::ping(const char *host) {
	try {
		pingobj_t *ping;
		int ret = 0;

		if((ping=ping_construct())==0)		throw 0;
		if(ping_host_add(ping, host)!=0) 	throw 1;
		if((ret=ping_send(ping))<0)		return 0;
		ping_destroy(ping);
		return(ret);
	}
	catch(int iResult) {
		switch(iResult) {
		  case 0:
		      __logging("netutils::ping: Error ping construct %s", host);
		      break;
		  case 1:
		      __logging("netutils::ping: Error ping host add %s", host);
		      break;
		  default:
		     __logging("netutils::ping: Unknow error %s", host);
		     break;
		}
		
		return(0);
	}
}




/**
 * @brief geoip resolv from 
 * @param __IN	host	input host for ping he
 * @returns  	0 	if not reply to ping
 *           	1 	is reply 
**/		
char *netutils::geoip(const char *host) {
	try {
		GeoIP *geo;
		if(!(geo = GeoIP_new(GEOIP_STANDARD)))	return((char *)ANY);
		const char *buff = GeoIP_country_code_by_addr(geo,host);
		GeoIP_delete(geo);

		if(buff==NULL)
			return((char *)ANY);

		return((char *)buff);
	}
	catch(int iResult) {
		__logging("Error on netutils::geoip %s", host);
		return((char *)ANY);
	}
	
	return((char *)ANY);
}



/**
 * @brief resolve domain ( domain2ip )
 * @param __IN	host	input host to resolve
 * @param __OUT	ip		outut resolved ip
 * @returns  	0 	if not resolve
 *           	1 	is resolve success
 * 
 * @note minimal IPv4 buffer 20bytes
**/	
int netutils::resolve(const char *host, char *ip) {
	
	try {
		struct hostent *h;
		unsigned long *u_ip;
		struct in_addr inaddr = {0};



		if((h = gethostbyname(host))!=0) {
			if(host==0)						return 0;
			if(sizeof(inaddr) != h->h_length)			return 0;	
			inaddr.s_addr = *((unsigned long *)h->h_addr);
			if((u_ip=(unsigned long *)inet_ntoa(inaddr))==NULL)	return 0;
			if(strlcpy(ip, inet_ntoa(inaddr), 20)==0)		return 0;
			
			return 1;
		}
	}
	catch(int iResult) {
			if(ip)
				__logging("Error netutils::resolve %s,%s", host, ip);
			else
				__logging("Error netutils::resolve %s", host);
	}
	
	return 0;
}






/**
 * @brief open tcp connection
 * @param __IN	host	host or ip
 * @param __IN	port	port number
 * @returns  	0 	if not resolve
 *           	1 	is resolve success
**/	
int netutils::tcp_open(const char *host,int port) {
	socklen_t addr_size;// = {0};
	struct sockaddr_in sin; // = {0};
	struct timeval timeout; // = {0};
	char *ip=(char *)malloc(21);
	
		
	
	try {	
		
		bzero(ip, 21);
		bzero(&timeout, sizeof(timeout));
		bzero(&sin, sizeof(sin));
		bzero(&addr_size, sizeof(addr_size));


		timeout.tv_sec = config::get_socket_timeout();
		timeout.tv_usec = 0;
	      
		

		if((this->sock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP))==-1)
			throw 0;

	//	request->sock = sock;

		sin.sin_family = AF_INET;
		if((sin.sin_port = htons(port))==0)	throw 3;
		
		if(netutils::resolve(host,ip)==1)
			inet_pton(AF_INET,ip,&sin.sin_addr);
		else
			inet_pton(AF_INET,host,&sin.sin_addr);
		
		
		addr_size = sizeof(sin);

		if (setsockopt(this->sock, SOL_SOCKET,SO_SNDTIMEO, (char *)&timeout, sizeof(timeout)) == -1) 
			throw 1;
		
		if (setsockopt(this->sock, SOL_SOCKET,SO_RCVTIMEO, (char *)&timeout, sizeof(timeout)) == -1) 
			throw 2;
		
		if(connect(this->sock, (struct sockaddr *)&sin, addr_size) == -1)
		    return 0;


		return(1);
	}
	
	catch(int iResult) {
		switch(iResult) {
		  case 0:
		    __logging("netutils::tcp_open: Error on create sock");
		    break;
		  case 1:
		    __logging("netutils::tcp_open: Error on set send sock timeout");
		    break;
		  case 2:
		    __logging("netutils::tcp_open: Error on set recv sock timeout");
		    break;
		  case 3:
		    __logging("netutils::tcp_open: Error invalid port %s", port);
		    break;
		  default:
		    __logging("netutils::tcp_open: Unknow Error %s,%d", host, port);
		    break;
		}
		
		__logging("netutils::tcp_open: %s, %s, %d\n", ip, host, port);
		return(0);
		
	}
	
	
}

bool netutils::isvalidIPV4(const char *ip) {
	try {
		struct sockaddr_in sa;
		int result = inet_pton(AF_INET, ip, &(sa.sin_addr));
		return result != 0;
	}
	catch(int iResult) {
		__logging("netutils::isvalidIPV4: Unknow Error %s\n", ip);
		return(false);
	}
}


bool netutils::isvalidIPV6(const char *ip) {
    try {
	struct sockaddr_in6 sin6;
	int result = inet_pton(AF_INET6, ip,&(sin6.sin6_addr));
	return result != 0;
    }
    catch(int iResult) {
      __logging("netutils::isvalidIPV6: Unknow Error %s\n", ip);
      return(false);
    }
}

unsigned long netutils::convIPV6toIPV4(const char *ip6) {
    
  return(0L);
}


unsigned short netutils::isIPV4orIPV6(const char *ip) {
    if(strstr(ip, "::")!=NULL)
      return(6);
    else {
      if(inet_addr(ip)!=NULL)
	return(4);
    }
    
    return(0);
}

/**
 * @brief tcp send data 
 * @param __IN	data 	data to send
 * @returns  	sended byte
 *           	0~ (0 or -1) if error 
**/	
int netutils::tcp_send(char *data,int len) {	return(send(this->sock, data, len, 0));	}

/**
 * @brief tcp recv 
 * @param __OUT	data 	memory space to write received data
 * @returns  	sended byte
 *           	~0 (0 or -1) if error 
**/
int netutils::tcp_recv(char *data,int len) {	return(recv(this->sock, data, len, 0));	}


/**
 * @brief tcp close 
 * @param void
 * @returns  	void
 *           	
**/
void netutils::tcp_close(void) {
	close(this->sock);
}

