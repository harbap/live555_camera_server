#ifndef _WRITE_LOG_H_
#define _WRITE_LOG_H_


#include <stdio.h>
#include <string.h>

void print_log(const char *fmt,...);
int get_log_files_num(const char *path);
void print_data(char *filename,char *data,int len);

#endif
