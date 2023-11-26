#include <bsd/bsd.h>
#include <bsd/stdio.h>
#include <bsd/string.h>
#include <polarssl/base64.h>
#include "output.h"
#include "config.h"
#include "misc.h"
#include "modules/protocol_http.h"
#include "modules/module_ssl.h"
#include "modules/module_https.h"

#define JLAB_SEP "####"
#define JLAB_SEP_SIZE 4

struct jlabrpc {
    int option;
    void *data;
    char *token;
};

/**
 * @brief This function send data to HTTP/S RPC, make data string using rpc_datahost,rpc_datafinger 
 * 
 * @param _IN 	host 	ip or host to scan
 * @param _IN	port 	to scan
 * @returns 	0	close
 *          	1	open
 * 
 * @note: Remember not use geo with NULL params use "ANY" string as a NULL
 */
int rpc_send(void *data) {
  int ret = 0;
  if(data==NULL)
    return(0);
  
  switch(config::get_httptype()) {
    case 0:
      http *lhttp = new http();
      lhttp->request_init(0,config::get_httppath(),1,config::get_httphost(),config::get_httpport());
      lhttp->request_add_data(data);
      lhttp->request_make();
      lhttp->request_run(); 
      delete(lhttp);
      break;
    case 1:
      https *lhttps = new https();
      lhttps->request_init(0,config::get_httppath(),1,config::get_httphost(),config::get_httpport()) ? ret=1 : ret=0,delete(lhttps),break;
      lhttps->request_add_data(data) ? ret=1 : ret=0,delete(lhttps),break;
      lhttps->request_make();
      lhttps->request_run() ? ret=1 : ret=0;
      delete(lhttps);
      break;
    default:
	__logging("rpc_send: Unknow http type\n");
	break;
  }
  return(ret);
}



/**
 * @brief This function make data to add discovered host in master server
 * @brief: use "ANY" string as a NULL
 * 
 * @param _IN 	ip		string ip discovered to adding
 * @param _IN	geo		string geo country code in string example "EN","US","ES","KY","RU"
 * @param _IN	pingstat	integer 1 is ping available or 0 is ping packet lost
 * @returns 	0	any
 * 
 * @note: Remember not use geo with NULL params use "ANY" string as a NULL
 */
int rpc_datahost(char *ip, char *geo, int pingstat,void *data) {
    //@note: Unchecked buffer possible overflow
   sprintf(data, "option=1&ip=%s&geo=%s&ping=%d&token=%s", ip,geo,pingstat,config::get_httptoken());
   return(0);
}


/**
 * @brief This function make data from output node to finger/cert not use NULL in finger or cert
 * @brief: use "ANY" string as a NULL
 * 
 * @param _IN 	node 	pointer to struct output node defined in output.h 
 * @param _OUT	data	data output string with POST data 
 * @returns 	0	success
 *          	-1	error
 * 
 * @note: Remember not use node->finger or node->cert with NULL params use "ANY" string as a NULL
 */
int rpc_datafinger(struct st_output_node *node,void *data) {
    char *b64str = (char *)__allocate(MAX_OUTPUT_FINGER) ? : return(-1);
    char *tmp_data = (char *)__allocate(512) ? : return(-1);
    char *default_var = "option=2&data=";
    int buffsize = 0;
    
    strlcat(data,default_var,MAX_OUTPUT_FINGER);
     
    if(!base64_encode(b64str, MAX_OUTPUT_FINGER,node->finger, strlen(node->finger))) {
	  __logging("rpc_nodemake: base64 encode error");
	  return(-1);
    }
    
    buffsize += strlen(b64str) + JLAB_SEP_SIZE;
    strlcat(data,b64str,MAX_OUTPUT_FINGER);
    strlcat(data,JLAB_SEP,MAX_OUTPUT_FINGER);
    bzero(b64str,MAX_OUTPUT_FINGER);

    if(!base64_encode(b64str, MAX_OUTPUT_FINGER,node->cert, strlen(node->cert))) {
	  __logging("rpc_nodemake: base64 encode error");
	  return(-1);
    }
       
    buffsize += strlen(b64str) + JLAB_SEP_SIZE;
    strlcpy(data,b64str,MAX_OUTPUT_FINGER);
    strlcat(data,JLAB_SEP,MAX_OUTPUT_FINGER);
    __liberty(b64str);
   
    //@note: non correctly buffer check possible overflow
    sprintf(tmp_data,"&port=%d&proto=%d&status=%d&service=%d&token=%s",node->port,node->proto,node->status,node->service,config::get_httptoken());
    strlcat(data,tmp_data, MAX_OUTPUT_FINGER);
    __liberty(tmp_data);
        
    return(0);
}