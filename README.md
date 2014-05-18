APUE
====

##自己APUE笔试的测试例程(各种修改，有完整的，也有不完整的)

### APUE中lib库的编译和使用：
1. 到 www.apuebook.com 下载源码(无论APUE是第几版，代码都是一样的，站点会不断更新。本菜的是第三版)
  tar 解包， cd apue.2e
2. 编辑 Make.defines.linux ，修改变量 WKDIR ，指向你放置 apue 源码的位置，我的是 /home/huangz/code/apue.2e ，所以设置为 WKDIR=/home/huangz/code/apue.2e
3. 编辑 include/apue.h ，增加一个常量 ARG_MAX ： #defines ARG_MAX 4096 ， threadctl/getenv1.c 和 threadctl/getenv3.c 4. 4. 4. 要用到这个常量； 4096 只是参考值，可以视情况按需修改。
5. 编辑 threadctl/getenv1.c 增加一行： #include "apue.h"
6. 编辑 threadctl/getenv3.c 增加一行： #include "apue.h"
7. 编辑 threads/badexit2.c ，修改第 31 行，将 pthread_self() 的返回值转换为 int 类型： printf("thread 2: ID is %d\n", (int)pthread_self()); 。
8. 编辑 std/linux.mk ，将两个 nawk 替换为 gawk 。
9. make
10. 将相应的文件复制到库位置里： sudo cp include/apue.h /usr/include ，以及 sudo cp lib/libapue.a /usr/lib 。
  至此，所有步骤执行完成了。

### GCC使用APUE-lib库：
1. gcc hiho.c -o hiho -lapue (-lpthread)

### 本菜编译lib时遇到的错误提示
1. apue源码make：/usr/include/bits/timex.h:31:7: 错误：expected ‘:’, ‘,’, ‘;’, ‘}’ or ‘__attribute__’ be
2. 解决：http://blog.csdn.net/muais/article/details/6885708
