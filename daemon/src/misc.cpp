/*
Copyright 2016 JennySec. All rights reserved.

Redistribution and use in source and binary forms, with or without modification, are
permitted provided that the following conditions are met:

 1. Redistributions of source code must retain the above copyright notice, this list of
    conditions and the following disclaimer.

 2. Redistributions in binary form must reproduce the above copyright notice, this list
    of conditions and the following disclaimer in the documentation and/or other materials
    provided with the distribution.

THIS SOFTWARE IS PROVIDED BY JennySec ''AS IS'' AND ANY EXPRESS OR IMPLIED
WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND
FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL <COPYRIGHT HOLDER> OR
CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON
ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING
NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF
ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

The views and conclusions contained in the software and documentation are those of the
authors and should not be interpreted as representing official policies, either expressed
or implied, of JennySec.
*/
/**
 * @file misc.c
 * @brief Misc utils
 * @author JennySec
**/

#include <bsd/bsd.h>
#include <bsd/stdlib.h>
#include <bsd/string.h>
#include <bsd/stdio.h>
#include <bsd/unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <time.h>
#include <arpa/inet.h>
#include <signal.h>
#include <assert.h>
#include "config.h"
#include "misc.h"

#define TOTALVAL 254

unsigned char denom = TOTALVAL+1;
int vals[TOTALVAL] = {
    1, 2, 3, 4, 5, 6, 7, 8, 9, 10,
   11, 12, 13, 14, 15, 16, 17, 18, 19, 20,
   21, 22, 23, 24, 25, 26, 27, 28, 29, 30,
   31, 32, 33, 34, 35, 36, 37, 38, 39, 40,
   41, 42, 43, 44, 45, 46, 47, 48, 49, 50,
   51, 52, 53, 54, 55, 56, 57, 58, 59, 60,
   61, 62, 63, 64, 65, 66, 67, 68, 69, 70,
   71, 72, 73, 74, 75, 76, 77, 78, 79, 80,
   81, 82, 83, 84, 85, 86, 87, 88, 89, 90,
   91, 92, 93, 94, 95, 96, 97, 98, 99, 100,
   101, 102, 103, 104, 105, 106, 107, 108, 109, 110,
   111, 112, 113, 114, 115, 116, 117, 118, 119, 120,
   121, 122, 123, 124, 125, 126, 127, 128, 129, 130,
   131, 132, 133, 134, 135, 136, 137, 138, 139, 140,
   141, 142, 143, 144, 145, 146, 147, 148, 149, 150,
   151, 152, 153, 154, 155, 156, 157, 158, 159, 160,
   161, 162, 163, 164, 165, 166, 167, 168, 169, 170,
   171, 172, 173, 174, 175, 176, 177, 178, 179, 180,
   181, 182, 183, 184, 185, 186, 187, 188, 189, 190,
   191, 192, 193, 194, 195, 196, 197, 198, 199, 200,
   201, 202, 203, 204, 205, 206, 207, 208, 209, 210,
   211, 212, 213, 214, 215, 216, 217, 218, 219, 220,
   221, 222, 223, 224, 225, 226, 227, 228, 229, 230,
   231, 232, 233, 234, 235, 236, 237, 238, 239, 240,
   241, 242, 243, 244, 245, 246, 247, 248, 249, 250,
   251, 252, 253, 254
};


int process_pids[10] = {0};


/**
 * @brief Make random IP
 * @param	any
 * @returns 	str		generate ip
 *          	
 */
char *random_ip(void) {
	unsigned char byte_a, byte_b, byte_c, byte_d;
	struct in_addr ip;
	do {
		byte_a = random_byte();
		byte_b = random_byte();
	}while(!is_valid_ipbytes(byte_a,byte_b));

	byte_c = random_byte();
	byte_d = random_byte();
		
	ip.s_addr = (byte_a |
				(byte_b << 8) |
				(byte_c << 16) |
				(byte_d << 24));

	 	
		
		
	return(inet_ntoa(ip));
	
}



/**
 * @brief Make byte
 * @param	any
 * @returns 	uchar random byte
 *          	
 */
/*
unsigned char random_byte(void) { 
    unsigned char i, random_val;
	srand(time(NULL));

    if (denom == 1)
            denom =  TOTALVAL+1;
    i = rand() % denom; 
    random_val = vals[i];
    vals[i] = vals[--denom];
    vals[denom] = random_val;
    return random_val;
}
*/
//NOTE change openfile method (fopen libc file i/o) to (unistd syscall) 
unsigned char random_byte(void) {
	FILE *f = fopen("/dev/urandom", "rb");
	char *data = (char *)__allocate(1);
	int a = 0;
	
	if(f) {
		assert(f);
		if(!(fread(data,1,1,f)))
			return(0);
		
		a = (int)data[0];
		fclose(f);
		__liberty(data);
		return((unsigned char)a);
	}
	
	__liberty(data);
	
	return(0);
	
}

/**
 * @brief Test is valid IP bytes A.B.C.D (10.0.0.1)
 * @param	_IN	byte_a	byte A of ip
 * @param	_IN	byte_b	byte B of ip
 * @returns 	str		generate ip
 *          	
 */
int is_valid_ipbytes(unsigned char byte_a,unsigned char byte_b) {
	switch(byte_a) {
		case 0:
		case 10:
		case 127:
		case 255:
				return(0);
		case 172:
		case 192:
			if(byte_b==168)
				return 0;
			break;
		default:
			break;
		
	}

	return(1);
}






/**
 * @brief Log function (equal than printf)
 * @param 	_IN format to impresion
 * @param 	_IN	... var 
 * @returns  ANY
 *
 */
int __logging(const char *format, ...) {
        va_list args;
        FILE *f;

        if(!(f = fopen(config::get_output_log_error(), "a+"))) {
                perror("Error log open");
                abort();
        }


        va_start(args, format);
			vfprintf(f, format, args);
        va_end(args);


        fclose(f);
        return 0;
}


//no fixed bug: Predictable random string (this application include random_byte in misc
char *random_string(int len) {
    static const char charset[] = "0123456789"
				  "abcdefghijklmnopqrstuvwxyz"
				  "ABCDEFGHIJKLMNOPQRSTUVWXYZ";
	char *mem = (char *)malloc(len*2);
	char *buff = mem;
	bzero(mem,len*2);
	srand(time(NULL));

    while (len-- > 0) {
	    size_t index = (double) rand() / RAND_MAX * (sizeof charset - 1);
        *buff++ = charset[index];
    }
	
    *buff = '\0';
	
	return(mem);
}

char *get_tmpfile(void) {
	char *path = (char *)malloc(512);
	char *buff = random_string(20);
	bzero(path, 512);
	sprintf(path,"/tmp/%s.discovery",buff);
	free(buff);
	return((char *)path);
}

void *__reallocate(void *data, size_t size) {
    void *mem;
    
    if(data==NULL) {
      __logging("reallocate: Null pointer\n");
      return(NULL);
    }
    
    if((mem=realloc(data,size))==NULL)
      __logging("reallocate: Error on memory reallocation");
    
    
    return(mem);
      
}

void *__allocate(int len) {
    void *data = malloc(len);
    
    if(data==NULL) {
	__logging("allocate: Invalid memory alloc\n");
	return(NULL);
    }
    
    bzero(data,len);
    return(data);
}

void __liberty(void *data) {
    return(free(data));
}


bool process_childs(int count,int offset) {
    int i = offset;
    int pid;
    for(i=offset;i<=count;i++) { 
      if ((pid = fork()) < 0) {
	  __logging("[misc.cpp] process_childs: Forking error");
	  return(false);
      }
      else if (pid > 0) {
	  process_pids[i]=pid;
	  return(true);
      }
    }
    return(true);
}

int get_processpid(int pos) {
  if(pos>10)
    return(-1);  

  if(getpgid(process_pids[pos])==-1)
    return(-1);

  return(kill(process_pids[pos],0));
}


int set_processpid(int pos, int pid) {
  if(pos>10 || pid <= 0)
    return(-1);
  
  return((process_pids[pos]=pid));
}



/* kiddie function
char *__random_ip(void) {
	int seed=31337;
	int num1=0,num2=0,num3=0,num4=0;
	char *ptr = (char *)calloc(1,1024);

	srand(seed);
	while(num1==0 || num1==255)
		num1=rand()%255;
	
	num2=rand()%255;sleep(0.1);
	num3=rand()%255;sleep(0.1);
	num4=rand()%255;sleep(0.1);
	
	sprintf(ptr, "%d.%d.%d.%d", num1, num2, num3, num4);

	return(ptr);
}
*/
