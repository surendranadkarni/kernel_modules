#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>

char *str = "Hello";
int main()
{
    int fd = open("chardev", O_RDWR);
    char buf[255] = {0};
    if(fd < 0)
    {
        printf("error opening file\n");
    }
    printf("check read\n");
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
     
    printf("check read\n");
    int bytes_written = write(fd, str, strlen(str));
    if(bytes_written < 0)
    {
        printf("error in writing to driver\n");
    }
    else
    {
        printf("%d bytes written\n", bytes_written);
        
        int num = read(fd, buf, 255);
        if(num > 0) 
        {
            printf("data %d string %s \n", num, buf);
        }
    }
    close(fd);
    return 0;
}
