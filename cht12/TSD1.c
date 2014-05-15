/*************************************************************************
	> File Name: TSD1.c
	> Author: jerboa
	> Mail: a.jerboa@gmail.com
	> Created Time: 2014年05月13日 星期二 10时27分47秒
 ************************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>

typedef struct private_tag{
	pthread_t	thread_id;
	char *string;
}private_t;

pthread_key_t	identity_key0;
pthread_mutex_t	identity_key0_mutex = PTHREAD_MUTEX_INITIALIZER;
int				identity_key0_counter = 0;

void identity_key0_destructor(void *);
void *identity_key0_get(void);
void *identity_key0_make(void);
void *thread_routine(void *arg);
void pr_private_data(void *);

void identity_key0_destructor(void *addr)
{
	private_t *private = (private_t *)addr;
	int status;

	printf("%s: 0x%x exiting...\n", private->string, private->thread_id);
	pr_private_data(addr);
//	printf("sdfhsohgosghsog");
	free(private);
	status = pthread_mutex_lock(&identity_key0_mutex);
	if(status != 0)
		perror("thread_mutex_lock() error");

	identity_key0_counter--;

	if(identity_key0_counter <= 0)
	{
		status = pthread_key_delete(identity_key0);
		if(status != 0)
			perror("pthread_key_delete() error");
		printf("key: identity_key0 deleted...\n");
	}

	status = pthread_mutex_unlock(&identity_key0_mutex);
}

void *identity_key0_get(void)
{
	void *addr;
	int status;

	addr = pthread_getspecific(identity_key0);
	if(addr == NULL)
	{
		addr = malloc(sizeof(private_t));
		if(addr == NULL)
			perror("heap: malloc() error");
		status = pthread_setspecific(identity_key0, (void *)addr);
		if(status != 0)
			perror("pthread_setspecific() error");
	}

	return addr;
}

void * identity_key0_make(void)
{
	int status;
	status = pthread_key_create(&identity_key0, identity_key0_destructor);
	if(status != 0)
		perror("pthread_key_careate() error");
	identity_key0_counter = 3;		/*3 threads to go*/

	return (void*)(status);
}

void *thread_routine(void *arg)
{
	private_t *addr;

	addr = (private_t *)identity_key0_get();
	addr->thread_id = pthread_self();
	addr->string = (char *)arg;
	printf("%s: 0x%x starting...\n",addr->string,  addr->thread_id);
	sleep(2);
	return NULL;
}

void pr_private_data(void *addr)
{
	private_t *private = (private_t *)addr;
	printf("private data: at %p, thread_id = 0x%x | string = %s\n", private, private->thread_id, private->string);
}

void main(int argc, char *argv[])
{
	pthread_t thread_1, thread_2;
	private_t *addr;
	int status;

	//printf("%s: 0x%x starting...\n", "thread_0", pthread_self());
	status = (int)identity_key0_make();
	if(status != 0)
		perror("cant create identity_key0");
	addr = (private_t *)identity_key0_get();
	addr->thread_id = pthread_self();
	addr->string = "thread_0";
	printf("%s :0x%x starting...\n", addr->string, addr->thread_id);
	status = pthread_create(&thread_1, NULL, thread_routine, "thread_1");
	if(status != 0)
		perror("pthread_create() error");
	status = pthread_create(&thread_2, NULL, thread_routine, "thread_2");
	if(status != 0)
		perror("pthread_create() error");

	pthread_exit(NULL);
}
