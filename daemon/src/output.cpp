/**
 * @file output.c
 * @brief Save and process output
 * @author JennySec
**/
 
 
#include <bsd/bsd.h>
#include <bsd/stdio.h>
#include <bsd/stdlib.h>
#include <bsd/string.h>
#include <pthread.h>
#include "config.h"
//#include "db.h" (added in output.h to this->database)
#include "config.h"
#include "output.h"

struct st_output_node node_list[255];

output::output() 
{
	int i=0;
	this->last_node=this->next_node=0;
	this->output_method=METHOD_MYSQL;
	this->database = new db();
	
	try {
		for(i=0;i<=255;i++) {
			if((node_list[i].finger = (char *)malloc(MAX_OUTPUT_FINGER))==NULL) 	throw -1;
			else	bzero((void *)node_list[i].finger,MAX_OUTPUT_FINGER);
			if((node_list[i].cert = (char *)malloc(32000))==NULL)	throw -1;
			else	bzero((void *)node_list[i].cert, 32000);
			if((node_list[i].service = (char *)malloc(50))==NULL)	throw -1;
			else	bzero((void *)node_list[i].service, 50);
			node_list[i].port 	= node_list[i].status = (int)0;
			node_list[i].tid 	= 0L;
			node_list[i].proto	= 0;

		}

	}

	catch(int iResult) {
		perror("output::output error");
	}

	
}

output::~output(void) {
	int i = 0;
	for(i=0;i<=this->last_node;i++) {
		node_list[i].port=node_list[i].status=node_list[i].proto=0;
		node_list[i].tid=0L;
		free(node_list[i].finger);
		free(node_list[i].cert);
		free(node_list[i].service);
	}


}


/**
 * @brief push value in to nodes stack and increase index
 * @param __IN	node	pointer to st_output_node struct used for save outputs in nodes
 *
 * @returns 	next node index (increased)
 *
 */
int output::push(struct st_output_node *node) {

	if(this->next_node==255) return -1;
	

	bzero(node_list[this->next_node].finger, MAX_OUTPUT_FINGER);
	bzero(node_list[this->next_node].cert, 32000);
	bzero(node_list[this->next_node].service, 50);
	
	strlcpy(node_list[this->next_node].finger,node->finger, MAX_OUTPUT_FINGER);
	strlcpy(node_list[this->next_node].cert, node->cert, 32000);
	strlcpy(node_list[this->next_node].service, node->service, 12);
	
	node_list[this->next_node].port 	= node->port;
	node_list[this->next_node].proto 	= node->proto;
	node_list[this->next_node].tid		= node->tid;
	node_list[this->next_node].status 	= 1;
	
	this->next_node++;
	this->last_node = this->next_node-1;
	return(this->next_node);
}


/**
 * @brief pop value from node stack and decrease index
 * @param __OUT	node	pointer to st_output_node struct used for save outputs in nodes
 *
 * @returns 	last node index (decrease)
 *
 */
int output::pop(struct st_output_node *node) {

	try {
		if (last_node == -1)
			return -1;

		strlcpy(node->finger, node_list[this->last_node].finger, MAX_OUTPUT_FINGER);
		strlcpy(node->cert, node_list[this->last_node].cert, 32000);
		strlcpy(node->service, node_list[this->last_node].service, 12);
		node->port 	= node_list[this->last_node].port;
		node->proto = node_list[this->last_node].proto;
		node->tid = node_list[this->last_node].tid;

		bzero(node_list[this->last_node].finger, MAX_OUTPUT_FINGER);
		bzero(node_list[this->last_node].cert, 32000);
		bzero(node_list[this->last_node].service, 12);
		node_list[this->last_node].status=0;
		node_list[this->last_node].port=0;
		node_list[this->last_node].tid=0L;
		node_list[this->last_node].proto=0;
	
		this->last_node--;
		this->next_node = this->last_node+1;
	}
	catch(int iResult) {
		perror("ouptut::pop error");
		return(iResult);
	}	

	return(this->last_node);
}



/**
 * @brief pop all to output method
 * @param no params (use stack with output::push added values)
 *
 * @returns 	0	error
 *				1	success
 *
 */
int output::build(void) {
	int i = this->last_node;
	
	if(this->output_method==METHOD_MYSQL) {


		while(node_list[i].status==1) {
			//DEBUG 
			this->database->addScan(node_list[i].tid, node_list[i].port, node_list[i].proto,
						node_list[i].service, node_list[i].finger,node_list[i].cert);
			
			bzero(node_list[this->last_node].finger, MAX_OUTPUT_FINGER);
			bzero(node_list[this->last_node].cert, 32000);
			bzero(node_list[this->last_node].service, 50);
			node_list[this->last_node].status=0;
			node_list[this->last_node].proto=0;
			node_list[this->last_node].port=0;
			node_list[this->last_node].tid=0L;
			
			this->last_node--;
			this->next_node = this->last_node+1;
			i--;
	
		}

		return 1;
	}	

	return 0;
}


/**
 *	@brief output new targets
 *	@param	_IN	ip	ip of entry
 *	@param	_IN	geo	country code (example ES)
 *
 *	@returns 	0	error
 *				1	success
 *
 */
unsigned long long output::target(const char *ip, const char *geo, int pingstat) {
    //if(config::get_outputmethod()==0)
    return(this->database->addTarget(ip, geo,pingstat));
}
