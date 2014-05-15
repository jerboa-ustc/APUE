#include <stdio.h>
#include <pthread.h>
#include <unistd.h>
#include "apue.h"
#include <errno.h>
#include <sys/types.h>

pthread_mutex_t mutex_lock;
pthread_mutexattr_t attr;
pthread_cond_t cond_lock;
struct timeval now;
struct timespec outtime;
int cond_counter = 0;

void down(char*);
void up(char *);
void fork_prepare(void);
void fork_parent(void);
void fork_child(void);

void fork_prepare(void)			/*参见APUE pthread_atfork章节，实际上是无法也不需要对条件变量cond_lock做出清理工作的。只需要对锁进行清理，如果APP使用的条件变量和锁是分开的（本例），那么APP需要清理锁；如果锁在内置的条件变量数据结构中，那么就需要清理条件变量*/
{
	int status;
	status = pthread_mutex_lock(&mutex_lock);
	if(status != 0)
		err_exit(status, "fork prepare handler lock error");
	printf("preparing locks...\n");
}

void fork_parent(void)
{
	int status;
	status = pthread_mutex_unlock(&mutex_lock);
	if(status != 0)
		err_exit(status, "fork parent handler unlock error");
	printf("parent unlocks...\n");
}

void fork_child(void)
{
	int status;
	status = pthread_mutex_unlock(&mutex_lock);
	if(status != 0)
		err_exit(status, "fork child handler unlock error");
	printf("child unlcoks...\n");
}

/*
	P / V 操作
*/
void lock_init(void)
{
	pthread_mutexattr_init(&attr);
	pthread_mutexattr_settype(&attr, PTHREAD_MUTEX_RECURSIVE);
	pthread_mutex_init(&mutex_lock, &attr);
	pthread_cond_init(&cond_lock, NULL);
	cond_counter = 1;
}

void down(char *buf)
{
	int status;
	printf("DOWN: %s\n", buf);
	pthread_mutex_lock(&mutex_lock);
	while(cond_counter == 0)		/*其实不是很明白为什么APUE要在这种情形下，才使用wait*/
	{
		gettimeofday(&now ,NULL);
		outtime.tv_sec = now.tv_sec + 10;
		outtime.tv_nsec = now.tv_usec * 1000;
		status = pthread_cond_timedwait(&cond_lock, &mutex_lock, &outtime);		/*进入阻塞，等待signal激活*/
		if(status == ETIMEDOUT)
		{
			printf("%s condition wait TIMEOUT\n", buf);		/*不用err_sys,因为参数只能是errno*/
		//	break;
		}
	}
	cond_counter--;					/*这里可安全的操作counter，因为：1-若从wait中被唤醒，signal已经个 mutex上锁
																	 2-若不执行wait，本分支刚开始已经把mutex上锁*/
	pthread_mutex_unlock(&mutex_lock);
}
void up(char *buf)
{
	pthread_mutex_lock(&mutex_lock);

	printf("UP: %s\n", buf);
	if(cond_counter == 0)
		pthread_cond_signal(&cond_lock);
	cond_counter++;
	pthread_mutex_unlock(&mutex_lock);
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
	
	down((char *)arg);
	printf("%s: lock got\n", (char *)arg);
	sleep(5);
	up((char *)arg);
	printf("%s: unlock\n", (char *)arg);
	return NULL;
	//pthread_exit(NULL);
}

int main (int argc, char *argv[])
{
    pthread_t thread_1_to_fork;
	pid_t proc_1_to_fork;
    int status;
	void *arg;
	lock_init();
	
#if defined(BSD) || defined(MACOS)
	printf("pthread_atfork is unsupported\n");
#else
	if((status = pthread_atfork(fork_prepare, fork_parent, fork_child)) != 0)
		err_exit(status, "cant install fork handlers");

	arg = (void *)("proc_0 | thread_1");
	status = pthread_create(&thread_1_to_fork, NULL, thread_routine, arg);
	if(status != 0)
		err_exit(status, "cant create new thread: thread_1");
	
	/*
		now, we got thread_1 with a lock, and thread_0 will go on to fork a new process.
	*/
	if((proc_1_to_fork = fork()) < 0)
		err_sys("fork error");
	else if(proc_1_to_fork == 0)	//子进程，申请锁，将会发现已经上锁，而且无法解开
	{	
		arg = (void *)("proc_1 | thread_0");
		thread_routine(arg);
	}
	else
	{	
		down("proc_0 | thread_0");
		printf("proc_0 | thread_0: lock got\n");
		up("proc_0 | thread_0");
		printf("proc_0 | thread_0: unlock\n");
		
	}
#endif

    pthread_exit(NULL);
}
