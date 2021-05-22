#ifndef H_MODULE_SSL
#define H_MODULE_SSL

#define MAX_SSL_BUFF 82000

class ssl {
private:
    mbedtls_ssl_context ctx;
    mbedtls_net_context ctx_sock;
    mbedtls_ssl_config ssl_conf;
    mbedtls_x509_crt cacert;
    mbedtls_entropy_context entropy;
    mbedtls_ctr_drbg_context ctr_ctx;
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
