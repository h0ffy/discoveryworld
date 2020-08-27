#ifndef NET_UTILS_H
#define NET_UTILS_H

#define ANY "ANY"

class netutils {
	private:
		int dummy;
		
	public:
		int sock;
		netutils(void);
		~netutils(void);
		
		static int ping(const char *host);
		static char *geoip(const char *host);
		static int resolve(const char *host, char *ip);
		static bool isvalidIPV4(const char *ip);
		static bool isvalidIPV6(const char *ip);
		static unsigned short isIPV4orIPV6(const char *ip);
		static unsigned long convIPV6toIPV4(const char *ip6);
		int tcp_open(const char *host,int port);
		int tcp_send(char *data,int len);
		int tcp_recv(char *data,int len); 
		void tcp_close(void);
};
#endif
