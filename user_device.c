#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>


int main()
{
    int fd = open("chardev", O_RDWR);
    char buf[255] = {0};
    if(fd < 0)
    {
        printf("error opening file\n");
    }
    int num = read(fd, buf, 255);
    if(num < 0)
    {
        printf("error in reading file");
    }
    else if(num == 0)
    {
        printf("no data\n");
    }
    else
    {
        printf("data %d string %s \n", num, buf);
    }
    
    close(fd);
    return 0;
}
