/*************************************************************************
	> File Name: alarm_sleep.c
	> Author: jerboa
	> Mail: a.jerboa@gmail.com
	> Created Time: 2014年05月10日 星期六 16时33分56秒
 ************************************************************************/

#include <stdio.h>
#include "apue.h"
#include <setjmp.h>
#include <signal.h>
#include <unistd.h>

static jmp_buf env_alarm;
static void sig_alarm(int signo)
{
	longjmp(env_alarm, 1);
}

unsigned int sleep2(unsigned int nsecs)
{
	int counter = 1;
	if(signal(SIGALRM, sig_alarm) == SIG_ERR)
		return nsecs;
	if(setjmp(env_alarm) == 0)
	{
		printf("%d\n", counter++);
		alarm(nsecs);
		pause();
	}
	return (alarm(0));
}

int main(void)
{
	sleep2(2);

	return 0;
}
