/**
 * @file input.c
 * @brief Load and process input
 * @author JennySec
**/
#include <bsd/bsd.h>
#include <bsd/stdio.h>
#include <bsd/stdlib.h>
#include "misc.h"
#include "net_utils.h"
#include "config.h"
#include "input.h"


input::input(void) {
	this->method = config::get_inputmethod();
;
	
	switch(method) {
		case 0:									// RANDOM IP
			break;						
		case 1:									// FROM IP LIST FILE
			if((this->f = fopen((char *)config::get_inputfile(),"r"))==NULL) {
				__logging("Not is possible open file %s\n", config::get_inputfile());
				abort();
			}
			
			fseek(this->f, 0x00, SEEK_SET);
			this->actual_line=0;
			break;			
		default:
			this->method=0;						// RANDOM IP
			break;

	}
	
}

input::~input(void) {
	try {
		if(this->f)
		  fclose(this->f);
	}
	catch(int iResult) {
		__logging("input::~input\n");
	}
}






/**
 * @brief Get next ip from input
 * @param _OUT	generated or readed IP 
 * @returns 	1	sucess
 *          	0	end of list ips or error
 */
int input::next(void *args) {
	char *str = (char *)args;
	size_t len;
	
	
	switch(this->method) {
		case 0:								// RANDOM IP GENERATION
			strlcpy((char *)str, random_ip(), 128);
			this->status=1;
			return 1;
			break;
		case 1:								//INPUT LIST IP FILE
			//volatile char *tmp;
			char *tmp;
			tmp=NULL;
			
			if(feof(this->f))
				return(0);
			
			if(getline((char **)&tmp, &len,this->f)==-1)
				return(0);
				
			if(len<20) {
				__logging("Error line %d is too long %s\n", this->actual_line, (const char *)tmp);
				return(0);
			}
			else {
				strlcpy((char *)str, (const char *)tmp,  strlen((const char *)tmp));
				if(netutils::isvalidIPV4((const char *)str)==true) {
					this->status=1;
					return(1);
				}
				else {
					__logging("Invalid ip %s\n", tmp);
				}
			}
			return(0);
			break;
		default:	
			break;	
	}

	this->status=0;
	return 0;
}


/**
 * @brief Get status of input
 * @param void 
 * @returns 	status
 *          	0	error
 */
int input::get_status(void) {
	return(this->status);
}
