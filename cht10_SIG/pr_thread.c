/*************************************************************************
	> File Name: pr_thread.c
	> Author: jerboa
	> Mail: a.jerboa@gmail.com
	> Created Time: 2014年05月11日 星期日 16时15分52秒
 ************************************************************************/

#include <stdio.h>
#include "apue.h"
#include <pthread.h>

pthread_t newtid;

void printids(const char *s)
{
	pid_t pid;
	pthread_t tid;
	
	pid = getpid();
	tid = pthread_self();
	printf("%s pid %u tid 0x%x\n", s, (unsigned int)pid, (unsigned int)tid);
}

void *thr_fn(void* arg)
{
	printids("new thread: ");
	return ((void*)0);
}

int main(void)
{
	int err;
	err = pthread_create(&newtid, NULL, thr_fn, NULL);
	if(err != 0)
		err_quit("cant create thread: %s\n", strerror(err));
	printids("main thread: ");
	sleep(1);
	exit(0);
}
