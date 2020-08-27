#ifndef MODULE_SSH_H
#define MODULE_SSH_H

#define MAX_SSH_SIZE 8000

class ssh {

    public:
        ssh();
        ~ssh();
        int finger(const char *host, int port); 
	bool loginpw(const char *host, int port, const char *user, const char *password); 	// idea no written cpp code
        bool logincert(const char *host, int port, void *cert);					// idea no written
	char *cert,*reply;
};
#endif