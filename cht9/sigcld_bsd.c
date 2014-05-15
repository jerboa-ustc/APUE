/*************************************************************************
	> File Name: sigcld_bsd.c
	> Author: jerboa
	> Mail: a.jerboa@gmail.com
	> Created Time: 2014年05月10日 星期六 15时36分34秒
 ************************************************************************/

#include <stdio.h>
#include "apue.h"
#include <sys/wait.h>

static void sig_cld(int);

int main()
{
	pid_t pid;
	if(signal(SIGCLD, sig_cld) == SIG_ERR)
		perror("signal error");
	if((pid = fork()) < 0)
		perror("fork error");
	else if(pid == 0)
	{
		printf("child proc %d\n", getpid());
//		sleep(2);
		_exit(0);
	}
	printf("father proc %d\n", getpid());
	pause();
	exit(0);
}

static void sig_cld(int signo)
{
	pid_t pid;
	int status;
	printf("SIGCLD received\n");
	if(signal(SIGCLD, sig_cld) == SIG_ERR)
		perror("signal error");
	if((pid = wait(&status)) < 0)
		perror("wait error");
	printf("child waitted = %d\n", pid);
}
