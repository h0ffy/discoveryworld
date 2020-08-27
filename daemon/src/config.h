#ifndef CONFIG_H
#define CONFIG_H
#include <libconfig.h>

#define DEFAULT_CONFIG_PATH "/opt/discoveryworld/etc/discoveryworld.conf"
#define MAX_SERVICES 4

#define METHOD_CSV			0
#define METHOD_MYSQL			1
#define METHOD_HTTP		 	2
#define METHOD_BERKELEY			3

#define CONF_METHOD_IPRANDOM 	0
#define CONF_METHOD_IPFILE 	1



#define USER_AGENT "Mozilla/5.0 (X11; Linux x86_64; rv:38.0) Gecko/20100101 Firefox/38.0 Iceweasel/38.8.0"

class config {
	private:
		const char *filepath;
		FILE *f;
		config_t conf;		
		struct sconf *discoveryconf;
	public:
		int method;
		int status;

	config(const char *filepath);
	~config(void);

	static const char *get_output_log_error(void);
	static const char *get_intput_filepath(void);
	static const char *get_interface(void);
	static const int get_resume(void);
	static const int get_max_threads(void);
	static const int get_num_process(void);
	static const int get_tcpsyn_timeout(void);
	static const int get_socket_timeout(void);
	static const char *get_dbname(void);
	static const char *get_dbhost(void);
	static const char *get_dbuser(void);
	static const char *get_dbpass(void);
	static const int get_dbport(void);
	static const char *get_dbtable(void);
	static const int get_port(int index);
	static const int get_service(int index);
	static const int get_servicebyport(int port);
	static const int get_inputmethod(void);
	static const int get_outputmethod(void);
	static const char *get_inputfile(void);
	static const char *get_agenttoken(void);
	static const char *get_agentip(void);
	static const char *get_httptoken(void);
	static const int *get_httptype(void);
	static const char *get_httppath(void);
	static const char *get_httphost(void);
	static const unsigned short get_httpport(void);
	
	static const char *get_pingtestip(void);
	static const char *get_tcptestip(void);
	static const unsigned short get_tcptestport(void);
	
	
};


	
struct sconf {
	char *filepath;
	char *interface;
	int ports[65536];
	int ports_count;
	int services[65536];
	int services_count;
	char *ip_from;
	int  port_from;
	int scan_type;
	int max_threads;
	int number_of_process;
	int resume;
	int tcpsyn_timeout;
	int socket_timeout;


	int output_method;
	char *output_dbhost;
	char *output_dbname;
	char *output_dbuser;
	char *output_dbpass;
	char *output_dbtable;
	int output_dbport;
	char *output_csv;

	char *output_log_error;
	char *output_log_raw;


	int input_method;
	char *input_filepath;	

	char *agent_ip;
	int agent_port;
	char *agent_token;
};
#endif
