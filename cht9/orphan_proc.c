/*************************************************************************
	> File Name: orphan_proc.c
	> Author: jerboa
	> Mail: a.jerboa@gmail.com
	> Created Time: 2014年05月10日 星期六 10时46分57秒
 ************************************************************************/

#include <stdio.h>
#include "apue.h"
#include <errno.h>

static void sig_hug(int signo)
{
	printf("SIGHUP received, pid = %d\n", getpid());
}

static void pr_ids(char *name)
{
	if(getpgrp() == tcgetpgrp(STDIN_FILENO))
		printf("前台进程%s pid = %d, ppid = %d, pgrp = %d, tpgrd = %d\n", name, getpid(), getppid(), getpgrp(), tcgetpgrp(STDIN_FILENO));
	else
		printf("后台进程%s pid = %d, ppid = %d, pgrp = %d, tpgrd = %d\n", name ,getpid(), getppid(), getpgrp(), tcgetpgrp(STDIN_FILENO));
}

int main(void)
{
	char c;
	pid_t pid;
	pr_ids("parent");
	if((pid = fork()) < 0)
		err_sys("fork error");
	else if(pid > 0)
	{
//		sleep(10);
		exit(0);
	}
	else
	{
		pr_ids("child");
		signal(SIGTSTP,sig_hug);
		kill(getpid(), SIGTSTP);
		pr_ids("child");
		if(read(STDIN_FILENO, &c, 1) != 1)
			printf("read error from controlling TTY, error = %d\n", errno);
	}
}
