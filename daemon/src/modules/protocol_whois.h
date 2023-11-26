#ifndef PROTOCOL_WHOIS_H
#define PROTOCOL_WHOIS_H

#define MAX_WHOIS_REPLY 25000
#define MAX_WHOIS_SERVERS 16

class whois {
public:
      whois();
      ~whois();
      
      char *reply;
      bool query(char *data);
};


#endif //PROTOCOL_WHOIS_H