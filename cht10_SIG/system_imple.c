/*************************************************************************
	> File Name: system_imple.c
	> Author: jerboa
	> Mail: a.jerboa@gmail.com
	> Created Time: 2014年05月11日 星期日 13时36分58秒
 ************************************************************************/

#include <stdio.h>
#include <sys/wait.h>
#include <errno.h>
#include <signal.h>
#include <unistd.h>

typedef struct sigaction SIGACT;

int system(const char* cmdstring)
{
	pid_t pid;
	int status;
	SIGACT ignore, saveintr, savequit;
	sigset_t chldmask, savemask;

	if(cmdstring == NULL)
	{
		return 1;
	}

	ignore.sa_handler = SIG_IGN;	/*父进程中设置忽略2个信号：SIGINT-SIGQUIT，子进程中再修改*/
	sigepmtyset(&ignore.sa_mask);
	ignore.sa_flags = 0;
	if(sigaction(SIGINT, &ignore, &saveintr) < 0)
		return -1;
	if(sigaction(SIGQUIT, &ignore, &savequit) < 0)
		return -1;
	sigemptyset(&chldmask);			/*BLOCK SIGCHLD信号*/
	sigaddset(&chldmask, SIGCHLD);
	if(sigprocmask(SIG_BLOCK, &chldmask, &savemask) < 0)
		return -1;

	if((pid = fork()) < 0)
		status = -1;
	else if(pid == 0)				/*子进程中重设信号屏蔽，3个SIG屏蔽都去掉*/
	{
		sigaction(SIGINT, &saveintr, NULL);
		sigaction(SIGQUIT, &savequit, NULL);
		sigprocmask(SIG_SETMASK, &savemask, NULL);

		execl("/bin/bash", "bash", "-c", cmdstring, (char *)0);
		_exit(127);
	}
	else
	{
		while(waitpid(pid, &status, 0) < 0)
			if(errno != EINTR)		/*不是因为中断而出错*/
			{
				status = -1;
				break;
			}
	}

	if(sigaction(SIGINT, &saveintr, NULL) < 0)
		return -1;
	if(sigaction(SIGQUIT, &savequit, NULL) < 0)
		return -1;
	if(sigprocmask(SIG_SETMASK, &savemask, NULL) < 0)
		return -1;

	return status;
}

