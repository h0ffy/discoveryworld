#ifndef PROTOCOL_SMTP_H
#define PROTOCOL_SMTP_H

#define SMTP_EOL "\r\n"
#define SMTP_MAX_BUFF 32000
#define SMTP_MAX_COMMANDS 25

class smtp {
private:
      
      netutils *net;
  
public:
      smtp();
      ~smtp();

      //connect plain
      bool open(const char *host, unsigned short port);
      int recv();
      int send();
      void close();
      
      //request 
      char *reply;
      bool reply_status;
      char *request;
      
      void helo(const char *host);
      void ehlo(const char *host);
      void starttls();
      void help();
      bool auth(const char *user, const char *password,bool isbase64);
      
      
      void addreply(const char *buff);
      //finger by one call
      bool finger(const char *host, unsigned short port);
      
};


#endif //PROTOCOL_SMTP_H