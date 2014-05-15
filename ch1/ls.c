/*************************************************************************
	> File Name: ls.c
	> Author: jerboa
	> Mail: a.jerboa@gmail.com
	> Created Time: 2014年05月05日 星期一 09时48分54秒
 ************************************************************************/

#include <stdio.h>
#include <dirent.h>

int main(int argc, char *argv[])
{
	DIR *dp;
	struct dirent * dirp;

	if(argc != 2)
		err_quit("usage: ls directory name");
	if((dp = opendir(argv[1])) == NULL)
		err_sys("can't open %s", argv[1]);
	while((dirp = readdir(dp)) != NULL)
		printf("%s\n", dirp->d_name);

	closedir(dp);
	exit(0);
}
