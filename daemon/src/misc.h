#ifndef MISC_H
#define MISC_H


extern "C" {
	int __logging(const char *format, ...);
	char* random_ip(void);
	unsigned char random_byte(void);  
	int is_valid_ipbytes(unsigned char byte_a,unsigned char byte_b);
	void initialize(void); // code in main
	char *get_tmpfile(void);
	char *random_string(int len);
	//int get_tmpfile(char *path);
	/*char* __itoa(int num, char* str, int base);
	void __reverse(char str[], int length);
	void __swap(char *a, char *b);
*/
	void __liberty(void *data);
	void *__reallocate(void *data, size_t size);
	void *__allocate(int len);
	bool process_childs(int count,int offset);
	int get_processpid(int pos);
	int set_processpid(int pos, int pid);
	
	
}


#endif
