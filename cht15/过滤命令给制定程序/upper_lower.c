/*************************************************************************
	> File Name: upper_lower.c
	> Author: jerboa
	> Mail: a.jerboa@gmail.com
	> Created Time: 2014年05月19日 星期一 10时05分32秒
 ************************************************************************/

#include <stdio.h>
#include <ctype.h>
#include "apue.h"

/*
 *	过滤程序的标准输入来自shell输入，标准输出到管道中，父进程从管道接收并输出到shell。
 */

int main(void)
{
	int c;
	while((c = getchar()) != EOF)
	{
		if(isupper(c))
			c = tolower(c);
		if((putchar(c) == EOF))
			err_sys("output error");
		if(c == '\n')
			fflush(stdout);
	}
	exit(0);
}
