/**
 * @file modules/module_agent.c
 * @brief DiscoveryWorld Agent Client 
 * @author JennySec
 * 
**/

#include <bsd/bsd.h>
#include <bsd/stdio.h>
#include <bsd/string.h>
#include "../config.h"
#include "../net_utils.h"
#include "../misc.h"
#include "module_agent.h"


agent::agent() {
	try {
	  if((this->request = (char *)__allocate(MAX_AGENT_BUFF*2))==NULL)	throw;
	}

	catch(int iResult) {
	  perror("No memory allocation is possible agent::agent");
	  exit(0);
	} 
		
}

agent::~agent() {
	__liberty(this->request);
}

int agent::send_resolve(char *ip) {
	try {
		netutils *net = new netutils();
		char *buff=(char *)__allocate(MAX_AGENT_BUFF*2);
		//bzero(buff, MAX_AGENT_BUFF*2);
		sprintf(this->request, "%s;%s;0;RESOLVE\n\r", config::get_agenttoken(), ip);	
		
		if(net->tcp_open(config::get_agentip(),0xC0FE)==1) {
		  net->tcp_send(this->request,strlen(this->request));
		  bzero(buff,MAX_AGENT_BUFF);
		  
		  while(net->tcp_recv(buff,MAX_AGENT_BUFF))
		    bzero(buff,MAX_AGENT_BUFF);
			
		}
		else {
		    delete(net);
		    __liberty(buff);
		    throw 1;
		}
		
		free(buff);
		net->tcp_close();
		delete(net);
		return(0);
	}
	catch(int iResult) {
	  switch(iResult) {
	    case 1:
		__logging("agent::send_resolve: Connection error\n");
		break;
	    default:
		__logging("agent::send_resolve: Unknow Error %x\n", iResult);
		break;
	  }
	  
	  
	  return(-1);
	}
	
}


int agent::send_web(char *ip,int port,char *service) {
    try{
        netutils *net = new netutils();
	char *buff=(char *)__allocate(MAX_AGENT_BUFF*2);
	//bzero(buff, MAX_AGENT_BUFF*2);
        sprintf(this->request, "%s;%s;%d;%s\n\r", config::get_agenttoken(), ip, port, service);

        if(net->tcp_open(config::get_agentip(),0xC0FE)==1) {
	      net->tcp_send(this->request,strlen(this->request));
	      bzero(buff,MAX_AGENT_BUFF);
	      
	      while(net->tcp_recv(buff,MAX_AGENT_BUFF))
		bzero(buff,MAX_AGENT_BUFF);
	}
	else {
	    __liberty(buff);
	    delete(net);
	    throw 1;
	}
        __liberty(buff);
        net->tcp_close();
	delete(net);
        return(0);
	
    }
    catch(int iResult) {
	switch(iResult) {
	  case 1:
	    __logging("agent::send_resolve: Connection error");
	    break;
	  default:
	    __logging("agent::send_resolve: Unknow Error %x", iResult);
	    break;
	}
	return(-1);
    }
}

