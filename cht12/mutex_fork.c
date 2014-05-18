#include "apue.h"
#include <pthread.h>
#include <errno.h>

pthread_mutex_t lock1 = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t lock2 = PTHREAD_MUTEX_INITIALIZER;

void
prepare(void)
{
    printf("preparing locks mutex @lock1 @lock2...\n");
    pthread_mutex_lock(&lock1);
    pthread_mutex_lock(&lock2);
}

void 
parent(void)
{
    printf("parent unlocking mutex @lock1 @lock2...\n");
    pthread_mutex_unlock(&lock1);
	pthread_mutex_unlock(&lock2);
}

void
child(void)
{
    printf("child unlocking mutex @lock1 @lock2...\n");
    pthread_mutex_unlock(&lock1);
	 pthread_mutex_unlock(&lock2);
}

void *
thr_fn(void *arg)
{
	int err;
	pid_t child_proc;
	printf("thread start...\n");	
	if((child_proc = fork()) < 0)
		err_quit("fork error");
	else if(child_proc == 0)
	{
		err = pthread_mutex_lock(&lock1);
		if(err == 0)
			printf("lock mutex @lock1 in child proc\n");
		else
			err_exit(err, "cant lock mutex @lock1 in child proc");
		err = pthread_mutex_unlock(&lock1);
		if(err == 0)
			printf("unlock mutex @lock1 in child proc\n");
		else
			err_exit(err, "cant unlock mutex @lock1 in child proc");
	}
	else
	{
		;
	}
    pthread_exit(0);
}

int
main(void)
{
    int        err;
    pid_t        pid;
    pthread_t    tid;
    
#if defined(BSD) || defined(MACOS)
    printf("pthread_atfork is unsupported\n");
#else
    if((err = pthread_atfork(prepare, parent, child)) != 0)
        err_exit(err, "cant install fork handlers");
	
	err = pthread_mutex_lock(&lock1);
	if(err == 0)
		printf("lock mutex @lock1 in father proc 1st-therad\n");
	else
		err_exit(err, "cant lock mutex");

    err = pthread_create(&tid, NULL, thr_fn, 0);
    if(err != 0)
        err_exit(err, "can't create thread");
	
	sleep(5);

	err = pthread_mutex_unlock(&lock1);
	if(err == 0)
		printf("unlock mutex @lock1 in father proc 1st-thread\n");
	else
		err_exit(err, "cant unlcok mutex");
#endif
    //exit(0);
	pthread_exit(0);
}
