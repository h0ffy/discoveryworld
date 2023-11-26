#ifndef INPUT_H
#define INPUT_H

class input {
	private:
		int status;
		FILE *f;
		int method;
		int actual_line;
	public:
		char *filepath;


	input(void);
	~input(void);

	int inputfromfile(char *filepath);	
	int next(void *args);
	int get_status(void);
	int set_filepath(char *filepath);		
		
};
#endif
