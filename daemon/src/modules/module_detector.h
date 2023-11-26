#ifndef MODULE_DETECTOR_H
#define MODULE_DETECTOR_H

class detector {
  protected:
    detector();
    ~detector();
    
  public:
    static int process_data(void *data, int datalen);
    static bool seclayer(char *host, int port);
    static int service_tcp(char *host, int port);
};

#endif