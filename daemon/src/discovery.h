#ifndef DISCOVERY_H
#define DISCOVERY_H
#include "output.h" 

class discovery {
	private:
		//int status;
	public:
		discovery(void);
		~discovery(void);
		
		static void *scan(void *args);
		static int http_finger(const char *host, int port,const char *data);
};


int mod_http(char *ip, int port, st_output_node *node);

struct st_scan {
	int index;
	char *ip;
};


struct st_modules {
	char *name;
	bool cipher_enable=false;
	int (*func)(char *ip, int port, st_output_node *node);
};



#endif
