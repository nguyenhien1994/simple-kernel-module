#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>

#define DEVICE_NAME "/dev/reverse"
#define BUFFER_SIZE 1024

int main()
{
    int fd;
    char input[BUFFER_SIZE];
    char output[BUFFER_SIZE];

    fd = open(DEVICE_NAME, O_RDWR);
    if (fd < 0)
    {
        perror("Failed to open the device...");
        return errno;
    }

    printf("Enter a string: ");
    scanf("%[^\n]%*c", input);
    write(fd, input, strlen(input));
    read(fd, output, BUFFER_SIZE);
    printf("Reversed string: %s\n", output);

    close(fd);
    return 0;
}