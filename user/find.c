#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"
#include "kernel/fs.h"

#define BUF_SIZE 512
#define NULL 0

// 实现 strrchr 的功能
char *my_strrchr(const char *s, int c)
{
    char *last = NULL;
    while (*s)
    {
        if (*s == c)
            last = (char *)s;
        s++;
    }
    return last;
}

int match(const char *path, const char *name)
{
    const char *p = my_strrchr(path, '/');
    p = (p == NULL) ? path : p + 1;
    return strcmp(p, name) == 0;
}

void find(const char *path, const char *name)
{
    char buf[BUF_SIZE], *p;
    int fd;
    struct dirent de;
    struct stat st;

    if ((fd = open(path, 0)) < 0)
    {
        fprintf(2, "find: cannot open %s\n", path);
        return;
    }

    if (fstat(fd, &st) < 0)
    {
        fprintf(2, "find: cannot stat %s\n", path);
        close(fd);
        return;
    }

    if (st.type == T_FILE)
    {
        if (match(path, name))
        {
            printf("%s\n", path);
        }
        close(fd);
        return;
    }

    if (st.type == T_DIR)
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

            memmove(p, de.name, DIRSIZ);
            p[DIRSIZ] = 0;
            find(buf, name);
        }
    }
    close(fd);
}

int main(int argc, char *argv[])
{
    if (argc < 3)
    {
        fprintf(2, "usage: find <path> <name>\n");
        exit(1);
    }
    find(argv[1], argv[2]);
    exit(0);
}

