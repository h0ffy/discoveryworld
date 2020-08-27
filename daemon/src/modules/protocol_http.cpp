/**
 * @file modules/procotol_http.c
 * @brief HTTP Protocol implementation using linux sockets
 * @author JennySec
 * 
**/


/**
 * @note add to this code the new commited HTTP implementation in (jFramework C/C++) leak! :D
**/
#include <bsd/bsd.h>
#include <bsd/string.h>
#include "../config.h"
#include "../misc.h"
#include "../net_utils.h"
#include "protocol_http.h"


static const char *methods[] = {"GET", "POST", "HEAD", "OPTIONS","TRACE", "DEBUG", "PUT", "DELETE", NULL};
static const char *versions[] = {"HTTP/1.0","HTTP/1.1",NULL};


http::http() {
	try {
		this->data = NULL;
		this->path = NULL;
		this->status = 0;
		this->nodes_count = 0;
		if((this->request = (char *)malloc(MAX_HTTP_BUFF))==NULL)	throw;
			bzero(this->request, MAX_HTTP_BUFF);
		if((this->reply = (char *)malloc(MAX_HTTP_BUFF))==NULL)	throw;
			bzero(this->reply, MAX_HTTP_BUFF);
		if((this->host = (char *)malloc(MAX_HTTP_HOST))==NULL)	throw;
			bzero(this->host, MAX_HTTP_HOST);
		
	}

	catch(int iResult) {
		perror("http::http: Memory allocation error");
		exit(0);
	} 
		
}



http::~http() {
	int i = 0;
  try{
	if(this->data!=NULL)
	  free(this->data);
	if(this->path!=NULL)
	  free(this->path);
	
	for(i=0;i<=this->nodes_count-1;i++) {
	  if(this->nodes[i].status==1) {  
	      this->nodes[i].status=0;
	      if(this->nodes[i].value)
		free(this->nodes[i].value);
	      if(this->nodes[i].var)
		free(this->nodes[i].var);
	  }
	}
	
	this->nodes_count=0;
	this->status=0;
	free(this->request);
	free(this->reply);
  }
  catch(int iResult) {
      __logging("http::~http: Unknow error");
  }
}




/**
 * @brief Fingerprint HTTP header or 8000bytes in case of not recognize header
 * @param	_IN	host	host or ip to connect it
 * @param	_IN	port	destination port
 * @returns 	int	0	ERROR
 * 			1	SUCCESS
 *          	
 */
int http::finger(const char *host,int port) {
	const char *agent = HTTP_USER_AGENT;
	netutils *net = new netutils();
	int ret = 0;
	char *buff = (char *)malloc(MAX_HTTP_BUFF);
	
	bzero(buff, MAX_HTTP_BUFF);
	
	this->scan_make(host,0,(const char *)"/",agent);
	if(net->tcp_open(host,port)==1) {
		if(net->tcp_send(this->request,strlen(this->request))>=1) {
			if((ret = net->tcp_recv(buff,MAX_HTTP_BUFF))<10) {
				net->tcp_close();
				return(0);
			}
		}
		else {
			net->tcp_close();
			return(0);
		}
	}
	
	
	//EXTRACT HTTP HEADER FROM FULL PACKET
	int i=0;
	int b=0;
	for(i=0;i<ret;i++) {
		if(	buff[i] == '\n' && 
			buff[i+1] == '\n') {
						buff[i]='\0';	}
								
		else if(	buff[i] == '\r' 	&& buff[i+1] == '\n' &&
					buff[i+2] == '\r' 	&& buff[i+3] == '\n'  ) 	
						      {
							      buff[i]='\0';
							      b++;
						      }
																
		else if(	buff[i] == '\n' 	&& buff[i+1] == '\r' &&
					buff[i+2] == '\n' 	&& buff[i+3] == '\r'  ) 	
						      {
							      buff[i]='\0';
							      b++;
						      }
	}
	
	
	strlcpy(this->reply,buff,MAX_HTTP_BUFF);
	
	
	free(buff);
	net->tcp_close();
	return(1);
}




/**
 * @brief Fast method to make packet for http fingerprint scan
 * @param	_IN	host		host or ip to connect it
 * @param	_IN	path		full path (with get variables also GET)
 * @param	_IN	user_agent	http user agent
 * @returns 	int	0	ERROR
 * 			1	SUCCESS
 *          	
 */
bool http::scan_make(const char *host, int method, const char *path, const char *user_agent) {
	
	static const char *format = "%s %s HTTP/1.1\r\nHost: %s\r\nUser-Agent: %s\r\n"
				    "Accept: text/html,application/xhtml+xml,application/xml;q=0.9,*/*;q=0.8\r\n"
				    "Accept-Language: en-US,en;q=0.5\r\n"
				    "Connection: close\r\n"
				    "\n\n";


	try {
		if(method>MAX_METHODS) {
			__logging("http::request_make: Invalid method (%d) use default GET",method);
			method=0;
		}
		
		if(this->request==NULL) 	throw 0;
		
		if((	strlen(format)			+
				strlen(methods[method])	+
				strlen(path)			+
				strlen(user_agent)) 	< MAX_HTTP_BUFF ) {
			
			sprintf(this->request, format, methods[method], path, host, user_agent);
			
			return(true);
			
		}

		else {
			sprintf(this->request, "Buff is too long");
			__logging("http::request_make: Buff is too long %d\n", strlen(this->request));
		}
		
		
			
		return(false);
	}	

	catch(int iResult) {
		if(iResult==0)
		    __logging("http::request_make: Invalid memory poointer");
		else
		    __logging("http::request_make: Unknow Error");
	}	

	return(false);
}





/**
 * @brief Add headers to request
 * @param	_IN	header		name of header variable
 * @param	_IN	value		value of variable
 * @returns 	bool	true
 *			false
 *          	
 */

bool http::request_add_header(const char *header, const char *value) {
  try {
//    
    if(this->nodes_count==MAX_HEADER_NODES)
      return(false);
    
    if(this->nodes[this->nodes_count].status==1)
      return(false);
    else
      this->nodes[this->nodes_count].status=1;
    
   // printf("Nodes: %d\n", this->nodes_count);
    
    
    if(strlen(header)>120) {
      //__logging("http::request_add: Buff is too long (Header: %d\tValue: %d\n", strlen(header), strlen(value));
      this->nodes[this->nodes_count].var = (char *)malloc(121);
      bzero(this->nodes[this->nodes_count].var,121);
      strlcpy(this->nodes[this->nodes_count].var, header, 120);
      //this->nodes[this->nodes_count].var[strlen(this->nodes[this->nodes_count].var)]=':';
    }
    else {
       this->nodes[this->nodes_count].var = (char *)malloc(strlen(header)+10);
       bzero(this->nodes[this->nodes_count].var, strlen(header)+10);
       strlcpy(this->nodes[this->nodes_count].var, header, strlen(header)+10);
       //printf("%s\n",this->nodes[this->nodes_count].var);
       //this->nodes[this->nodes_count].var[strlen(this->nodes[this->nodes_count].var)]=':';
    }
    
    if(strlen(value)>8000) {
	//__logging("http::request_add: Buff is too long (Header: %d\tValue: %d\n", strlen(header), strlen(value));
	this->nodes[this->nodes_count].value =  (char *)malloc(8000);
	bzero(this->nodes[this->nodes_count].value, 8000);
	strlcpy(this->nodes[this->nodes_count].value,value,8000);
    }
    else {
	this->nodes[this->nodes_count].value = (char *)malloc(strlen(value)+10);
	bzero(this->nodes[this->nodes_count].value, strlen(value)+10);
	strlcpy(this->nodes[this->nodes_count].value,value,strlen(value)+10);
    }
   
    this->nodes_count++;
    return(true);
  }
  catch(int iResult) {
      __logging("http::request_add_header: Unknow Error"); 
      return(false);
    
  }
}


/**
 * @brief Initialize request
 * @param	_IN	method		method of use 0-7 (GET - DELETE)
 * @param	_IN	path		full path with variables in case of GET
 * @param	_IN	version		version of use (0-1) (HTTP/1.0, HTTP/1.1)
 * @param	_IN	host		remote host
 * @param 	_IN	port		remote port
 * @returns 	bool	true	
 *			false
 *          	
 */

bool http::request_init(int method,const char *path, int version, const char *host,int port) {
    try {
	if(this->status==1)
	  return(false);
	else
	  this->status=1;
	
	
	this->version = version ? 1 : 0;
	
	if(method>MAX_METHODS)
	  this->method = 0;
	else
	  this->method = method;
	
	
	if(port<=0 || port>65535)
	  this->port=80;
	else
	  this->port=port;
	
      
	if(strlen(path)>8000) {
	  this->path = (char *)malloc(8000);
	  bzero(this->path, 8000);
	  strlcpy(this->path, path, 8000);
	}
	else {
	  this->path = (char *)malloc(strlen(path)+10);
	  bzero(this->path, strlen(path)+10);
	  strlcpy(this->path, path, strlen(path)+10);
	}
  
  
	if(strlen(host)>512)
	  throw 0;
	else if(host==NULL)
	  return(false);
	else if(strlen(host)<5)
	  return(false);
	
	if((this->host = (char *)malloc(strlen(host)+10))==NULL)
	  throw -1;
	
	bzero(this->host,strlen(host)+10);
	strlcpy(this->host,host,strlen(host)+10);
	
	
	
	this->nodes_count=0;
	this->data=NULL;
	return(true);
  
    }
    catch(int iResult) {
	switch(iResult) {
	  case -1:
	    __logging("http::request_init: Critical memory error", host);   
	    abort();
	  case 0:
	    __logging("http::request_init: Error host is too long %s", host);   
	    break;
	  default:
	    __logging("http::request_init: Unknow Error"); 
	    break;
	}
	
	return(false);
    }
  
}


/**
 * @brief Add data to request
 * @param	_IN	data	raw data example "a=aaaaa&b=bbbbbb"
 * @returns 	bool	true
 *			false
 *          	
 */

bool http::request_add_data(char *data) {
    try {
	int len;
	
	if(data==NULL || this->data!=NULL)
	  return(false);
	
	if(strlen(data)>MAX_DATA)
	  len=MAX_DATA;
	else
	  len=strlen(data);
	
	if((this->data = (char *)malloc(len+10))==NULL)	  throw 0;
	
	bzero(this->data, len+10);
	strlcpy(this->data, data, len);
	return(true);
	
  
    }
    catch(int iResult) {
	switch(iResult) {
	  case 0:
	      __logging("http::request_add_data: Critical error memory allocation");
	      abort();
	  default:
	    __logging("http::request_add_data: Unknow Error"); 
	}
	return(false);
    }
  
}



/**
 * @brief Build request
 * @returns 	void
 *       
 */
void http::request_make() {
    int i = 0;
    char *tmp = (char *)__allocate(512);
    
    
    //force use 
    
    //insert defaults nodes
    if(this->nodes_count==0) {
	  this->request_add_header("Host", this->host);
	  this->request_add_header("User-Agent", HTTP_USER_AGENT);
	  this->request_add_header("Accept", "text/html,application/xhtml+xml,application/xml;q=0.9,*/*;q=0.8");
	  this->request_add_header("Accept-Language", "en-US,en;q=0.5");
	  this->request_add_header("Connection", "keep-alive");
	  
	  if(this->data!=NULL) {
	      char *strlength = (char *)__allocate(10);
	      sprintf(strlength, "%d", strlen(data));
	      this->request_add_header("Content-Length", strlength);
	      __liberty(strlength);
	  }
      
    }
    bzero(tmp, 512);
    sprintf(tmp,"%s %s %s%s", methods[this->method],this->path, versions[this->version], HTTP_END_LINE);
    strlcpy(this->request, tmp, MAX_HTTP_BUFF);
    
    this->nodes_count--;
    
    for(i=0;i<=this->nodes_count-1;i++) {
	bzero(tmp, 512);
	if(this->nodes[i].var==NULL || this->nodes[i].value==NULL)
	  continue;
	else {
	
	  sprintf(tmp, "%s: %s%s", this->nodes[i].var, this->nodes[i].value, HTTP_END_LINE);
	 
	  //printf("%s\n", tmp);
	  strlcat(this->request, tmp, MAX_HTTP_BUFF);
	}
	
    }

    if(this->data!=NULL)
      strlcat(this->request, this->data,MAX_HTTP_BUFF);
   
    strlcat(this->request,HTTP_FOOT_LINE,MAX_HTTP_BUFF);
    
 
    __liberty(tmp);
    //printf("%s", this->request);fflush(stdout);exit(0);
	// if(strncmp(this->nodes[0][0], defaults_nodes[0][0])
  
}


/**
 * @brief Run request
 * @returns 	bool	true
 *			false
 */
bool http::request_run() {
    netutils *net = new netutils();
    
    if(net->tcp_open(this->host,this->port)==1) {
	if(net->tcp_send(this->request,strlen(this->request))>0) {
	    if((this->reply_count=net->tcp_recv(this->reply,MAX_HTTP_BUFF))>10) {
	      net->tcp_close();
	      return(true);
	    }
	}
	
	net->tcp_close();
    }
    
  
    return(false);
}



bool http::request_headextract() {
  int b=0,i=0;
  
  for(i=0;i<this->reply_count;i++) {
	  if(this->reply[i] == '\n' && this->reply[i+1] == '\n') {  this->reply[i]='\0'; }
							  
	  else if(this->reply[i] == '\r' && this->reply[i+1] == '\n' &&
		  this->reply[i+2] == '\r' && this->reply[i+3] == '\n'  ) {
			  this->reply[i]='\0';
			  b++;
		  }
															  
	  else if(this->reply[i] == '\n' && this->reply[i+1] == '\r' &&
		  this->reply[i+2] == '\n' && this->reply[i+3] == '\r'  ) {
		      this->reply[i]='\0';
		      b++;
	      }
  }


  return(!b ? true : false);
 
}
