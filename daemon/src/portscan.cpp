/**
 * @file portscan.c
 * @brief Port scan impementation
 * @author JennySec (Thank you fazer for your present millions lines)
**/

#include <bsd/bsd.h>
#include <bsd/stdio.h>
#include <bsd/string.h>
#include <bsd/stdlib.h>
#include <errno.h>
#include <netdb.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <linux/tcp.h>
#include <arpa/inet.h>
#include <libnet.h>
#include <pcap.h>
#include "config.h"
#include "misc.h"
#include "net_utils.h"
#include "portscan.h"



struct portscan_table connections[PORTSCAN_TABLE_MAX] = {0};
static bool portscan_mon = false;

portscan::portscan() {}
portscan::~portscan() {}


//Make pcap monitor to next version
/**
 * @brief This function test open port with SYN TCP Packet
 * @param _IN 	host 	ip or host to scan
 * @param _IN	port 	to scan
 * @returns 	0	close
 *          	1	open
 */
bool portscan::tcp_syn(char *host, int port) 
{
	try {
		libnet_t *libnet;
		libnet_ptag_t tcp, ipv4;
		u_long dst_ip,src_ip;
		u_short dst_port=port,src_port;
		char libnet_errbuf[LIBNET_ERRBUF_SIZE];
		//pcap needs
		pcap_t *pcap;
		char libpcap_errbuf[PCAP_ERRBUF_SIZE];
		struct bpf_program filter;
		bpf_u_int32 mask;
		bpf_u_int32 net;	
		char *str_filter;
		struct pcap_pkthdr header;
		const u_char *packet;
		//char filter_expression[] = "tcp[tcpflags] & (tcp-ack) != 0 and port %d";
		char filter_expression[] = "tcp[tcpflags] & (tcp-ack) != 0 and port %d and src %s";
		struct tcphdr *tcp_recv;
		bool ret=false;

		if(host==NULL || port<=0 || port>65535)
			return(-1);

		//make packet (found)
		if((libnet = libnet_init(LIBNET_RAW4, config::get_interface(), libnet_errbuf))==NULL) {
			__logging("libnet_init not found");
			raise(2);
			abort();
		}


		if((dst_ip = libnet_name2addr4(libnet,host,LIBNET_RESOLVE))==-1) {
			__logging("Invalid resolve ipaddr %s", host);
			return -1;
		}


		libnet_seed_prand(libnet);


		libnet_build_tcp(	src_port = libnet_get_prand(LIBNET_PRu16),
					dst_port,
					libnet_get_prand(LIBNET_PRu16),
					0, //libnet_get_prand(LIBNET_PRu32),
					TH_SYN,
					7, 
					0,
					0,
					LIBNET_TCP_H,
					NULL,
					0,
					libnet,
					0);

					

		libnet_build_ipv4(	LIBNET_TCP_H + LIBNET_IPV4_H,
					0,
					libnet_get_prand(LIBNET_PRu16),
					0,
					127, //libnet_get_prand(LIBNET_PR8),
					IPPROTO_TCP,
					0,
					src_ip = libnet_get_ipaddr4(libnet), 
					dst_ip,
					NULL,
					0,
					libnet,
					0);



		//settings sniff
		if(pcap_lookupnet(config::get_interface(), &net, &mask, libpcap_errbuf)==-1) {
			__logging("error on pcap_lookupnet");
			return -1;
		}

		if((pcap = pcap_open_live(config::get_interface(), BUFSIZ,1,config::get_tcpsyn_timeout(),libpcap_errbuf))==NULL) {
			__logging("error on pcap_open_live");
			raise(2);
			abort();
		}

		//make filter string
		str_filter = (char *)malloc(strlen(filter_expression)+50);
		bzero(str_filter, strlen(filter_expression)+50);
		sprintf(str_filter, filter_expression, src_port, host);
		//sprintf(str_filter, filter_expression, src_port);

		if(pcap_compile(pcap, &filter, str_filter,0,net)==-1) {
			__logging("Error on compile pcap: %s", filter);
			return -1;
		}

		if(pcap_setfilter(pcap, &filter)==-1) {
			__logging("Error on set filter: %s", filter);
			return -1;
		}




		//send packet (found)
		if(libnet_write(libnet)==-1) {
			__logging("error write libnet");
			return -1;
		}


		//sniff pcap
		if((packet = pcap_next(pcap, &header))!=NULL) {
		    tcp_recv = (struct tcphdr *)(packet + LIBNET_IPV4_H + LIBNET_ETH_H);
	    //		if((ntohl(tcp_recv->ack))==1 && (ntohl(tcp_recv->syn))==1) {  // no debian Linux
		    if(tcp_recv->ack==1 && tcp_recv->syn==1) {
		    //if((tcp_recv->th_flags & (TH_SYN|TH_ACK)) == (TH_SYN|TH_ACK)) { //BSD
			    ret=true; //open
		    }
		    else if(tcp_recv->rst==1) {
			    ret=false;
		    }
		    else {
			    ret=false; //close
		    }
		}

		//destroy
		free(str_filter);
		libnet_destroy(libnet);
		pcap_close(pcap);
		return(ret);
	}
	
	catch(int iResult) {
		__logging("Critical error on portscan::tcp_syn");
		return(0);
	}
	
	return(0);
}



/**
 * @brief This function test if TCP port is open with connect method
 * @param _IN	ip 		the ip to connect
 * @param _IN	port 	port to test
 * @returns 	0	 is close.
 *          	1	 is open.
 */
int portscan::tcp_connect(char *ip, int port) {
	int sock;
	socklen_t addr_size;// = {0};
	struct sockaddr_in sin; // = {0};
	struct timeval timeout; // = {0};

	try {
		if(ip==NULL || ip==(void *)-1 || port <= 0 || port > 65535)
			return(0);
		
		if(netutils::isvalidIPV4(ip)==false)
			return(0);

		memset(&sin, 0, sizeof(sin));
		memset(&timeout, 0, sizeof(timeout));
		memset(&addr_size, 0, sizeof(addr_size));

		timeout.tv_sec = config::get_socket_timeout();
		timeout.tv_usec = 0;


		sin.sin_family = AF_INET;
		sin.sin_port = htons(port);
		inet_pton(AF_INET,ip,&sin.sin_addr);
		addr_size = sizeof(sin);


		if ((sock=socket(AF_INET, SOCK_STREAM, IPPROTO_TCP)) == -1){
			return 0;
		}

		if (setsockopt(sock, SOL_SOCKET,SO_SNDTIMEO, (char *)&timeout, sizeof(timeout)) == -1){
			throw 0;
		}

		if (setsockopt(sock, SOL_SOCKET,SO_RCVTIMEO, (char *)&timeout, sizeof(timeout)) == -1){
			throw 0;
		}

		if (connect(sock, (struct sockaddr *)&sin, addr_size) == -1){
			close(sock);
			return 0;
		}

		//   shutdown(sock,0);
		close(sock);
		return(1);
		
	}
	catch(int iResult) {
		if(sock)
			close(sock);
			
		__logging("portscan::tcp_connect exception\n");
		return(0);
	}
	
	return(0);
	
}


/*
 * NOTE untested and not avaliable in v0.1 
 */

int portscan::portscan_add(const char *ip, int port,int protocol) {
  int id = 0;


  while(connections[id].node_status!=false && id < PORTSCAN_TABLE_MAX)
    id++;

  if(id>PORTSCAN_TABLE_MAX)
    return(-1);
  
  
  connections[id].node_status=true;
  connections[id].exec_status=false;
  connections[id].port_status=false;
  
  connections[id].ip = (unsigned char *)__allocate(128);
  strlcpy((char *)connections[id].ip, ip, 128);
  connections[id].port = port;
  connections[id].id = id;
  connections[id].protocol = protocol;
  connections[id].uip = inet_addr(ip);
  connections[id].scan_type = 0;

  //portscan::portscan_send(ip,port); (using libnet)
  
  return(id);
  
}

int portscan::portscan_del(int id) {
  if(id>PORTSCAN_TABLE_MAX)
    return(-1);
  
  
  connections[id].node_status=0;
  connections[id].exec_status=0;
  connections[id].port_status=0;
  connections[id].port = 0;
  connections[id].id = 0;
  connections[id].protocol = 0;
  
  __liberty(connections[id].ip);
  
}

bool portscan::portscan_status(int id) {
  try {
      if(id>PORTSCAN_TABLE_MAX)
	throw 0;
      
      return(connections[id].node_status);
      
    }
    catch(int iResult) {
      switch(iResult) {
	case 0:
	   __logging("portscan::portscan_status: Port scan error invalid id %d", id);
	   break;
	default:
	  __logging("portscan::portscan_status: Unknow Error %d", id);
	  break;
      }
      
      return(false);
    }
  
}


void portscan::portscan_waitexec(int id) {
    try {
      if(id>PORTSCAN_TABLE_MAX)
	throw 0;
      
      if(connections[id].node_status==false)
	throw 1;
      
      while(connections[id].exec_status!=true)
	sleep(0.1);
      
      //return(connections[id].exec_status);
      
    }
    catch(int iResult) {
      switch(iResult) {
	case 0:
	   __logging("portscan::portscan_waitexec: Port scan error invalid id %d", id);
	   break;
	case 1:
	   __logging("portscan::portscan_waitexec: Valid id but his status is disable %d", id);
	   break;
	default:
 	  __logging("portscan::portscan_waitexec: Unknow Error %d", id);
	  break;
      }
      
    }
  
}



bool portscan::portscan_isexec(int id) {
    try {
      if(id>PORTSCAN_TABLE_MAX)
	throw 0;
      
      if(connections[id].node_status==false)
	throw 1;
      
      return(connections[id].exec_status);
      
    }
    catch(int iResult) {
      switch(iResult) {
	case 0:
	   __logging("portscan::portscan_isexec: Port scan error invalid id %d", id);
	   break;
	case 1:
	   __logging("portscan::portscan_isexec: Valid id but his status is disable %d", id);
	   break;
	default:
 	  __logging("portscan::portscan_isexec: Unknow Error %d", id);
	  break;
      }
      
      return(false);
    }
  
}

bool portscan::portscan_isopen(int id) {
    try {
      if(id>PORTSCAN_TABLE_MAX)
	throw 0;
      
      if(connections[id].node_status==false)
	throw 1;
      
      return(connections[id].port_status);
      
    }
    catch(int iResult) {
      switch(iResult) {
	case 0:
	   __logging("portscan::portscan_isopen: Port scan error invalid id %d", id);
	   break;
	case 1:
	   __logging("portscan::portscan_isopen: Valid id but his status is disable %d", id);
	   break;
	default:
	  __logging("portscan::portscan_isopen: Unknow Error %d", id);
	  break;
      }
      
      return(false);
    }
  
}

void portscan::portscan_thread(void *args) {
  try {
    pcap_t *pcap;
    bpf_u_int32 mask;
    bpf_u_int32 net;
    char libpcap_errbuf[PCAP_ERRBUF_SIZE];
    struct bpf_program fp;
    const char filter_expression[] = "tcp[tcpflags] & (tcp-ack) != 0 and port %d and src %s";
    u_char *user=NULL;
    
    if(portscan_mon==true)
      throw 0;
    
    if(pcap_lookupnet(config::get_interface(), &net, &mask, libpcap_errbuf)==-1) {
	    __logging("error on pcap_lookupnet");
	    raise(2);
	    abort();
    }

    if((pcap = pcap_open_live(config::get_interface(), BUFSIZ,1,config::get_tcpsyn_timeout(),libpcap_errbuf))==NULL) {
	    __logging("error on pcap_open_live");
	    raise(2);
	    abort();
    }
    /*
    if(pcap_compile(pcap, &fp, "tcp",0,net)==-1) {
          __logging("error on pcap_compile");
	  raise(2);
	  abort();
    }
      */
    
    
    //pcap_open_live
    
    while(1) {
	int i = 0;
	for(i=0;i<PORTSCAN_TABLE_MAX;i++) {
	  if(connections[i].node_status==true && connections[i].exec_status==false) {
	      char *filter = (char *)malloc(strlen(filter_expression)+128+5);
	      sprintf(filter, filter_expression, connections[i].ip, connections[i].port);
	      //connections[i].port_status = portscan::tcp_syn((char *)connections[i].ip,connections[i].port);
	      //pcap_loop handler
	      if(pcap_compile(pcap,&fp,filter,0,net)!=-1)
		pcap_loop(pcap,0,portscan::portscan_handler,user); // NOTE change to pcap_dispatch
	      
	      connections[i].exec_status=true;
	      free(filter);
	  }
	}
      
    }
   
  }
  catch(int iResult) {
   switch(iResult) {
     case 0:
       __logging("portscan::portscan_thread: Portscan monitor previously executed");
       break;
     default:
       __logging("portscan::portscan_thread: Unknow error");
       break;
   }
  }
}


int portscan::portscan_getid(unsigned long ip, int port) {
    int id = 0;
    for(id=0;id<PORTSCAN_TABLE_MAX;id++) {
	if(connections[id].port==port) {
	    if(connections[id].uip==ip)
	      return(id);
	}
    }
}

void portscan::portscan_handler(unsigned char *user, const struct pcap_pkthdr *packet, const unsigned char *bytes) {
    struct tcphdr *tcp_recv;
    struct iphdr *ip_recv;
    /*
     * In case of no analize packet in handler and send to analyzer queue
    struct portscan_queue *queue = (struct portscan_queue *)malloc(sizeof(packet) + packet->len);
    
   
    memcpy(queue->pkthdr,packet,packet->len);
    memcpy(queue->data, bytes, packet->caplen);
    */
    
    ip_recv = (struct iphdr *)(packet + LIBNET_ETH_H);
    tcp_recv = (struct tcphdr *)(packet + LIBNET_IPV4_H + LIBNET_ETH_H);
   
    
    if(tcp_recv && ip_recv) {
	int id;
	if((id=portscan::portscan_getid(ip_recv->saddr,tcp_recv->dest))!=-1) {
	    if(tcp_recv->ack == 1 && tcp_recv->syn == 1 && connections[id].scan_type==PORTSCAN_TYPE_SYNACK)
	      connections[id].port_status=true;
	    else if(tcp_recv->ack == 1 && connections[id].scan_type==PORTSCAN_TYPE_ACK)
	      connections[id].port_status=true;
	    else if(tcp_recv->rst == 1 && connections[id].scan_type==PORTSCAN_TYPE_RST)
	      connections[id].port_status=true;
	    else if(tcp_recv->fin == 1 && tcp_recv->syn == 1 && connections[id].scan_type==PORTSCAN_TYPE_SYNFIN)
	      connections[id].port_status=true;
	    else if(tcp_recv->fin == 1 && tcp_recv->ack == 1 && connections[id].scan_type==PORTSCAN_TYPE_FINACK)
	      connections[id].port_status=true;
	    else
	      connections[id].port_status=false;
	    
	    connections[id].exec_status = true;
	}
	
    }
  
    //pcap_dispatch()
}