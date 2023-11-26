#ifndef THREADS_H
#define THREADS_H

#include <pthread.h>
#define MAX_THREADS 100


class threads {
	private:
		pthread_t output_thread;
	public:
		char *next_ip;

		threads(void);
		~threads(void);

		static int portscan_getmutex(void);
		static int portscan_setmutex(int status);
		static int httpsfinger_getmutex(void);
		static int httpsfinger_setmutex(int status);
		
		
		static int scan_abort(void);
		static int scan_del(int index);
		static const int scan_find(void); // find free scan position if desorder fix
		static int scan_available(void);	
		static pthread_t scan_add(const char *ip);
		static int end_join(void);
		
		static void *proc_control(void *args);
		pthread_t add(void *(*func)(void *),void *args);
		
// NO AVAILABLE IN v0.1 UNAVAILABLE
/*
		void output_monitor();

		int output_start(	int method,
					char *path, char *dbname, 
					char *dbuser, char *dbpass,	
					char *dbtable, int dbport );		
*/
};


#endif
