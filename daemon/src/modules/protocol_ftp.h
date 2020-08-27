#ifndef PROTOCOL_FTP_H
#define PROTOCOL_FTP_H

#define MAX_FTP_REPLY 32000
#define MAX_FTP_REQUEST 8000
#define MAX_FTP_NODES 25
#define MAX_FTP_VAR 10
#define MAX_FTP_VAL 50
#define FTP_EOL "\r\n"

struct st_ftp {
    char *var;
    char *val;
    bool status=false;
};


class ftp {
  private:
      netutils *net;
      int last_node;
      bool reply_status=false;
      void reply_add(const unsigned char *reply);
      struct st_ftp nodes[MAX_FTP_NODES]; // non-lambda
      
  public:
      ftp();
      ~ftp();
      char *reply;
      char *request;
      bool open(const char *ip, const int port);
      void request_add(const char *var, const char *val);
      void request_init();
      bool finger(const char *ip, const int port);
      void request_make(int offset);
      void build();
      
  
};

#endif