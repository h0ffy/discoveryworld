/**
 * @file discovery.c
 * @brief Discovery scanner
 * @author JennySec
**/
#include <bsd/bsd.h>
#include <bsd/stdio.h>
#include <bsd/stdlib.h>
#include <bsd/string.h>
#include <bsd/unistd.h>
#include <fcntl.h>
#include <signal.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <pthread.h>
#include <openssl/ssl.h> 
#include <mbedtls/compat-1.3.h>
#include <mbedtls/config.h>
#include <mbedtls/net.h>
#include <mbedtls/ssl.h>
#include <mbedtls/entropy.h>
#include <mbedtls/ctr_drbg.h>
#include <mbedtls/error.h>
#include <mbedtls/certs.h>
#include "config.h"
#include "net_utils.h" 
#include "portscan.h"  
#include "misc.h"
#include "threads.h"
#include "modules/protocol_http.h"
#include "modules/module_ssl.h"
#include "modules/module_https.h"
#include "modules/module_ssh.h"
#include "modules/module_agent.h"
#include "discovery.h"  


               
discovery::discovery(void) {}                
discovery::~discovery(void) {}


/**
 * @brief Main function of threads discovery
 * @param _IN	st_scan	
 * @returns 	false
 *          	true
 * 
 * @note read foot note about untested memory leak
 */
void *discovery::scan(void *args) {
	struct st_scan *scan = (struct st_scan *)args;
	struct st_output_node node;
	int pingstat=0;
	int i=0;
	int last_open_port=0;
	bool status=false,discovery_status=false;
	int open_ports[65536]={0};
	unsigned long long tid;
	int http_len=0;
	int https_len=0;
	char *tmp_path;
	char *tmp_https;
	//bzero(open_ports,65535);
	

try {
		node.port 	= 0;
		node.tid	= 0L;
		node.proto	= 0; 
		if((node.finger = (char *)malloc(MAX_OUTPUT_FINGER)) == NULL) throw -1;
		else
		   bzero(node.finger,MAX_OUTPUT_FINGER);
		if((node.cert = (char *)malloc(32000)) == NULL) throw -1;
		else
		  bzero(node.cert,32000);
		if((node.service = (char *)malloc(50)) == NULL) throw -1;
		else
		  bzero(node.service,50);
		if(scan->ip==NULL || scan->ip==(const char *)0x00000000) throw 0;
		 
		
		pingstat=netutils::ping((const char *)scan->ip);	

	
	
		 //__logging("Test it %d %s\n",scan->index, scan->ip);
	
		 //threads::scan_del(scan->index);
	
		/*while(threads::portscan_getmutex()==1) {
			sleep(0.1);
		}*/
		
		if(threads::portscan_getmutex()==0) {
		threads::portscan_setmutex(1);
					int b=0;
					int port;
					while((port=config::get_port(b))) {
						if(port==0)
							continue;
						if(portscan::tcp_syn((char *)scan->ip,port)==1) {
								__logging("IP: %s\tPort: %d\tOPEN\n", scan->ip,port);
								status=true;
								pingstat=1;
								open_ports[last_open_port]=port;
								last_open_port++;
						}
						else {
								__logging("IP: %s\tPort: %d\tCLOSE\n", scan->ip,port);
						}
							
						b++;
					}
		threads::portscan_setmutex(0);
		}
		else {
					int b=0;
					int port;
					while((port=config::get_port(b))) {
						if(port==0)
							continue;
						if(portscan::tcp_connect((char *)scan->ip,port)==1) {
								__logging("IP: %s\tPort: %d\tOPEN\n", scan->ip,port);
								status=true;
								pingstat=1;
								open_ports[last_open_port]=port;
								last_open_port++;
						}
						else {
								__logging("IP: %s\tPort: %d\tCLOSE\n", scan->ip,port);
						}
							
						b++;
					}
			
		}

		// prealpha version not use services.cpp
		if(status==true) {
			output *out = new output();
			agent *ag = new agent();
			
			tid=out->target(scan->ip, netutils::geoip(scan->ip),pingstat);
			ag->send_resolve(scan->ip);
	        
			
			
			//v0.2: change to service.cpp object poll control
			http *lhttp 	= 	new http();
			https *lhttps 	=	new https();
			ssh *sh 	= 	new ssh();
			netutils *net 	= 	new netutils();
			
    
			
			
            //The next switch only use in alpha version after use more protocols and need implements struct with protocol name + pointer_to_function to protocol module (module_XXXXX.cpp)
			int c=0;
			for(c=0;c<last_open_port;c++) {
				if(open_ports[c]==0)
					continue;
				
				/*ag->send_resolve(scan->ip);*/ //temporary disable for debug remember uncomment this line
				//__logging("Port: %d\tIP: %s\t[OK]\n", open_ports[c],scan->ip);
				switch(config::get_servicebyport(open_ports[c])) {
					case 0:	//HTTP CASE	
						//printf("OK HTTP OPEN PORT %s:%d\n", scan->ip,open_ports[c]);
						__logging("OK HTTP OPEN PORT %s:%d\n", scan->ip,open_ports[c]);
						if((http_len = lhttp->finger(scan->ip,open_ports[c]))) {
							strlcpy(node.finger,(const char *)lhttp->reply,MAX_OUTPUT_FINGER);
							strlcpy(node.cert,(const char *)ANY, 4);
							strlcpy(node.service,(const char *)"HTTP",5);
							node.proto=0;
							node.status=1;
							discovery_status=true;
						}
						
						//ag->send_resolve(scan->ip); 
						ag->send_web(scan->ip,open_ports[c],(char *)"HTTP");
					
						break;
					case 1:
						//printf("OK HTTPS OPEN PORT %s:%d\n", scan->ip,open_ports[c]);
						__logging("OK HTTPS OPEN PORT %s:%d\n", scan->ip,open_ports[c]);
						while(threads::httpsfinger_getmutex()==1) {
							sleep(0.1);
						}
						threads::httpsfinger_setmutex(1);
                            			//tmp_https = https_finger(scan->ip, open_ports[c]);
						lhttps->finger(scan->ip,open_ports[c]);
                        			threads::httpsfinger_setmutex(0);
						
						if(lhttps->reply_count==0 || lhttps->reply==NULL)
						  break;
						
						strlcpy(node.finger,(const char *)lhttps->reply,MAX_OUTPUT_FINGER);
						strlcpy(node.cert,(const char *)ANY, 4);
						strlcpy(node.service,(const char *)"HTTPS",6);
						node.proto=0;
						node.status=1;
						discovery_status=true;
						free(tmp_https);
						ag->send_web(scan->ip,open_ports[c],(char *)"HTTPS");
						break;
					
					
					case 2: //not tested
						__logging("OK TELNET OPEN PORT %s:%d\n", scan->ip,open_ports[c]);
						if(net->tcp_open(scan->ip,open_ports[c])==1) {
							if(net->tcp_recv(node.finger, 32000)>1) {
								strlcpy(node.cert,(const char *)ANY, 4);
								strlcpy(node.service,(const char *)"TELNET",7);
								node.proto=0;
								node.status=1;
								discovery_status=true;		
							}
							net->tcp_close();
						}
						break;
					case 3: //not tested
						__logging("OK FTP OPEN PORT %s:%d\n", scan->ip,open_ports[c]);
						if(net->tcp_open(scan->ip,open_ports[c])==1) {
							if(net->tcp_recv(node.finger, 2000)>1) {
								if(net->tcp_send((char *)"SYST\n\r", 8)>1)
									net->tcp_recv(node.finger+strlen(node.finger)-1, 8000-strlen(node.finger));
									
								strlcpy(node.cert,(const char *)ANY, 4);
								strlcpy(node.service,(const char *)"FTP",4);
								node.proto=0;
								node.status=1;
								discovery_status=true;
							}
						}
						break;
					
					case 4:	//SSH Services Case
						sh->finger(scan->ip,open_ports[c]);
						node.proto=0;
						node.status=1;
						strlcpy(node.finger,sh->reply,32000);
						strlcpy(node.service,(const char *)"SSH", 4);
						strlcpy(node.cert,sh->cert,32000);
						discovery_status=true;
						break;
					
					default:
						__logging("OPEN PORT %s:%d (UNKNOW SERVICE)\n", scan->ip,open_ports[c]); 
						break;	
				}
				
				
				if(node.status==1) {
					node.tid=tid;
					node.port=open_ports[c];
					out->push(&node);
				}
				bzero(node.finger,MAX_OUTPUT_FINGER);
				bzero(node.cert,32000);
				bzero(node.service, 50);
				node.status = node.port = node.proto = 0;
				
			}
			
			
			if(discovery_status==true)
				out->build();
		
			
			delete lhttp;
			delete lhttps;
			delete net;
			delete sh;
			delete ag;
			delete out;			
		}			
			


		free(node.finger);
		free(node.cert); 
		free(node.service);
		node.status = node.port = node.proto = 0;
		node.tid = 0L;
		/*
		 * @note: Test if this free leak memory (is correctly but is allocated by main threads)
		 */
		//free(scan->ip); // test if leak or not (valgrind yeah!).
	
	}
	catch(int iResult) {
		if(iResult==-1)
			__logging("Critical memory error");
		else if(iResult==0)
			__logging("Invalid IP");
		else
			__logging("Unknow critical error");
		//throw iResult;
	}
	
	threads::scan_del(scan->index);
	
}

/*
// this call module method implementation for next version non functional in alpha version
int mod_http(char *ip, int port, st_output_node *node) {
	http *lhttp = new http();
	int http_len;
	
	
	//#ifdef DEBUG
	__logging("OK HTTP OPEN PORT %s:%d\n", ip,port);
	//#endif
	
	http_len = lhttp->finger(ip,port);
	strlcpy(node->finger,(const char *)lhttp->reply,MAX_OUTPUT_FINGER);
	strlcpy(node->cert,(const char *)ANY, 4);
	strlcpy(node->service,(const char *)"HTTP",5);
	
	node->proto=0;
	node->status=1;
	//agent *ag = new agent();
//	discovery_status=true;
//	ag->send_web(scan->ip,open_ports[c],(char *)"HTTP");
	return(0);
}
*/
