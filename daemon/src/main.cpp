#include <bsd/bsd.h>
#include <bsd/stdio.h>
#include <bsd/stdlib.h>
#include <bsd/libutil.h>
//#include <unistd.h>
#include "input.h"
#include "config.h"
#include "net_utils.h"
#include "misc.h"
#include "threads.h"
#include "output.h"
#include "netstatus.h"
#include "discovery.h"
/*#include "modules/module_ssl.h"
#include "modules/protocol_http.h"
#include "modules/module_https.h"*/
#include "modules/module_agent.h"



void signal_callback_handler(int signum);

char *last_ip;
unsigned long last_point;
extern bool g_th_freeze;
extern int g_watchdog;

int main(int argc, char *argv[]) {
	/*pthread_t hThread;
	struct st_scan *scan;
	int i =0;
	int x = 0;
	*/	
	FILE *f;
	
		
	if(getuid() && geteuid()) {
		printf("Cannot be root\n");
		abort();
	}
	
	if(argc!=3) {
		printf("%s -c discoveryworld.conf\n", argv[0]);
		fflush(stdout);
		exit(0);
	}
	else if(strncmp(argv[1],"-c",2)!=0) {
		printf("%s -c discoveryworld.conf\n", argv[0]);
		fflush(stdout);	
		exit(0);
	}

	if(!(f=fopen(argv[2],"r"))) {
	      printf("Configuration file no exist");
	      fflush(stdout);
	      exit(0);
	}
	else
	    fclose(f);
	
	
	
	unsigned char *ip = (unsigned char *)__allocate(128);
	last_ip = (char *)__allocate(128);
	bzero(last_ip,128);
	//bzero(ip,20);


	config *conf = new config(argv[2]);
	initialize();
	input *in = new input();
	threads *th = new threads();	// thread class initialize only GCC -pedantic detect it how non-used but is false i lost 15min debugging it f*
	netutils *net = new netutils();
	
	th->add(threads::proc_control,NULL);
	//th->add(netstatus::monitor,NULL); /* Network Monitor Status (Test status).
	
	__logging("START IT\t\t\t[OK]\n");
	while(in->next(ip)==1) {
		while(g_th_freeze==true)
		  sleep(5);
		if(g_watchdog==0) {
		    __logging("Watchdog error\n");
		    abort();
		}
		  
		
		bzero(last_ip,128);
		strlcpy(last_ip,(const char*)ip,128);
	  
		//non ipv6 implementation
		if(ip==(unsigned char *)NULL || ip==(unsigned char *)0xFFFFFFFF || net->isvalidIPV4((const char *)ip)==false)
			continue;

		while(threads::scan_available()==0)
		  sleep(3);
			
		threads::scan_add((const char *)ip); //(const char *)ip);
		last_point++;
		bzero(ip, 128);

	}
	

//End Of Code
	threads::end_join();
	__logging("End of scan... bye bye :D\n");
    	//https_end();
	__liberty(ip);
	__liberty(last_ip);
	exit(0);
	return 0;
}

void initialize(void) {
	db *database = new db();
	try {	

		database->test_db() ? __logging("DATABASE\t\t\t[OK]\n") : 	throw 0;

	}
	catch(int iResult) {
		switch(iResult) {
			case 0:
				__logging("Test is database contains all tables");
				exit(0);
			default:
				break;
		}
		
		abort();
	}

/*
	if(	signal(SIGINT,signal_callback_handler)==SIG_ERR 	||
		signal(SIGQUIT,signal_callback_handler)==SIG_ERR	||
		signal(SIGSEGV,signal_callback_handler)==SIG_ERR	||
		signal(SIGKILL,signal_callback_handler)==SIG_ERR	||
		signal(SIGSTKFLT,signal_callback_handler)==SIG_ERR) 
		{
			perror("Error on make own signal callback but not is critical");
			fflush(stdout);
		}*/


	if(getpid()==1)
		return;

	pid_t sid,pid;
	int fd;	

		
		
		//Double forking (Demonize)
		
        if ((pid = fork()) < 0) {
                perror("fork");
                exit(EXIT_FAILURE);
        }

        else if (pid == 0) {
                if ((pid = fork()) < 0) {
                    perror("fork");
                    exit(EXIT_FAILURE);
                }
                else if (pid > 0) {
                    exit(EXIT_SUCCESS);
                }

                exit(EXIT_SUCCESS);
        }
	
	
        if((sid=setsid())<0) {
                perror("Cannot setsid()");
                exit(EXIT_FAILURE);
        }
		
	umask(027);
	chdir("/tmp/");
	      
	fd = NULL;//open("/dev/null",O_RDWR, 0);

	if (fd != -1) 
	{   
	      dup2(fd, STDIN_FILENO);
	      dup2(fd, STDOUT_FILENO);
	      dup2(fd, STDERR_FILENO);

	      if (fd > 2)
		close(fd);
	}
	else {
	    umask(0);
	    close(0);close(1);close(2);
	}
	
	__logging("Make childs process\t\t\t");
	if(config::get_num_process()>1) 
	  process_childs(config::get_num_process(),0);
	__logging("[OK]\n");
      
	//delete db;
	/*umask(0);
        close(0);close(1);close(2);*/
}

void signal_callback_handler(int signum) {
        if(config::get_inputmethod()==CONF_METHOD_IPFILE) {
                if(last_ip!=NULL) {
                        __logging("ERROR: The last IP is: %s\nLast line number is: %d\n", last_ip,last_point);
                }

                else {
                        __logging("ERROR: The last IP is not valid but the last point is: %d\n", last_point);

                }

		//config::config_set_resume(last_point);
               //config::config_set_lastip(last_ip);
        }

        else {
                printf("ERROR: The last IP is: %s\n", last_ip);
                __logging("ERROR: The last IP is: %s\n", last_ip);
                //config::config_set_lastip(last_ip);
        }
        free(last_ip);
        exit(signum);
        abort(); //this line not execute but is possible in case of zombie on exit error 
}

