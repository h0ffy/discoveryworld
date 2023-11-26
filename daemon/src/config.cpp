/**
 * @file config.c
 * @brief Load and process config file
 * @author JennySec / h... (A... de...)
 * @license OpenSource (GPL)
 *			-	https://www.jennysec.com/discoveryworld/license.html
 *			-	https://www.jennysec.com/discoveryworld/license.txt
 *
 * @url		-	https://www.jennysec.com
 *			-	https://www.jennysec.es
 *			-	https://www.jennylab.es
 *			-	https://www.github.com/discoveryworld/
 *
 * @mail	-	ag...@....
 *			-	h....@....
 *			-	info@.....
 *			-	lab@jla....
 *
 * @licensedProducts
 *			- This code is licensed over GPL (OpenSource) with previously posted URL link to license 
 *
 * @changelog
 *			- 19/06/2016 (Creation or publish)
 *			-
**/
#include <bsd/bsd.h>
#include <bsd/stdio.h>
#include <libconfig.h>
#include "config.h"

struct sconf theconf;

config::config(const char *filepath) {
	volatile const char *log_file, *input_filepath,*interface;
	volatile const char *dbhost, *dbname, *dbuser, *dbpass, *dbtable;
	volatile const char *agent_token,*agent_ip;
	volatile int port_list_count,service_list_count;
	const config_setting_t *port_list,*service_list;
	this->status=0;

	
	try {
		this->discoveryconf = &theconf;
		config_init(&this->conf);

		if(!config_read_file(&this->conf, filepath)) {
			perror("Not possible to open log file");
			throw 0x29a;
		}
		this->status=1;
		
		//log-file
		if(config_lookup_string(&this->conf, "log-status", (const char **)&log_file)) {
			if((this->discoveryconf->output_log_error = (char *)malloc(8000))==NULL) throw -1;
			if(memset((void *)this->discoveryconf->output_log_error, 0, 8000)==NULL) throw -1;
			if(strlcpy(this->discoveryconf->output_log_error,(const char *)log_file,8000)==0x00) throw -1;
		}
		else {
			this->discoveryconf->output_log_error=NULL;
			perror("Invalid output log ( log-status ) in discoveryworld.conf");
			abort();
		}


		if(!config_lookup_int(&this->conf, "input-method", &this->discoveryconf->input_method)) {
			this->discoveryconf->input_method=0x00;
		}

		//input-file
		if(config_lookup_string(&this->conf, "input-file", (const char **)&input_filepath)) {
			if((this->discoveryconf->input_filepath = (char *)malloc(2048))==NULL) throw -1;
			if(memset((void *)this->discoveryconf->input_filepath, 0, 2048)==NULL) throw -1;
			if(strlcpy(this->discoveryconf->input_filepath,(const char *)input_filepath,2048)==0x00) throw -1;
		}
		else {
			this->discoveryconf->input_filepath=NULL;
			if(this->discoveryconf->input_method==CONF_METHOD_IPFILE)
				throw -1;
		}

		

		// GENERIC
		if(!config_lookup_int(&this->conf, "resume",&this->discoveryconf->resume)) {
			this->discoveryconf->resume=0;
		} 

		if(!config_lookup_int(&this->conf, "max-threads",&this->discoveryconf->max_threads)) {
			this->discoveryconf->max_threads = 1;
		}
		else {
			if(this->discoveryconf->max_threads > 75)
			    this->discoveryconf->max_threads = 75;
			else if(this->discoveryconf->max_threads <= 0)
			    this->discoveryconf->max_threads=1;
		}

		if(!config_lookup_int(&this->conf, "num-process",&this->discoveryconf->number_of_process)) {
			this->discoveryconf->number_of_process = 1;
		}
		else {
			if(this->discoveryconf->number_of_process > 10)
				this->discoveryconf->number_of_process = 10;
			else if(this->discoveryconf->number_of_process <= 1)
				this->discoveryconf->number_of_process = 1;
		}

		
		
		if(!config_lookup_int(&this->conf, "tcpsyn-timeout",&this->discoveryconf->tcpsyn_timeout)) {
			this->discoveryconf->tcpsyn_timeout = 120;
		} 	

		if(!config_lookup_int(&this->conf, "socket-timeout",&this->discoveryconf->socket_timeout)) {
			this->discoveryconf->socket_timeout = 2000;
		}

		if(config_lookup_string(&this->conf, "interface", (const char **)&interface)) {
			if((this->discoveryconf->interface = (char *)malloc(10))==NULL) throw -1;
			bzero((void *)this->discoveryconf->interface, 0);
			if(strlcpy(this->discoveryconf->interface, (const char *)interface, 10)==0x00) throw -1;
		}
		else {
			if((this->discoveryconf->interface = (char *)malloc(10))==NULL) throw -1;
			bzero((void *)this->discoveryconf->interface, 0);
			if(strlcpy(this->discoveryconf->interface, "eth0", 10)==0x00) throw -1;
		}

		
		if(!config_lookup_int(&this->conf, "output-method", &this->discoveryconf->output_method)) {
			this->discoveryconf->output_method=0x00; //
		}
		

		if((port_list =	config_lookup(&this->conf,"ports"))==CONFIG_FALSE)	throw 1;
		if(	config_setting_is_list(port_list) == CONFIG_FALSE 	&&
			config_setting_is_array(port_list) == CONFIG_FALSE	&&
			config_setting_is_group(port_list) == CONFIG_FALSE) throw 1;
				
		if((port_list_count=config_setting_length(port_list))==0)	throw 1;
		if(port_list_count>65535) {
			perror("List of ports is too long");
			exit(0);
		}
		
		int i=0;
		for(i=0;i<port_list_count;i++) {
			this->discoveryconf->ports[i] = config_setting_get_int_elem(port_list,i);
			if(	this->discoveryconf->ports[i]>65535 ||
				this->discoveryconf->ports[i]<=0	) {
					printf("Invalid port: %d\n", this->discoveryconf->ports[i]);
					throw;
				}
					
		}
		this->discoveryconf->ports[port_list_count+1]=0;
		this->discoveryconf->ports_count=port_list_count;
	
	
	

		if((service_list =	config_lookup(&this->conf,"services"))==CONFIG_FALSE)	throw 1;
		if(	config_setting_is_list(service_list) == CONFIG_FALSE 	&&
			config_setting_is_array(service_list) == CONFIG_FALSE	&&
			config_setting_is_group(service_list) == CONFIG_FALSE) throw 1;
				
		if((service_list_count=config_setting_length(service_list))==0)	throw 1;
		if(service_list_count>65535) {
			perror("List of ports is too long");
			exit(0);
		}
		
		int b=0;
		for(b=0;b<service_list_count;b++) {
			this->discoveryconf->services[b] = config_setting_get_int_elem(service_list,b);
			if(this->discoveryconf->services[b]<=-1) {
				perror("Error service number is negative");
				exit(0);
			}
			else if(this->discoveryconf->services[b]>MAX_SERVICES) {
				printf("Invalid service %d\n", this->discoveryconf->services[b]);
				exit(0);
			}
		}
		this->discoveryconf->services[service_list_count+1]=0;
		this->discoveryconf->services_count = service_list_count;
		if(service_list_count!=port_list_count) throw 0x4a;
	
	
		if(config_lookup_string(&this->conf, "agent.token", (const char **)&agent_token)) {
			if((this->discoveryconf->agent_token = (char *)malloc(32*2))==NULL) throw -1;
			bzero((void *)this->discoveryconf->agent_token,32*2);
			if(strlcpy(this->discoveryconf->agent_token, (const char *)agent_token, 33)==0) throw -1;
		}
		
		if(config_lookup_string(&this->conf, "agent.ip", (const char **)&agent_ip)) {
			if((this->discoveryconf->agent_ip = (char *)malloc(20*2))==NULL) throw -1;
			bzero((void *)this->discoveryconf->agent_ip,20*2);
			if(strlcpy(this->discoveryconf->agent_ip, (const char *)agent_ip, 20)==0) throw -1;
		}
		
		/**
		  * @note add switch case to add scalatility on db class and output method (non force to add padding in config file (db output))
		**/
		// DB
	
		if(config_lookup_string(&this->conf, "db.host", (const char **)&dbhost)) {
			if((this->discoveryconf->output_dbhost = (char *)malloc(128))==NULL) throw -1;
			bzero((void *)this->discoveryconf->output_dbhost, 128);
			if(strlcpy(this->discoveryconf->output_dbhost, (const char *)dbhost,128)==0) throw -1;
		}
		else {
			this->discoveryconf->output_dbhost = NULL;
			throw -1;
		}
		
		if(config_lookup_string(&this->conf, "db.name", (const char **)&dbname)) {
			if((this->discoveryconf->output_dbname = (char *)malloc(128))==NULL) throw -1;
			bzero((void *)this->discoveryconf->output_dbname, 128);
			if(strlcpy(this->discoveryconf->output_dbname, (const char *)dbname,128)==0) throw -1;
		}
		else {
			this->discoveryconf->output_dbname = NULL;
			throw -1;
		}



		if(config_lookup_string(&this->conf, "db.user", (const char **)&dbuser)) {
			if((this->discoveryconf->output_dbuser = (char *)malloc(128))==NULL) throw -1;
			bzero((void *)this->discoveryconf->output_dbuser, 128);
			if(strlcpy(this->discoveryconf->output_dbuser, (const char *)dbuser, 128)==0) throw -1;
		}
		else {
			this->discoveryconf->output_dbuser = NULL;
			throw -1;
		}

		if(config_lookup_string(&this->conf, "db.pass", (const char **)&dbpass)) {
			if((this->discoveryconf->output_dbpass = (char *)malloc(128))==NULL) throw -1;
			bzero((void *)this->discoveryconf->output_dbpass, 128);
			if(strlcpy(this->discoveryconf->output_dbpass, (const char *)dbpass,128)==0) throw -1;
		}
		else {
			this->discoveryconf->output_dbhost = NULL;
			throw -1;
		}

		if(config_lookup_string(&this->conf, "db.table", (const char **)&dbtable)) {
			if((this->discoveryconf->output_dbtable = (char *)malloc(128))==NULL) throw -1;
			bzero((void *)this->discoveryconf->output_dbtable, 128);
			if(strlcpy(this->discoveryconf->output_dbtable, (const char *)dbtable,128)==0) throw -1;
		}
		else {
			this->discoveryconf->output_dbtable = NULL;
			throw -1;
		}

		if(!config_lookup_int(&this->conf, "db.port",&this->discoveryconf->output_dbport)) {
			this->discoveryconf->output_dbport=3306;
		}


	}

	catch(int iResult) {
		printf("Error: %s:%d\t%s\n", config_error_file(&this->conf),config_error_line(&this->conf),config_error_text(&this->conf));

		if(iResult!=0x29a) {
			config_destroy(&this->conf);
		}
		
		switch(iResult) {
			case 1:
				perror("Error on config ports");
				break;
			case 0x4a:
				perror("Service and Ports non match");
				
			default:
				break;
		}
	
		perror("config::config fatal error");
		fflush(stdout);
		exit(-1);
	}

}


config::~config(void) {
	config_destroy(&this->conf);


	free((void *)this->discoveryconf->output_log_error);
	free((void *)this->discoveryconf->filepath);
	free((void *)this->discoveryconf->interface);
	free((void *)this->discoveryconf->ip_from);
	free((void *)this->discoveryconf->output_dbhost);
	free((void *)this->discoveryconf->output_dbname);
	free((void *)this->discoveryconf->output_dbuser);
	free((void *)this->discoveryconf->output_dbpass);
	free((void *)this->discoveryconf->output_dbtable);
	free((void *)this->discoveryconf->output_log_error);
	free((void *)this->discoveryconf->output_log_raw);
	free((void *)this->discoveryconf->input_filepath);
}


/**
 * @note 		STATIC CALLER TO OUTPUT TO ANY PART OF APPLICATION
 */

const char *config::get_output_log_error(void)			{	return(theconf.output_log_error);	}
const int config::get_port(int index)				{	return(theconf.ports[index]);		}
const int config::get_service(int index)			{	return(theconf.services[index]);	}
const int config::get_max_threads(void) 			{	return(theconf.max_threads);		}
const int config::get_num_process(void) 			{	return(theconf.number_of_process);	}
const int config::get_resume(void) 				{ 	return(theconf.resume);			}
const int config::get_tcpsyn_timeout(void) 			{	return(theconf.tcpsyn_timeout);		}
const int config::get_socket_timeout(void) 			{	return(theconf.socket_timeout);		}
const char *config::get_intput_filepath(void) 			{ 	return(theconf.input_filepath);		}
const char *config::get_interface(void) 			{	return(theconf.interface);		}
const char *config::get_dbname(void) 				{	return(theconf.output_dbname);		}
const char *config::get_dbhost(void) 				{	return(theconf.output_dbhost);		}
const char *config::get_dbuser(void) 				{	return(theconf.output_dbuser);		}
const char *config::get_dbpass(void) 				{	return(theconf.output_dbpass);		}
const int config::get_dbport(void) 				{	return(theconf.output_dbport);		}
const char *config::get_dbtable(void) 				{	return(theconf.output_dbtable);		}
const int config::get_inputmethod(void)				{	return(theconf.input_method);		}
const int config::get_outputmethod(void)			{	return(theconf.output_method);		}
const char *config::get_inputfile(void)				{	return(theconf.input_filepath);		}
const char *config::get_agenttoken(void)			{	return(theconf.agent_token);		}
const char *config::get_agentip(void)				{	return(theconf.agent_ip);		}
const char *config::get_httptoken(void)				{	return("Token IT");			}
const int *config::get_httptype(void)				{	return(0);				}
const char *config::get_httppath(void)				{	return("/rpc.php");			}
const char *config::get_httphost(void)				{	return("192.168.15.21");		}
const unsigned short config::get_httpport(void)			{	return(80);				}
const char *config::get_pingtestip(void)			{	return("8.8.8.8");			}
const char *config::get_tcptestip(void)				{	return("69.172.201.153");		}
const unsigned short config::get_tcptestport(void)		{	return(80);				}




const int config::get_servicebyport(int port)	{
	int i=0;
	
	for(i=0;i<theconf.ports_count;i++) {
		if(theconf.ports[i] == 0)
			continue;
			
		if(port==theconf.ports[i])
			return(theconf.services[i]);
	}
	
	return(-1);
}

