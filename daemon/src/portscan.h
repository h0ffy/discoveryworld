#ifndef PORTSCAN_H
#define PORTSCAN_H

#define PORTSCAN_TABLE_MAX 1000

#define PORTSCAN_TYPE_SYNACK 0
#define PORTSCAN_TYPE_ACK 1
#define PORTSCAN_TYPE_RST 2
#define PORTSCAN_TYPE_SYNFIN 3
#define PORTSCAN_TYPE_FINACK 3


struct portscan_table {
      int id;
      unsigned char *ip;
      unsigned long uip;
      int port;
      int protocol;
      int scan_type;    //0 syn-ack, 1 ack, 2 rst, 3 syn-fin
      bool node_status;
      bool exec_status;
      bool port_status;
      
};

struct portscan_queue {
    struct pktp_pkthdr *pkthdr;
    u_char data[];
};

class portscan {
	private:
		int dummy;
		static int portscan_getid(unsigned long ip, int port);
	public:
		portscan();
		~portscan();
		//single thread 
		static bool tcp_syn(char *host, int port);
		static int tcp_connect(char *ip, int port); 
		
		
		//multithread portscan poll
		static void portscan_thread(void *args);
		static int portscan_add(const char *ip, int port,int protocol);
		static int portscan_del(int id);
		static bool portscan_status(int id);
		static bool portscan_isopen(int id);
		static bool portscan_isexec(int id);
		
		static void portscan_waitexec(int id);
		static void portscan_handler(unsigned char *user, const struct pcap_pkthdr *packet, const unsigned char *bytes);
};



#endif

/*
class portscan {
	private:
		int dummy;

	public:
		portscan();
		~portscan();

		
		static bool scan_tcpsyn(const char *ip, const unsigned int port);
		static bool scan_tcpconnect(const char *ip, const unsigned int port);
		static bool scan_udpconnect(const char *ip, const unsigned int port);
};
*/
