#ifndef MODULE_HTTPS_H
#define MODULE_HTTPS_H
#define MAX_HTTPS_BUFF 82000
#define MAX_CERTS 20


class https {
private:
    ssl  *sl;
    http *lhttp;
public:
    
    char *request;
    unsigned char *reply;
    int reply_count;
    https();
    ~https();
    bool request_init(int method,const char *path, int version,const char *host,int port);
    bool request_add_data(char *data);
    bool request_add_header(const char *header, const char *value);
    void request_make();
    bool request_run();
    bool request_headextract();
    bool finger(const char *ip, int port);
    
};

extern "C" {
    char *https_finger(const char *host,int port);
    size_t https_writedata(void *ptr, size_t size, size_t nmem, void *stream);
    int https_end(void);
    char *https_cert(const char *host, int port);
    //static size_t wrfu(void *ptr,  size_t  size,  size_t  nmemb,  void *stream);
}

#endif