/*************************************************************************
	> File Name: sync_thread.c
	> Author: jerboa
	> Mail: a.jerboa@gmail.com
	> Created Time: 2014年05月12日 星期一 11时29分59秒
 ************************************************************************/

#include <stdio.h>
#include "apue.h"
#include <pthread.h>
#include <time.h>

typedef struct to_info{
	void (*to_fn)(void *);		/*func*/
	void *to_arg;
	struct timespec to_wait;	/*time to wait*/
}TO_INFO;

pthread_mutexattr_t attr;
pthread_mutex_t mutex;

#define SECTONSEC	1000000000
#define USECTONSEC	1000

void *timeout_helper(void *arg)
{
	TO_INFO *tip;

	tip = (TO_INFO *)arg;
	nanosleep(&tip->to_wait, NULL);
	(*tip->to_fn)(tip->to_arg);

	return 0;
}

void timeout(const struct timespec *when, void (*func)(void *), void *arg)
{
	struct timespec now;
	struct timeval tv;
	TO_INFO *tip;
	int err;


	gettimeofday(&tv, NULL);
	now.tv_sec = tv.tv_sec;
	now.tv_nsec = tv.tv_usec * USECTONSEC;
	if((when->tv_sec > now.tv_sec) || (when->tv_sec == now.tv_sec && when->tv_nsec > now.tv_nsec))
	{
		tip = malloc(sizeof(TO_INFO));
		if(tip != NULL)
		{
			tip->to_fn = func;
			tip->to_arg = arg;
			tip->to_wait.tv_sec = when->tv_sec - now.tv_sec;
			if(when->tv_nsec >= now.tv_nsec)
			{
				tip->to_wait.tv_nsec = when->tv_nsec - now.tv_nsec;
			}
			else
			{
				tip->to_wait.tv_sec--;
				tip->to_wait.tv_nsec = SECTONSEC - now.tv_nsec + when->tv_nsec;
			}
			err = makethread(timeout_helper, (void *)tip);
			if(err ==0)
				return;
		}
	}
	(*func)(arg);
}

int makethread(void *(*fn)(void *), void *arg)
{
	int err;
	pthread_t tid;
	pthread_attr_t attr;

	err = pthread_attr_init(&attr);
	if(err != 0)
		return err;
	err = pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_DETACHED);
	if(err == 0)
		err = pthread_create(&tid, &attr, fn, arg);
	pthread_attr_destroy(&attr);

	return err;
}

void retry(void *arg)
{
	pthread_mutex_lock(&mutex);
	/*
	some thing to do here.
	*/
	pthread_mutex_unlock(&mutex);
}

int main(void)
{
	int err, condition, arg;
	struct timespec when;

	if((err = pthread_mutexattr_init(&attr)) != 0)
		err_exit(err, "pthread_mutexattr_init failed");
	if((err = pthread_mutexattr_settype(&attr, PTHREAD_MUTEX_RECURSIVE)) != 0)
		err_exit(err, "cant set recursive type");
	if((err = pthread_mutex_init(&mutex, &attr)) != 0)
		err_exit(err, "cant create recursive mutex");
	pthread_mutex_lock(&mutex);
	if(condition)
	{
		/*计算when变量，规划分离线程将要启动的时刻*/
		timeout(&when, retry, (void*)arg);
	}
	pthread_mutex_unlock(&mutex);

	exit(0);
}
