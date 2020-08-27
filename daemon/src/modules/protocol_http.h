#ifndef MODULE_HTTP_H
#define MODULE_HTTP_H

#ifndef MAX_HTTP_BUFF
    #define MAX_HTTP_BUFF 82000
#endif 


#define MAX_METHODS 7
#define MAX_HEADER_NODES 25
#define MAX_DATA 8000
#define MAX_HTTP_HOST 256
#define HTTP_END_LINE "\r\n"
#define HTTP_FOOT_LINE "\n\n"
#define HTTP_METHOD_GET 	0
#define HTTP_METHOD_POST 	1
#define HTTP_METHOD_HEAD	2
#define HTTP_METHOD_OPTIONS	3


#define HTTP_USER_AGENT "Mozilla/5.0 (X11; Linux x86_64; rv:38.0) Gecko/20100101 Firefox/38.0 Iceweasel/38.8.0" \
			"Accept: text/html,application/xhtml+xml,application/xml;q=0.9,*/*;q=0.8" \
			"Accept-Language: en-US,en;q=0.5"



struct http_header_node {
	char *var;
	char *value;
	int status;
};

//C wrapping class
struct http_request {
	int method;
	char *path;
	int version;
	http_header_node *nodes[MAX_HEADER_NODES];
	int header_length;
	int node_count;
	unsigned char *data;
	
};


class http {
	private:
		char *host;
		
		int method;
		int version;
		int status;
		int port;
		char *path;
		char *data;
		unsigned char *packet;
		unsigned char *packet_long;
		int packet_size;
		http_header_node nodes[MAX_HEADER_NODES];
		int nodes_count;
		int reply_count;
		http_request req;
		
		bool request_memory(void);
		
	public:
		char *reply;
		char *request;
		http();
		~http();
		
		bool scan_make(const char *host, int method, const char *path, const char *user_agent);
		bool request_add_header(const char *header, const char *value);
		int finger(const char *host,int port);
		bool request_init(int method,const char *path, int version,const char *host,int port);
		bool request_add_data(char *data);
		bool request_headextract();
		void request_make();
		bool request_run();
		
		//bool request_add_data(char *data);

};



#endif
