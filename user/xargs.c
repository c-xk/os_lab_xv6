#include "kernel/types.h"
#include "kernel/stat.h"
#include "kernel/param.h"
#include "user/user.h"

int main(int argc, char *argv[])
{
    char buf[128];
    char *new_argv[MAXARG];

    // 检查参数个数是否超过了限制
    if (argc > MAXARG)
    {
        printf("Too many arguments\n");
        exit(1);
    }

    // 把argv的内容拷贝到new_argv
    for (int i = 1; i < argc; i++)
    {
        new_argv[i - 1] = argv[i];
    }

    // 循环读取用户的输入
    while (gets(buf, sizeof(buf)))
    {
        int buf_len = strlen(buf);

        if (buf_len < 1)
            break;

        buf[buf_len - 1] = '\0'; // 将读取到的字符串中的换行符替换为字符串结束符
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
    }

    while (wait(0) > 0)
        ;

    exit(0);
}
