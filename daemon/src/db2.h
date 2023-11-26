#ifndef DB_H
#define DB_H
#include <cassandra.h>

class db {
	private:
		int dummy;
		CassFuture* connect_future;
		CassCluster* cluster;
		CassSession* session;
		unsigned long long message_length;
		const char *message;
		char *query;
		char *dbtable;
		int status;

	public:
/*		db(	unsigned short db_type, char *dbhost, char *dbname,
			char *dbuser, char *dbpass, char *dbtable, int dbport );*/
		db();
		~db();
	
		bool test_db(void);
		void db_query_make(const char *ip, struct st_output_node *nodes);
		bool hosts_exists(const char *ip);
		bool scan_exists(unsigned long long tid,int port,int proto); 
		unsigned long long getTIDbyIP(const char *ip);
		unsigned long long getSCANID(unsigned long long tid,int port,int proto); 
		bool addScan(unsigned long long tid, unsigned int port, unsigned int proto, char *service, char *finger, char *cert); /* Fix: Mysql lib need send finger with send_long_data api */
		
		
		
		unsigned long long addTarget(const char *ip,const char *geo, int pingstat);
};
#endif
