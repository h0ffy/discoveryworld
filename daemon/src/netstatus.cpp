#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>
#include "config.h"
#include "misc.h"
#include "threads.h"
#include "net_utils.h"
#include "netstatus.h"


#define MAX_NETSTATUS_ALERT 6
bool g_th_freeze=false;


bool netstatus::ping(const char *ip) {
  try {
    netutils *net = new netutils();
    
    bool ret = net->ping(ip);
    
    delete(net);   
    return(ret);
  }
  catch(int iResult) {
      __logging("netstatus::ping: Unknow Error");
      return(false);
  }
}

bool netstatus::tcptest(const char *ip, unsigned short port) {
  try {
    netutils *net = new netutils();
    bool ret=false;
    
    if((ret=net->tcp_open(ip,port))==true)
      net->tcp_close();
    
    delete(net);
    return(ret);
  }
  catch(int iResult) {
      __logging("netstatus::tcptest: Unknow Error\n");
      return(false);
  }
}


void *netstatus::monitor(void *args) {  
  int l_alert = 0;
  try {
    while(1) {
	if(l_alert>=MAX_NETSTATUS_ALERT) {
	    g_th_freeze=true;
	    __logging("Monitor Network: Gobal network connection fail\n");
	    __logging("Wait minute...\t");
	    sleep(60);
	    __logging("[OK]\n");
	    l_alert=0;
	}

	if(netstatus::tcptest(config::get_tcptestip(),config::get_tcptestport())==false) {
	  __logging("Monitor Network: Ping to uptime server non response\n");
	  l_alert++;
	  g_th_freeze=true;
	  
	  sleep(2);
	  if(netstatus::ping(config::get_pingtestip())==false) {
	    __logging("Monitor Network: TCP connect to uptime server non response\n");
	    l_alert++;
	    g_th_freeze=true;	  
	  }
	  else {
	    l_alert=0;
	    g_th_freeze=false;
	    continue;
	  }
	  
	}
	else {
	  g_th_freeze=false;
	  l_alert=0;
	}
	
	sleep(5);
    } // while end
  }
  catch(int iResult) {
      __logging("netstatus::monitor: Unknow critical error\n");
      abort();
  }
  pthread_exit(0);
  return(NULL);
}
