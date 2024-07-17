#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"

int main(int argc, char *argv[])
{
  int fd1[2];
  int fd2[2];
  int *p1, *p2;
  int first, v;
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
    wait(&status); // 修改：等待子进程结束
  }
  else
  {
    p1 = fd1;
    p2 = fd2;
    while (1)
    {
      pipe(p2); // 修改：在每次循环中创建新的管道
      close(p1[1]); // 修改：关闭父进程写端
      if (read(p1[0], &first, sizeof(first)))
        printf("prime %d\n", first);
      else
        break;
      
      if (fork() > 0)
      {
        while (read(p1[0], &v, sizeof(v)))
        {
          if (v % first != 0)
            write(p2[1], &v, sizeof(v));
        }
        close(p1[0]);
        close(p2[1]);
        int status;
        wait(&status); // 修改：确保等待子进程结束
        break;
      }
      else
      {
        close(p1[0]);
        int *tmp = p1;
        p1 = p2;
        p2 = tmp;
      }
    }
  }
  
  exit(0);
}

