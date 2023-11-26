#ifndef OUTPUT_H
#define OUTPUT_H


#define MAX_NODES 15
#define MAX_OUTPUT_FINGER 82500
 
#include "db.h"
class output {
	private:
		char *path;
		int last_node;
		int next_node;
		int output_method;
		struct st_output_node *nodes[12];
		db *database;
	public:	
		
		output(void);

		~output(void);


		static void *monitor(void *args);

		int push(struct st_output_node *node);		
		int pop(struct st_output_node *node);
		int build(void);
		unsigned long long target(const char *ip, const char *geo, int pingstat);	
};


struct st_output_node {
	unsigned long long tid;
	unsigned int port;
	unsigned int proto;
	char *service;
	char *finger;
	char *cert;
	int status;	
};
#endif
