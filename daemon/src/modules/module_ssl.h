#ifndef H_MODULE_SSL
#define H_MODULE_SSL

#define MAX_SSL_BUFF 82000

class ssl {
private:
    ssl_context ctx;
    x509_crt cacert;
    entropy_context entropy;
    ctr_drbg_context ctr_ctx;
    unsigned char *request;
    int sock;
  
public:
    ssl();
    ~ssl();
    
    bool open(const char *ip,int port);
    int connect();
    int send(const unsigned char *data,int len);
    int recv();
    static bool testport(char *host, unsigned short port,int proto);
  
    unsigned char *reply;
    
    //SSL_METHOD *method;
    //SSL_CTX *ctx;

  
};



#endif