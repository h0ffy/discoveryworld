/**
 * @file db.c
 * @brief Process and execute databases output (actualy only MYSQL/MariaDB.
 * @author JennySec
**/
#include <bsd/bsd.h>
#include <bsd/stdio.h>
#include <bsd/string.h>
#include <bsd/stdlib.h>
#include <syslog.h>
#include "config.h"
#include "output.h"
#include "misc.h"
#include "db.h"

// non scalable class (only mysql) add scalability in next update version
db::db()
{
	//MYSQL_RES *result;

	this->status=0; 
	

	try {
		if(this->status==1)										throw 0;
		if((this->conn = mysql_init(NULL))==NULL)							throw -1;		
		if(mysql_real_connect(this->conn, config::get_dbhost(), config::get_dbuser(), config::get_dbpass(), config::get_dbname(), config::get_dbport(), 0, 0)==NULL) throw -1;
		if((this->query=(char *)malloc(32000))==NULL) 		throw -1;
		bzero((void *)this->query, 32000);
		
		


	}
	catch(int iResult) {
		this->status=0;
		switch(iResult) {
			case 0x00:
				__logging("db:db connect to db not is possible");
				abort();
			case 0x01:
				__logging("db:db main table no exists");
				abort();
			case 0x02:
				__logging("db::test_db not exists all tables");
				abort();
			default:
				__logging("db:db error");
				abort();
		}
	
	}

}


db::~db(void) {
	this->status=0;
	free((void *)this->query);
	mysql_close(this->conn);
}
	


void db::db_query_make(const char *ip, struct st_output_node *node) {
	try {
		//volatile int i;
		const char query_insert[] = "INSERT INTO %s (IP,PORT,PING) VALUES ('%s', '%s', %d);";
		const char query_update[] = "UPDATE %s SET ports='%s,%s' WHERE ip='%s';";
		char *tmp_query, *tmp_ports;
		//char *ports;

		if((tmp_query=(char *)malloc(9000))==NULL) 	throw -1;
		if((tmp_ports=(char *)malloc(10))==NULL) 	throw -1;


		
		/*
		if((ports=(char *)malloc(1000))==NULL) 		throw -1;
		bzero(ports, 1000);

		for(i=0;i<=count;i++) {
			if(i==0) {
				sprintf(tmp_query, query_insert, this->dbtable, ip, ports, nodes[i]->status);
				if(strlcat(this->query, tmp_query, 32000)) throw -1;		
			}
			else {
				sprintf(tmp_query, query_update, this->dbtable, ports, ip);
				if(strlcpy(this->query, tmp_query, 32000)) throw -1;
			}
			bzero(tmp_query, 32000);
		}*/

	}		
	
	catch(int iResult) {
		__logging("db::db_query_make");
		exit(-1);
	}
	
}


/**
 * @brief Test if exists all tables in database
 * @param 	any
 * @returns 	false
 *          	true
 */
bool db::test_db(void) {
	int i =	0;
	MYSQL_RES *result;

	try {
		const char *query_table_exists[] = {"SHOW TABLES LIKE 'scans'",
											"SHOW TABLES LIKE 'targets'",
											"SHOW TABLES LIKE 'webcms'",
											"SHOW TABLES LIKE 'domains'"};
			

		for(i=0;i<=3;i++) {
			if(mysql_query(this->conn, query_table_exists[i]))  throw 0;
			if((result=mysql_store_result(this->conn))==NULL)   throw 0;
			if(mysql_num_fields(result)==~0)                    throw 0;
			mysql_free_result(result);			
		}

		return(true);
	
	
	}
	catch(int iResult) {
		return(false);
	}
	
	return(false);
	
}



/**
 * @brief Test if host exists into targets list table
 * @param	_IN	ip		the ip to test if exists	
 * @returns 	false
 *          	true
 */
bool db::hosts_exists(const char *ip) {
	const char query1[] = "SELECT IP FROM targets WHERE IP=?";
	MYSQL_STMT *stmt;
	void *data, *out;
	MYSQL_BIND bind[1], in[1];
	bool ret = false;
	unsigned long ret_len=0L;
	bool data_enable=false;
	
	try {
			if((stmt = mysql_stmt_init(this->conn))==NULL) throw 0;
			while(mysql_stmt_prepare(stmt, query1, (unsigned long)strlen(query1))!=0)
				sleep(0.1);
			
			
			bzero(bind, sizeof(bind));
			
			bind[0].buffer_type 	=	MYSQL_TYPE_STRING;
			bind[0].buffer 			=	(void *)ip;
			bind[0].buffer_length	=	strlen(ip);
			
			if(mysql_stmt_bind_param(stmt, bind)!=0) throw 2;
			if(mysql_stmt_execute(stmt)!=0) throw 3;
			mysql_stmt_store_result(stmt);

			
			out=malloc(2000);
			bzero(out, 2000);
			bzero(in, sizeof(in));
			in[0].buffer_type		= 	MYSQL_TYPE_STRING;
			in[0].buffer 			=	(char *)out;
			in[0].buffer_length		=	2000;
			in[0].length			=	&ret_len;
			

			mysql_stmt_bind_result(stmt, in);
			mysql_stmt_store_result(stmt);
			mysql_stmt_fetch(stmt);
			
			if(ret_len>0) {
				data = (char *)malloc(ret_len);
				bzero(data, ret_len);
				in[0].buffer		= 	(void *)data;
				in[0].buffer_length	=	ret_len;
				mysql_stmt_fetch_column(stmt, in, 0, 0);
				data_enable=true;
			}
			
	
			if(mysql_stmt_num_rows(stmt)!=0) {
				if(strncmp(ip,(char *)data,strlen(ip))==0)
					ret=true;
			}
			
			if(data_enable)
				free(data);
			if(!out)
				free(out);
				
			mysql_stmt_free_result(stmt);
			
			if(mysql_stmt_close(stmt)!=0) throw 4;
			

	}
	catch(int iResult) {
		switch(iResult) {
				case 0:
					__logging("db::hosts_exists: MySQL stmt memory alloc error");
					return(false);
				case 1:
					__logging("db::hosts_exists: MYSQL prepare statement error");
					return(false);
				case 2:
					__logging("db::hosts_exists: MYSQL bind param memory error");
					return(false);
				case 3:
					__logging("db::hosts_exists: MYSQL stmt execute error");
					return(false);
				case 4:
					__logging("db::hosts_exists: MySQL stmt memory free error");
					return(false);
				default:
					break;
		}
		
		return(false);
		
	}
		
	return(ret);
}



/**
 * @brief Retreive TID by IP (ip2tid)
 * @param	_IN ip		the ip to retreive tid
 * @returns 	false/0		is error
 *          	tid			target id value in database from ip 
 */
unsigned long long db::getTIDbyIP(const char *ip) {
	const char query1[] = "SELECT TID FROM targets WHERE IP=?";
	MYSQL_STMT *stmt;
	void *data;
	MYSQL_BIND bind[1], in[1];
	unsigned long long ret;
	unsigned long ret_len;
	bool data_enable=false;
	
	try {
			if((stmt = mysql_stmt_init(this->conn))==NULL) throw 0;
			if(mysql_stmt_prepare(stmt, query1, (unsigned long)strlen(query1))!=0) throw 1;
			
			
			bzero(bind, sizeof(bind));
			
			bind[0].buffer_type 	=	MYSQL_TYPE_STRING;
			bind[0].buffer 			=	(void *)ip;
			bind[0].buffer_length	=	strlen(ip);
			
			
			if(mysql_stmt_bind_param(stmt, bind)!=0) throw 2;
			if(mysql_stmt_execute(stmt)!=0) throw 3;
			mysql_stmt_store_result(stmt);

			
			bzero(in, sizeof(in));
			in[0].buffer_type		= 	MYSQL_TYPE_LONG;
			in[0].buffer 			=	&ret;
			in[0].length			=	&ret_len;
	

			mysql_stmt_bind_result(stmt, in);
			mysql_stmt_store_result(stmt);
			mysql_stmt_fetch(stmt);
			
			if(ret_len>0) {
				data                =   &ret;
				in[0].buffer		= 	(void *)data;
				in[0].buffer_length	=	ret_len;
                data_enable         =   true;
				mysql_stmt_fetch_column(stmt, in, 0, 0);
			}
			
	
			if(mysql_stmt_num_rows(stmt)==0) {
				ret=0;
			}
		
			mysql_stmt_free_result(stmt);
			
			if(mysql_stmt_close(stmt)!=0) throw 4;
			return((unsigned long long)ret);
			

	}
	catch(int iResult) {
		switch(iResult) {
				case 0:
					__logging("db::getTIDbyIP: MySQL stmt memory alloc error");
					return(false);
				case 1:
					__logging("db::getTIDbyIP: MYSQL prepare statement error");
					return(false);
				case 2:
					__logging("db::getTIDbyIP: MYSQL bind param memory error");
					return(false);
				case 3:
					__logging("db::getTIDbyIP: MYSQL stmt execute error");
					return(false);
				case 4:
					__logging("db::getTIDbyIP: MySQL stmt memory free error");
					return(false);
				default:
					break;
		}
		
		return(false);
		
	}
		
	return(ret);
}





/**
 * @brief Test if exists scan
 * @param	_IN tid		the TID
 * @param	_IN port	the port
 * @param	_IN proto	the protocol
 * @returns 	false
 *				true	          	
 */
bool db::scan_exists(unsigned long long tid,int port,int proto) {
	const char query1[] = "SELECT TID FROM scans WHERE TID=? AND PORT=? AND PROTO=?";
	MYSQL_STMT *stmt;
	void *data, *out;
	MYSQL_BIND bind[3], in[1];
	bool ret = false;
	unsigned long ret_len;
	bool data_enable=false;
	
	try {
			if((stmt = mysql_stmt_init(this->conn))==NULL) throw 0;
			if(mysql_stmt_prepare(stmt, query1, (unsigned long)strlen(query1))!=0) throw 1;
			
			
			bzero(bind, sizeof(bind));
			
			bind[0].buffer_type 	=	MYSQL_TYPE_LONG;
			bind[0].buffer 			=	&tid;
			bind[1].buffer_type		=	MYSQL_TYPE_SHORT;
			bind[1].buffer			=	&port;
			bind[2].buffer_type		=	MYSQL_TYPE_SHORT;
			bind[2].buffer			=	&proto;
			
			
			if(mysql_stmt_bind_param(stmt, bind)!=0) throw 2;
			if(mysql_stmt_execute(stmt)!=0) throw 3;
			mysql_stmt_store_result(stmt);

			
			out=malloc(2000);
			bzero(out, 2000);
			bzero(in, sizeof(in));
			in[0].buffer_type		= 	MYSQL_TYPE_STRING;
			in[0].buffer 			=	(char *)out;
			in[0].buffer_length		=	2000;
			in[0].length			=	&ret_len;
	

			mysql_stmt_bind_result(stmt, in);
			mysql_stmt_store_result(stmt);
			mysql_stmt_fetch(stmt);
			
			if(ret_len>0) {
				data                =   (char *)malloc(ret_len);
				bzero(data, ret_len);
				in[0].buffer		= 	(void *)data;
				in[0].buffer_length	=	ret_len;
				mysql_stmt_fetch_column(stmt, in, 0, 0);
				data_enable=true;
			}
			
	
			
			if(mysql_stmt_num_rows(stmt)!=0) {
				ret=true;
			}
			
			if(data_enable)
				free(data);
			if(!out)
				free(out);

			mysql_stmt_free_result(stmt);
			
			if(mysql_stmt_close(stmt)!=0) throw 4;
			

	}
	catch(int iResult) {
		switch(iResult) {
				case 0:
					__logging("db::scan_exists: MySQL stmt memory alloc error");
					return(false);
				case 1:
					__logging("db::scan_exists: MYSQL prepare statement error");
					return(false);
				case 2:
					__logging("db::scan_exists: MYSQL bind param memory error");
					return(false);
				case 3:
					__logging("db::scan_exists: MYSQL stmt execute error");
					return(false);
				case 4:
					__logging("db::scan_exists: MySQL stmt memory free error");
					return(false);
				default:
					break;
		}
		
		return(false);
		
	}
		
	return(ret);
}



/**
 * @brief Retreive SCANID by tid,port,proto
 * @param	_IN tid		the TID
 * @param	_IN port	the port
 * @param	_IN proto	the protocol
 * @returns 	false
 *				true	          	
 */
unsigned long long db::getSCANID(unsigned long long tid,int port,int proto) {
	const char query1[] = "SELECT SCANID FROM scans WHERE TID=? AND PORT=? AND PROTO=?";
	MYSQL_STMT *stmt;
	void *data, *out;
	MYSQL_BIND bind[3], in[1];
	unsigned long long ret;
	unsigned long ret_len;
	bool data_enable=false;
	
	try {
			if((stmt = mysql_stmt_init(this->conn))==NULL) throw 0;
			if(mysql_stmt_prepare(stmt, query1, (unsigned long)strlen(query1))!=0) throw 1;
			
			
			bzero(bind, sizeof(bind));
			
			bind[0].buffer_type 		=	MYSQL_TYPE_LONG;
			bind[0].buffer 			=	&tid;
			bind[1].buffer_type		=	MYSQL_TYPE_LONG;
			bind[1].buffer			=	&port;
			bind[2].buffer_type		=	MYSQL_TYPE_LONG;
			bind[2].buffer			=	&proto;
			
			
			if(mysql_stmt_bind_param(stmt, bind)!=0) throw 2;
			if(mysql_stmt_execute(stmt)!=0) throw 3;
			mysql_stmt_store_result(stmt);

			
			in[0].buffer_type		= 	MYSQL_TYPE_LONG;
			in[0].buffer 			=	(char *)&ret;
			in[0].length			=	&ret_len;
	

			mysql_stmt_bind_result(stmt, in);
			mysql_stmt_store_result(stmt);
			mysql_stmt_fetch(stmt);
			
			if(ret_len>0) {
				data 			= 	(char *)&ret;
				in[0].buffer		= 	(void *)data;
				in[0].buffer_length	=	ret_len;
				mysql_stmt_fetch_column(stmt, in, 0, 0);
				data_enable=true;
			}
			
	
			
			if(mysql_stmt_num_rows(stmt)==0) {
				ret=0;
			}
		
			mysql_stmt_free_result(stmt);
			if(mysql_stmt_close(stmt)!=0) throw 4;
			return(ret);

	}
	catch(int iResult) {
		switch(iResult) {
				case 0:
					__logging("db::getSCANID: MySQL stmt memory alloc error");
					return(-1);
				case 1:
					__logging("db::getSCANID: MYSQL prepare statement error");
					return(-1);
				case 2:
					__logging("db::getSCANID: MYSQL bind param memory error");
					return(-1);
				case 3:
					__logging("db::getSCANID: MYSQL stmt execute error");
					return(-1);
				case 4:
					__logging("db::getSCANID: MySQL stmt memory free error");
					return(-1);
				default:
					break;
		}
		
		return(false);
		
	}
		
	return(ret);
}





/**
 * @brief Add new target if not exists
 * @param	_IN ip			the IP
 * @param	_IN geo			country extension
 * @param	_IN pingstat	if ping enable or not
 * @returns 	false	is exists or error
 *				true	is added
 */
unsigned long long db::addTarget(const char *ip,const char *geo,int pingstat) {
	const char query1[] = "INSERT INTO targets (IP,GEO,PING) VALUES (?,?,?)";
	MYSQL_STMT *stmt;
	MYSQL_BIND bind[3];
	//bool data_enable=false;
	unsigned long ret=0;

	
	if(ip==NULL || ip==(const char *)0xFFFFFFFF)
		throw 5;
	if(geo==NULL || geo==(const char *)0xFFFFFFFF)
		throw 6;
	if(pingstat < 0 || pingstat >1)
		pingstat=0;
	
	
	if(this->hosts_exists(ip)) {
		return(this->getTIDbyIP(ip));
	}
	
	
	
	try {
			if((stmt = mysql_stmt_init(this->conn))==NULL) throw 0;
			//if(mysql_stmt_prepare(stmt, query1, (unsigned long)strlen(query1))!=0) throw 1;
			while(mysql_stmt_prepare(stmt, query1, (unsigned long)strlen(query1))!=0)
				sleep(0.5);
			
			bzero(bind, sizeof(bind));
			
			bind[0].buffer_type 		=	MYSQL_TYPE_STRING;
			bind[0].buffer 			=	(void *)ip;
			bind[0].buffer_length		=	strlen(ip);
			bind[1].buffer_type		=	MYSQL_TYPE_STRING;
			bind[1].buffer			=	(void *)geo;
			bind[1].buffer_length		=	strlen(geo);
			bind[2].buffer_type		=	MYSQL_TYPE_SHORT;
			bind[2].buffer			=	&pingstat;
			
			__logging("\nIP: %s\tGEO: %s\tPING: %d\n", ip,geo,pingstat);
			if(mysql_stmt_bind_param(stmt, bind)!=0) throw 2;
			if(mysql_stmt_execute(stmt)!=0) throw 3;
			
			mysql_stmt_store_result(stmt);

			
			if(mysql_stmt_affected_rows(stmt)!=0) {
				ret=this->getTIDbyIP(ip);
				//printf("IP: %s\tGEO: %s\tPING: %d\n", ip,geo,pingstat); //debug
			}

			mysql_commit(this->conn);	
			mysql_stmt_free_result(stmt);
			
			if(mysql_stmt_close(stmt)!=0) throw 4;
	
			return(ret);

	}
	catch(int iResult) {
		switch(iResult) {
				case 0:
					__logging("db::addTarget: MySQL stmt memory alloc error");
					return(-1);
				case 1:
					__logging("db::addTarget: MYSQL prepare statement error");
					return(-1);
				case 2:
					__logging("db::addTarget: MYSQL bind param memory error");
					return(-1);
				case 3:
					__logging("db::addTarget: MYSQL stmt execute error");
					return(-1);
				case 4:
					__logging("db::addTarget: MySQL stmt memory free error");
					return(-1);
				case 5:
					__logging("db::addTarget: Invalid IP: %x", ip);
					return(-1);
				case 6:
					__logging("db::addTarget: Invalid IP: %x", ip);
					return(-1);
				default:
					break;
		}
		
		return(-1);
		
	}
		
	return(-1);
}





/**
 * @brief Add new scans or update if exists
 * @param	_IN tid		the TID (Target ID on discoveryworld database)
 * @param	_IN port	the port
 * @param	_IN proto	the protocol
 * @param 	_IN finger	finger text (html reply)
 * @param 	_IN cert	certificate if exists
 * @returns 	false	is exists or error
 *				true	is added
 * Fix:	not is confirmed update fix possible error but the update line is correctly
 */
bool db::addScan(	unsigned long long tid,
					unsigned int port,
					unsigned int proto,
					char *service,
					char *finger,
					char *cert) {
	const char *query1[] = {
				"INSERT INTO scans (TID,PORT,PROTO,SERVICE,FINGER,CERT) VALUES (?,?,?,?,?,?)",
				"UPDATE scans SET SERVICE=?, FINGER=?, CERT=? WHERE SCANID=?"
				};
	int option=0;
	MYSQL_STMT *stmt;
	MYSQL_BIND bind[6];
	bool ret = false;
	bool data_enable=false;
	bool exists=false;
	unsigned long long scanid;
    	unsigned long length;
	char *tmp=(char *)malloc(2048);
	
	bzero(tmp,2048);
	bzero(bind, sizeof(bind));
	if((exists=this->scan_exists(tid,port,proto))==true) {
		scanid=this->getSCANID(tid,port,proto);
		bind[0].buffer_type 		=	MYSQL_TYPE_STRING;
		bind[0].buffer 			=	(void *)service;
		bind[0].buffer_length		=	50;
		bind[1].buffer_type		=	MYSQL_TYPE_STRING;
		bind[1].buffer_length   	=   	strlen(finger);
		//bind[1].buffer			=	(void *)finger;
		bind[1].length			=	&length;
		bind[1].long_data_used	=	true;
		bind[1].is_null_value			=	0;
		
		bind[2].buffer_type		=	MYSQL_TYPE_VARCHAR;
		bind[2].buffer			=	(void *)cert;
		bind[2].buffer_length		=	strlen(cert);
		bind[3].buffer_type		=	MYSQL_TYPE_LONG;
		bind[3].buffer			=	&scanid;
		option=1;
	}
	else {				
		bind[0].buffer_type		=	MYSQL_TYPE_LONG;
		bind[0].buffer			=	&tid;
		bind[1].buffer_type		=	MYSQL_TYPE_SHORT;
		bind[1].buffer			=	&port;
		bind[2].buffer_type		=	MYSQL_TYPE_SHORT;
		bind[2].buffer			=	&proto;
		bind[3].buffer_type 		=	MYSQL_TYPE_STRING;
		bind[3].buffer 			=	(void *)service;
		bind[3].buffer_length		=	strlen(service);
		bind[4].buffer_type		=	MYSQL_TYPE_STRING;
		bind[4].buffer_length   	=   	strlen(finger);
		bind[4].is_null_value			=	0;
		//bind[4].buffer			=	(void *)finger;
		bind[4].length			=	&length;
		bind[4].long_data_used	=	true;
		
		bind[5].buffer_type		=	MYSQL_TYPE_VARCHAR;
		bind[5].buffer			=	(void *)cert;
		bind[5].buffer_length		=	strlen(cert);
		option=0;
	}
	
	try {
			if((stmt = mysql_stmt_init(this->conn))==NULL) throw 0;
            		if(mysql_stmt_prepare(stmt, query1[option], (unsigned long)strlen(query1[option]))!=0) throw 1;
			if(mysql_stmt_bind_param(stmt, bind)!=0) throw 2;
			int i =0;

			for(i=0;i<(int)strlen(finger);i+=2)
			{
				bzero(tmp, 3);
				memcpy(tmp, finger+i, 2);

				if(option==1)
					mysql_stmt_send_long_data(stmt,1,tmp,(unsigned long)2);
				else
					mysql_stmt_send_long_data(stmt,4,tmp,(unsigned long)2);								
			}
			free(tmp);

				
			if(mysql_stmt_execute(stmt)!=0) throw 3;
			
			mysql_stmt_store_result(stmt);

			
			if(mysql_stmt_affected_rows(stmt)!=0) {
				ret=true;
			}
			
			mysql_commit(this->conn);
			mysql_stmt_free_result(stmt);			
			if(mysql_stmt_close(stmt)!=0) throw 4;
			
			return(ret);
	}
	catch(int iResult) {
		switch(iResult) {
				case 0:
					__logging("db::addScan: MySQL stmt memory alloc error");
					return(-1);
				case 1:
					__logging("db::addScan: MYSQL prepare statement error");
					return(-1);
				case 2:
					__logging("db::addScan: MYSQL bind param memory error");
					return(-1);
				case 3:
					__logging("db::addScan: MYSQL stmt execute error");
					return(-1);
				case 4:
					__logging("db::addScan: MySQL stmt memory free error");
					return(-1);
                case 5:
                    __logging("db::addScan: Error on send buffer");
                    return(-1);
				default:
					break;
		}
		return(false);
		
	}
		
	return(ret);
}
