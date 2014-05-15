/*************************************************************************
	> File Name: apue_tree.c
	> Author: jerboa
	> Mail: a.jerboa@gmail.com
	> Created Time: 2014年05月06日 星期二 14时04分12秒
 ************************************************************************/

#include <stdio.h>
#include "apue.h"
#include <dirent.h>
#include <limits.h>

typedef int MyFunc(const char*, const struct stat*, int, int);
static MyFunc myfunc;
static int MyFtw(char *, MyFunc*);
static int dopath(MyFunc*);
static void print_fname(const char *, int);
static long nreg, ndir, nblk, nchr, nfifo, nslink, nsock, ntot;
static char *fullpath;
static int depth;

#define FTW_F 1
#define FTW_D 2
#define FTW_DNR 3
#define FTW_NS 4

#ifdef PATH_MAX
	const int PATH_LEN = PATH_MAX;
#else
	const int PATH_LEN = 1024;
#endif

#define END(s) ((char *)(s) + strlen(s))

int main(int argc, char* argv[])
{
	int ret;
	if(argc != 2)
		err_quit("usage: ftw <starting-pathneme");
	
	ret = MyFtw(argv[1], myfunc);	/* start dfs dirs*/

	ntot = nreg + ndir + nblk + nchr + nfifo + nslink + nsock + ntot;

	if(ntot == 0)
		ntot = 1;
	printf("regular files	= %7ld, %5.2f %%\n", nreg, nreg*100.0/ntot);
	printf("directories		= %7ld, %5.2f %%\n", ndir, ndir*100.0/ntot);
	printf("block special	= %7ld, %5.2f %%\n", nblk, nblk*100.0/ntot);
	printf("char special	= %7ld, %5.2f %%\n", nchr, nchr*100.0/ntot);
	printf("FIFOs			= %7ld, %5.2f %%\n", nfifo, nfifo*100.0/ntot);
	printf("symbolic links	= %7ld, %5.2f %%\n", nslink, nslink*100.0/ntot);
	printf("sockets			= %7ld, %5.2f %%\n", nsock, nsock*100.0/ntot);

	exit(0);
}

static int MyFtw(char *pathname, MyFunc *func)
{
	int ret;

	fullpath = malloc(PATH_LEN);

	strncpy(fullpath, pathname, PATH_LEN);
	fullpath[PATH_LEN-1] = '\0';

	ret = dopath(func);
	free(fullpath);
	return(ret);
}

static int dopath(MyFunc* func)
{
	struct stat statbuf;
	struct dirent *dirp;
	DIR *dp;
	int ret;
	char *ptr;

	if(lstat(fullpath, &statbuf) < 0)	/*stat error*/
		return (func(fullpath, &statbuf, FTW_NS, depth));
	if(S_ISDIR(statbuf.st_mode) == 0)	/*not a dir*/
		return (func(fullpath, &statbuf, FTW_F, depth));

	/*now, we got a real dir*/
	if((ret = func(fullpath, &statbuf, FTW_D, depth)) != 0)	/*unknown counter error*/
		return ret;

	ptr = END(fullpath);
	*ptr++ = '/';
	*ptr = '\0';

	if((dp = opendir(fullpath)) == NULL)	/*cant read a dir*/
		return(func(fullpath, &statbuf, FTW_DNR, depth));
	
	depth++;

	while((dirp = readdir(dp)) != NULL)
	{
		if( strcmp(dirp->d_name, ".")==0 || strcmp(dirp->d_name, "..")==0 )
			continue;

		strcpy(ptr, dirp->d_name);	/*dfs, append a sub dir name*/
		
		if((ret = dopath(func)) != 0)	/*不明白为什么会有！=0 的情况发生，这里直接dppath(func)也可以*/
			break;
	}
	ptr[-1] = 0;
	
	depth--;

	if(closedir(dp) < 0)
		err_ret("cant close this dir %s", fullpath);

	return(ret);
}

static int myfunc(const char* pathname, const struct stat *statptr, int type, int depth)
{
	switch (type)
	{
		case FTW_F:
			switch (statptr->st_mode & S_IFMT)
			{
				case S_IFREG:	nreg++;		break;
				case S_IFBLK:	nblk++;		break;
				case S_IFCHR:	nchr++;		break;
				case S_IFIFO:	nfifo++;	break;
				case S_IFLNK:	nslink++;	break;
				case S_IFSOCK:	nsock++;	break;
				case S_IFDIR:
								err_dump("directory should have type FTW_D, %s.", pathname);
			}
			break;
		case FTW_D:
			ndir++;
			break;
		case FTW_DNR:
			err_ret("cant read this dir %s", pathname);
			break;
		case FTW_NS:
			err_ret("stat error for %s", pathname);
			break;
		default:
			err_dump("unknown type %d for pathname %s", type, pathname);
	}
	
	print_fname(pathname, depth);
	return (0);
}

static void print_fname(const char *pathname, int depth)
{
	int i=0, dep=0;
	for(; i<depth*4 - 1; i++)
	{
		if(!(i & 3))
		{
			printf("|");
			if(i) dep++;
		}
		if(dep + 1 == depth)
			printf("-");
		else
			printf(" ");
	}
	printf(" %s\n", pathname);
}
