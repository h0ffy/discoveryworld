#ifndef MODULE_AGENT_H
#define MODULE_AGENT_H

#define MAX_AGENT_BUFF 1024
class agent {
	private:
		char *request;
	public:
		agent();
		~agent();
		int send_resolve(char *ip);
		int send_web(char *ip,int port,char *service);

};

#endif
