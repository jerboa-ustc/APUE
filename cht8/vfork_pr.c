/*************************************************************************
	> File Name: vfork_pr.c
	> Author: jerboa
	> Mail: a.jerboa@gmail.com
	> Created Time: 2014年05月07日 星期三 16时24分52秒
 ************************************************************************/

#include <stdio.h>
#include "apue.h"

int glob = 6;
int main(void)
{
	int var;
	pid_t pid;

	var = 88;
	printf("before vfork\n");
//	if((pid = vfork()) < 0)
	if((pid = fork()) < 0)
		err_sys("vfork error.");
	else if(pid == 0)
	{
		glob++;
		var++;
		printf("pid = %d, glob = %d, var = %d\n", getpid(), glob, var);
		exit(0);
	}
	else
	{
		printf("pid = %d, glob = %d, var = %d\n", getpid(), glob, var);
	}
//	printf("pid = %d, glob = %d, var = %d\n", getpid(), glob, var);
	exit(0);
}
