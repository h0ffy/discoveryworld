#ifndef H_SERVICES
#define H_SERVICES


struct st_service {
  int serviceid;
  const char *name;
  void (*chk_func)(const char *ip, st_output_node *node);
};

class services {
public:
  static bool srvcheck(const char *ip, st_output_node *node);
  static void chk_http(const char *ip, st_output_node *node);
  static void chk_https(const char *ip, st_output_node *node);
  static const char *getServiceByID(int id);
};


#endif