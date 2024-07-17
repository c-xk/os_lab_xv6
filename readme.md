# xv6实验报告

---

## 环境搭建

1. 使用gnome-boxes安装Ubuntu20.04 LTS![](/home/shinka/.config/marktext/images/2024-07-17-17-03-51-image.png)

2. 运行```sudo apt-get install git build-essential gdb-multiarch qemu-system-misc gcc-riscv64-linux-gnu binutils-riscv64-linux-gnu```安装需要的包

3. 运行```qemu-system-riscv64 --version```和```riscv64-linux-gnu-gcc --version```检测环境是否安装成功![](/home/shinka/.config/marktext/images/2024-07-17-17-08-36-image.png)![](/home/shinka/.config/marktext/images/2024-07-17-17-09-04-image.png)

---

## Lab 1: Xv6 and Unix utilities

### Boot xv6

#### 实验目的

在qemu虚拟机中运行xv6环境

#### 实验步骤

1. 从远程仓库中检出util分支
   
   ```shell
   $ git clone git://g.csail.mit.edu/xv6-labs-2021
   Cloning into 'xv6-labs-2021'...
   ...
   $ cd xv6-labs-2021
   $ git checkout util
   Branch 'util' set up to track remote branch 'util' from 'origin'.
   Switched to a new branch 'util'
   ```

2. 试运行```make qemu```
   
   ![](/home/shinka/.config/marktext/images/2024-07-17-17-28-54-image.png)

3. 测试```ls```命令
   
   ![](/home/shinka/.config/marktext/images/2024-07-17-17-30-05-image.png)

#### 实验中遇到的问题和解决方法

一开始尝试在arch（本机）上直接运行，但是2021年的课程文件在如今（2024）已经过时了，在滚动更新的arch中无法运行，因此选择在Ubuntu虚拟机运行

#### 实验心得

学习了xv6的启动和gcc的基本调试方法

### sleep

#### 实验目的

1. 在xv6操作系统上实现sleep

2. 熟悉调用系统内核的方法

3. 熟悉在xv6上进行c语言编程与调试

#### 实验步骤

1. 引用头文件
   
   - "kernel/types.h"
     
     基本类型的定义
   
   - "kernel/stat.h"
     
     文件系统结构的定义
   
   - "user/user.h"
     
     用户调用系统内核的接口

2. main函数
   
   ```c
   int main(int argc, char *argv[])
   {
     if (argc < 2)
     {
       fprintf(2, "need a param");
       exit(1);
     }
     int i = atoi(argv[1]);
     sleep(i);
     exit(0);
   }
   ```
   
   argc是传入参数的数量，argv是具体参数。参数至少有1个，第0号参数指向程序自身。
   
   sleep函数是用户调用系统sleep的接口。
   
   用过autoi将参数转为int型

3. 测试![](/home/shinka/.config/marktext/images/2024-07-17-18-19-45-image.png)

#### 实验中遇到的问题和解决方法

在未阅读内核源码时，无法得知sleep的调用接口，需要我们熟悉系统各种接口的使用方法。

#### 实验心得

1. 本实验初步揭示了Unix-like系统的内核结构。用户不是直接访问内核，而是通过调用系统提供的接口来对内核进行访问。

2. 本实验也说明了在c语言中系统向程序传递参数的方式

### pingpong

#### 实验目的

1. 学习在Unix-like系统下创建子进程的方式

2. 学习父进程与子进程通过管道通信的方式

3. 学习基本的进程同步操作

#### 实验步骤

1. 使用`pipe()`函数创建管道
   
   ```c
   int parent_fd[2];
   pipe(parent_fd);
   ```
   
   `pipe(p)` 函数会在 `int *p` 指针指向的地址处生成两个文件描述符。`p[0]` 是读端的文件描述符，`p[1]` 是写端的文件描述符。持有这些文件描述符的进程可以对管道进行读写操作。

2. 使用`fork()`创建子进程
   
   ```c
     if (fork() == 0)
     {
       ...
     }
     else if(fork()>0)
     {
       ...
     }
   ```
   
   当 `fork()` 被执行时，系统会自动生成一个子进程。在父进程中，`fork()` 的返回值是子进程的 ID，通常大于 0；在子进程中，`fork()` 的返回值为 0；如果进程创建失败，`fork()` 将返回 -1。

3. 使用`wait()`实现进程同步
   
   对于父进程而言，`wait(&status)` 函数会阻塞父进程直到某一个子进程退出，并返回该子进程的 ID。如果没有子进程在运行，`wait` 函数将返回 -1 并设置 `errno`。`status` 将包含子进程的退出状态。

4. 核心代码
   
   ```c
   if (fork() == 0) //子进程
    {
    char buf[10];
    read(parent_fd[0], buf, sizeof buf);
    int id = getpid();
    printf("%d: received %s\n", id, buf);
    write(child_fd[1], "pong", 4);
    close(child_fd[0]);
    close(child_fd[1]);
    }
    else //父进程
    {
    char buf[10];
    int id = getpid();
    write(parent_fd[1], "ping", 4);  
    close(parent_fd[1]);//及时关闭写端文件描述符
    int status;
    wait(&status); //进程同步，等待子进程结束
    read(child_fd[0], buf, sizeof buf);
    printf("%d: received %s\n", id, buf);
    close(child_fd[0]);
    }
   ```

5. 测试结果![](/home/shinka/.config/marktext/images/2024-07-17-18-35-37-image.png)

#### 实验中遇到的问题和解决方法

1. 在一开始对创建进程与管道没有任何概念，通过阅读教科书学习了创建子进程以及管道的使用方法。

2. 实验过程中忽视了对写端文件描述符的关闭，导致读进程被阻塞。在实践中及时关闭管道的文件描述符很重要。

3. 实验过程中忽视了进程之间的同步。通过wait()函数，可以使进程之间的通信在恰当的时候进行。

#### 实验心得

1. 父子进程的概念是一个难点，每fork一次都会新建一个子进程，而通过fork的返回值可以判断当前处在父进程还是子进程，从而在不同分支中执行不同的语句。

2. 管道的理解是另一个难点。创建一个管道类似于创建了一个临时文件，创建它的进程拥有指向它的文件描述符。当子进程被创建时，文件描述符也会被复制，即指向管道的文件描述符多了一份。在写端的文件描述符没有完全关闭时，读操作将被阻塞。当指向管道的所有文件描述符都被关闭时，管道被释放。为了使管道正常被释放，所有进程都需要正确地关闭文件描述符。

3. 使用wait可以实现基本进程同步。

### primes

#### 实验目的

1. 进一步掌握子进程的创建与进程间的通信

2. 实现基本的并发操作

#### 实验步骤

1. 埃拉托色尼筛法
   
   要找出2到n(n≥2)的所有质数：
   
   - 从第一个数2开始，在剩余的数中去除它的所有倍数
   
   - 从小到大检查列表里的每一个数，每检查到一个数时就去除它的所有倍数
   
   - 所有被检查到的数都是质数

2. 并发执行过程
   
   - 父进程初始化2到35的升序序列，写入管道，将序列传给第一个子进程。
   
   - 子进程通过管道从它的父进程读取序列，并且将序列中的第一个数标记为质数，同时创建它的管道与子进程。
   
   - 子进程在剩余序列中去除第一个数（检查的某个质数）的倍数，将得到的结果写入管道，传给它的子进程。
   
   - 所有的进程像一条链条一样串联起来，形成一颗进程树（链），链条的每个环境都能检出一个质数![image-20230711142842449.png](https://github.com/Iuriak/OS-Xv6-Lab-2023/blob/main/assets/image-20230711142842449.png?raw=true)

3. 核心代码
   
   ```c
     pipe(fd1);
     if (fork() > 0)
     {
       close(fd1[0]);
       for (int i = 2; i <= 35; i++)
       {
         write(fd1[1], &i, sizeof(i));
       }
       close(fd1[1]);
       int status;
       wait(&status); // 等待子进程结束
     }
     else
     {
       p1 = fd1;
       p2 = fd2;
       while (1)
   //子进程在交换了管道文件描述符后回到这里，执行一轮循环。
   //此时它将创建它的子进程，成为一个父进程
       {
         pipe(p2); 
         close(p1[1]);
         if (read(p1[0], &first, sizeof(first)))
           printf("prime %d\n", first);
         else
           break;
         if (fork() > 0)
       //父子进程在这里分界，子进程获得父进程在此之前的数据的拷贝
         {
           while (read(p1[0], &v, sizeof(v)))
           {
             if (v % first != 0)
               write(p2[1], &v, sizeof(v));
           }
           close(p1[0]);
           close(p2[1]);
           int status;
           wait(&status); // 等待子进程结束
           break;
         }
         else
         {
           close(p1[0]);
           int *tmp = p1;//交换新旧管道文件描述符
           p1 = p2;
           p2 = tmp;
         }
       }
     }
   ```

4. 测试结果
   
   ![](/home/shinka/.config/marktext/images/2024-07-17-19-32-49-image.png)

#### 实验中遇到的问题和解决方法

1. 这个实验相比pingpong难度有了较大的提高，不仅需要实现多进程的并发，而且要在不同进程间进行通信，要完成该实验需要对fork与pipe有较为深入的理解。我对它们的理解见pingpong实验。

2. 在实验中，为了防止某一子进程变成孤儿进程，需要确保子进程退出后才能退出父进程，通过wait实现

3. 在实验中，需要及时关闭管道文件描述符，以保证管道被正常释放，以免占用过多空间。

#### 实验心得

通过该实验，我初步掌握了并发编程，同时对进程之间的通信与同步机制也有了更深的理解。详见pingpong实验。

### find

#### 实验目的

1. 熟悉Unix-like系统的文件系统组织结构

2. 熟悉对文件系统的读取和查找

#### 实验步骤

1. `stat`结构
   
   `stat`中存储了文件的元数据，不管是文件还是文件夹都是如此，通过`stat.type`判断文件类型。

2. `dirent`结构
   
   `dirent`是文件夹拥有的目录结构。如果文件的类型是文件夹，`dirent`会记录该文件夹的目录

3. 核心代码
   
   ```c
   void find(const char *path, const char *name)
   {
       char buf[BUF_SIZE], *p;
       int fd;
       struct dirent de;
       struct stat st;
       if ((fd = open(path, 0)) < 0)//打开文件
       {
           fprintf(2, "find: cannot open %s\n", path);
           return;
       }
       if (fstat(fd, &st) < 0)//读取stat
       {
           fprintf(2, "find: cannot stat %s\n", path);
           close(fd);
           return;
       }
       if (st.type == T_FILE)//如果是文件进行匹配
       {
           if (match(path, name))//匹配函数，未展示
           {
               printf("%s\n", path);
           }
           close(fd);
           return;
       }
       if (st.type == T_DIR)//如果是文件夹，检查目录
       {
           if (strlen(path) + 1 + DIRSIZ + 1 > sizeof(buf))
           {
               printf("find: path too long\n");
               close(fd);
               return;
           }
           strcpy(buf, path);
           p = buf + strlen(buf);
           *p++ = '/';
           while (read(fd, &de, sizeof(de)) == sizeof(de))
           {
               if (de.inum == 0 || strcmp(de.name, ".") == 0 || strcmp(de.name, "..") == 0)
                   continue;
       //inum为0表示文件不存在或为空，'.'表示当前目录，'..'表示父目录
               memmove(p, de.name, DIRSIZ);
               p[DIRSIZ] = 0;
               find(buf, name);//递归查询
           }
       }
       close(fd);
   }
   ```

4. 测试结果
   
   ![](/home/shinka/.config/marktext/images/2024-07-17-20-32-44-image.png)

#### 实验中遇到的问题和解决方法

1. 没有找到C标准库中的`strrchr`函数，需要自己实现路径匹配中查找`/`的操作
2. 一次查询操作只能在当前目录下进行匹配，匹配不完全，需要进行递归操作

#### 实验心得

通过本实验，我初步掌握了Unix-like系统的文件系统结构，学习了对文件夹的目录读取和递归遍历。在该文件系统下，文件与文件夹用同一个结构存储，而目录则单独使用一个结构来存储。

### xargs

#### 实验目的

1. 了解Unix-like系统下xargs命令的使用方法

2. 学习在一个程序下进入另一个程序的方法

3. 进一步学习并发执行，使不同的进程执行不同的子程序

#### 实验步骤

1. 核心代码
   
   ```c
   int main(int argc, char *argv[])
   {
       char buf[128];
       char *new_argv[MAXARG];
       // 检查参数个数是否超过了限制
       if (argc > MAXARG)
       ...
       // 把argv的内容拷贝到new_argv
       for (int i = 1; i < argc; i++)
           new_argv[i - 1] = argv[i];
       // 循环读取用户的输入
       while (gets(buf, sizeof(buf)))
       {
           int buf_len = strlen(buf);
           if (buf_len < 1)
               break;
           buf[buf_len - 1] = '\0'; 
      // 将读取到的字符串中的换行符替换为字符串结束符
           int argv_len = argc - 1;
           // 拆分输入字符串并填充到new_argv
           char *p = buf;
           while (*p)
           {
               // 跳过连续的空格字符
               while (*p && *p == ' ')
                   p++;
               if (*p)
               {
                   // 检查参数个数是否超过了限制
                   if (argv_len >= MAXARG - 1)
                   {
                       printf("Too many arguments\n");
                       exit(1);
                   }
                   new_argv[argv_len++] = p;
                   // 跳过当前参数剩余字符
                   while (*p && *p != ' ')
                       p++;
                   // 添加字符串结束符
                   if (*p)
                       *p++ = '\0';
               }
           }
           // 终止字符串
           new_argv[argv_len] = 0;
           // 创建子进程并执行新程序
           if (fork() == 0)
           {
               exec(new_argv[0], new_argv);
               exit(0);  // 如果exec失败，子进程需要退出
           }
           //父进程进入下一次循环，与子进程同时进行，实现并发
       }
       while (wait(0) > 0)
           ;        //父进程等待所有子进程退出后再退出
       exit(0);
   }
   ```

2. 测试结果![](/home/shinka/.config/marktext/images/2024-07-17-20-40-41-image.png)

#### 实验中遇到的问题和解决方法

实验中，对xargs命令不熟悉，不了解它的作用与运行机制。通过学习linux的xargs命令后对它有了基本的理解

#### 实验心得

1. 了解了xargs的使用方法与作用，对Unix-like系统的使用有了更深的理解

2. 学习了多进程的进阶使用方法，子进程不一定要与父进程执行同一个程序，子进程也可以是另一个程序的入口。通过这种方法，不仅可以极大提高并发执行效率，而且子进程之间彼此互不干涉，只要父进程不崩溃，子进程的故障不会影响全局。
