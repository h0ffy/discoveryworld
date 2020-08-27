#ifndef NETSTATUS_H
#define NETSTATUS_H

class netstatus {
protected:
    netstatus();
    ~netstatus();
  
public:
  static bool ping(const char *ip);
  static bool tcptest(const char *ip, unsigned short port);
  static void *monitor(void *args);
 
};




#endif