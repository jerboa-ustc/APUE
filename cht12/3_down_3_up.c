#include <stdio.h>
#include <pthread.h>
#include <unistd.h>
#include "apue.h"



pthread_mutex_t mutex_lock;
pthread_mutexattr_t attr;
pthread_cond_t cond_lock;
int cond_counter = 0;

void down(char*);
void up(char *);

/*
	P / V 操作
*/
void lock_init(void)
{
	pthread_mutex_init(&mutex_lock, NULL);
	pthread_mutexattr_init(&attr);
	pthread_mutexattr_settype(&attr, PTHREAD_MUTEX_RECURSIVE);
	pthread_cond_init(&cond_lock, NULL);
	cond_counter = 1;
}

void down(char *buf)
{
	printf("DOWN: %s\n", buf);
	pthread_mutex_lock(&mutex_lock);
	while(cond_counter == 0)		/*其实不是很明白为什么APUE要在这种情形下，才使用wait*/
		pthread_cond_wait(&cond_lock, &mutex_lock);		/*进入阻塞，等待signal激活*/
	
	cond_counter--;					/*这里可安全的操作counter，因为：1-若从wait中被唤醒，signal已经个 mutex上锁
																	 2-若不执行wait，本分支刚开始已经把mutex上锁*/
	pthread_mutex_unlock(&mutex_lock);
}
void up(char *buf)
{
	pthread_mutex_lock(&mutex_lock);

	cond_counter++;
	pthread_mutex_unlock(&mutex_lock);
	printf("UP: %s\n", buf);
	pthread_cond_signal(&cond_lock);
}
////////////////////////////////
void down2(char * buf)
{
	printf("DOWN: %s\n", buf);
	pthread_mutex_lock(&mutex_lock);
//	pthread_mutex_unlock(&mutex_lock);
}
void up2(char * buf)
{
//	pthread_mutex_lock(&mutex_lock);
	pthread_mutex_unlock(&mutex_lock);
	printf("UP: %s\n", buf);
}
////////////////////////////////
/*
	次线程只有一个任务就是：申请锁
*/
void *thread_routine (void *arg)
{
    int status;
	
	down("proc_0 | thread_1");
	printf("proc_0 | thread_1: lock got\n");
	sleep(5);
	up("proc_0 | thread_1");
	printf("proc_0 | thread_1: unlock\n");
	return NULL;
	//pthread_exit(NULL);
}

int main (int argc, char *argv[])
{
    pthread_t thread_1_to_fork;
	pid_t proc_1_to_fork;
    int status;
	lock_init();
/*	
#if defined(BSD) || defined(MACOS)
	printf("pthread_atfork is unsupported\n");
#else
	if((err = pthread_atfork(prepare, parent, child)) != 0)
		err_exit(err, "cant install fork handlers");
*/
	status = pthread_create(&thread_1_to_fork, NULL, thread_routine, 0);
	if(status != 0)
		err_exit(status, "cant create new thread: thread_1");
	
	/*
		now, we got thread_1 with a lock, and thread_0 will go on to fork a new process.
	*/
//	sleep(2);
	if((proc_1_to_fork = fork()) < 0)
		err_sys("fork error");
	else if(proc_1_to_fork == 0)	//子进程，申请锁，将会发现已经上锁，而且无法解开
	{	
		down("proc_1 | thread_0");
		printf("proc_1 | thread_0: lock got\n");
		up("proc_1 | thread_0");
		printf("proc_1 | thread_0: unlock\n");
		
	}
	else
	{	
		down("proc_0 | thread_0");
		printf("proc_0 | thread_0: lock got\n");
		up("proc_0 | thread_0");
		printf("proc_0 | thread_0: unlock\n");
		
	}
	
    pthread_exit(NULL);
}
