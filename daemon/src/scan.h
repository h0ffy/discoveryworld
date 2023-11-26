#include "output.h"
//#include "portscan.h"

class discoveryscan {
	private:

	public:
		discoveryscan();
		~discoveryscan();
		static void netscan(void *args); // st_scan
		static void modules(void *args); // st_mods
		static void out(void *args);	// st_out
};


struct st_mods {
	char module_path[2000];
	char module_name[128];
	int module_type;
	struct st_scan *scan;
};



struct st_scan {
	char ip[20];
	char domain[2048];
	int ports[20];
	int protocols[20];
	int services[20];
	struct st_output out;
};

