/*************************************************************************
	> File Name: cmd_fliter.c
	> Author: jerboa
	> Mail: a.jerboa@gmail.com
	> Created Time: 2014年05月19日 星期一 10时11分18秒
 ************************************************************************/

#include <stdio.h>
#include "apue.h"
#include <ctype.h>

int main(void)
{
	char line[MAXLINE];
	FILE *fpin;

	if((fpin = popen("upper_lower", "r")) == NULL)
		err_sys("popen error");
	for( ; ; )
	{
		fputs("prompt> ", stdout);
		fflush(stdout);
		if(fgets(line, MAXLINE, fpin) == NULL)
			break;
		if(fputs(line, stdout) == EOF)
			err_sys("fputs error to pipe");
	}
	if(pclose(fpin) == -1)
		err_sys("pclose error");
	putchar('\n');
	exit(0);
}
