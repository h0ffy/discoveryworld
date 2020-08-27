/**
 * @file threads.c
 * @brief Threads control
 * @author JennySec
**/

#include <bsd/bsd.h>
#include <bsd/stdlib.h>
#include <pthread.h>
#include "threads.h"
#include "discovery.h"
#include "misc.h"
#include "output.h"
#include "config.h"


pthread_mutex_t portscan_mutex;
pthread_mutex_t httpsfinger_mutex;
pthread_t scan_threads[MAX_THREADS];
int portscan_status=0;
int portscan_mutex_status=0;
int httpsfinger_status=0;
int httpsfinger_mutex_status=0;
struct st_scan scan[MAX_THREADS];
int thread_stat[MAX_THREADS] = {0};



threads::threads(void) {
	this->next_ip = (char *)malloc(128);bool th_freeze=false;
	bzero(this->next_ip, 128);
	portscan_status=0;
	pthread_mutex_init(&portscan_mutex, NULL);
	portscan_mutex_status=1;
	//pthread_detach(pthread_self());
	//idx=0;
}
threads::~threads(void) {
	free(this->next_ip);
	this->scan_abort();
	if(portscan_mutex_status==1) {
		pthread_mutex_destroy(&portscan_mutex);
		portscan_mutex_status=0;
		portscan_status=0;
	}
}


/**********************
		SCAN
***********************/

/**
 * @brief Wait to scan find
 * @param 	no params
 *
 * @returns 	hell	in any case
 *
 */
int threads::scan_available(void) { 
	threads::scan_find();
	return 666;
}


/**
 * @brief This function make thread
 * @param 	args 	arguments
 *
 * @returns 	0 	close
 *          	1 	open
 */
pthread_t threads::add(void *(*func)(void *),void *args) {
      pthread_attr_t attr;
      pthread_t tthread;
      
      pthread_attr_init(&attr);
      pthread_attr_init(&attr);
      pthread_attr_setinheritsched(&attr, PTHREAD_EXPLICIT_SCHED);
      //pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_DETACHED);
      pthread_create(&tthread, &attr, func, args);
      pthread_attr_destroy(&attr);
      
}



/**
 * @brief This function make scan thread with param IP
 * @param 	ip		ip to scan 
 *
 * @returns 	0 	close
 *          	1 	open
 */
pthread_t threads::scan_add(const char *ip) {
	int idx = threads::scan_find();
	
	pthread_attr_t attr;

	scan[idx].index = idx;
	scan[idx].ip = (char *)malloc(128);
	bzero(scan[idx].ip,128);
	strlcat(scan[idx].ip, ip, strlen(ip)+1);
	thread_stat[idx] = 1;
	
	
	pthread_attr_init(&attr);
	pthread_attr_setinheritsched(&attr, PTHREAD_EXPLICIT_SCHED);
	//pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_DETACHED);
	pthread_create(&scan_threads[idx], &attr, discovery::scan, (void *)&scan[idx]);
	pthread_attr_destroy(&attr);

	return(scan_threads[idx]);
}


/**
 * @brief Abort all threads
 * @param 	no params
 *
 * @returns 	0 	close
 *          	1 	open
 */
int threads::scan_abort(void) { 
	volatile int i = 0;

	for(i=0;i<=config::get_max_threads();i++) 
		threads::scan_del(i);

	return 0;
}


/**
 * @brief Terminate and clean selected thread
 * @param 	index	index of thread 
 *
 * @returns 	0 	close
 *          	1 	open
 */
int threads::scan_del(int index) {
	thread_stat[index]=0;
	pthread_exit(0);
	return 0;
}


/**
 * @brief Find free index
 * @param 	no params
 *
 * @returns 	index value
 *
 */
const int threads::scan_find(void) { 
	int i = 0;
	while(thread_stat[i]!=0) {
		if(i==config::get_max_threads())
			i=0;
		else
			i++;
	
	}
	
	return(i);
} // find free scan position if desorder fix



// PORTSCAN MUTEX CONTROL
/**
 * @brief Test is enable mutex portscan
 * @param 	no params
 *
 * @returns 	0	unlock
 *				1	locked
 *
 */
int threads::portscan_getmutex(void) 	{	return(portscan_status);	}


/**
 * @brief Set portscan mutex
 * @param 	status		0=unlock	1=lock
 *
 * @returns 	0	unlock
 * @returns		1	locked
 *
 */
int threads::portscan_setmutex(int status) {

	if(portscan_mutex_status==0) 	{	return(0);	}
	
	if(status==0) {
		if(pthread_mutex_unlock(&portscan_mutex)==0) {
			portscan_status=0;
			return(1);
		}
	}
	else {
		portscan_status=1;
		if(pthread_mutex_lock(&portscan_mutex)==0)
			return(1);
	}
	
	return(0);

}




// PORTSCAN MUTEX CONTROL
/**
 * @brief Test is enable mute httpsfinger
 * @param 	no params
 *
 * @returns 	0	unlock
 *				1	locked
 *
 */
int threads::httpsfinger_getmutex(void) 	{	return(httpsfinger_status);	}


/**
 * @brief Set httpsfinger mutex
 * @param 	status		0=unlock	1=lock
 *
 * @returns 	0	unlock
 * @returns		1	locked
 *
 */
int threads::httpsfinger_setmutex(int status) {

	if(httpsfinger_mutex_status==0) 	{	return(0);	}
	
	if(status==0) {
		if(pthread_mutex_unlock(&httpsfinger_mutex)==0) {
			httpsfinger_status=0;
			return(1);
		}
	}
	else {
		httpsfinger_status=1;
		if(pthread_mutex_lock(&httpsfinger_mutex)==0)
			return(1);
	}
	
	return(0);

}


void *threads::proc_control(void *args) {
      int count=0;
      
      if((count=config::get_num_process())<=0)
	count=1;
      
      while(1) {
	  int i = 0;
	  for(i=0;i<=count;i++) {
	      if(get_processpid(i)==-1) {
		process_childs(1,i);
	      }
	      sleep(0.1);
	  }
	  
	  sleep(5);
      }
      
      pthread_exit(0);
}


int threads::end_join() {
	int b=0;
	int i=0;
	
	for(b=0;b<MAX_THREADS;b++) {
		i=0;
		for(i=0;i<MAX_THREADS;i++)
			pthread_join(scan_threads[i],NULL);

		for(i=0;i>MAX_THREADS;i--)
			pthread_join(scan_threads[i],NULL);

	}
	return 0;
}




