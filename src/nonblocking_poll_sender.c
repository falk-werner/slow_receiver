#include <sys/types.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <poll.h>
#include <unistd.h>

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>

#define SOCKET_FILENAME "slow_receiver.sock"
#define BUFFER_SIZE (1024 * 1024)
#define TIMEOUT (3 * 1000)

int main(int argc, char* argv[])
{
    int const fd = socket(AF_LOCAL, SOCK_STREAM | SOCK_NONBLOCK, 0);
    if (0 > fd)
    {
        perror("socket");
        exit(EXIT_FAILURE);
    }

    struct sockaddr_un address;
    memset(&address, 0, sizeof(address));
    address.sun_family = AF_LOCAL;
    strcpy(address.sun_path, SOCKET_FILENAME);

    int rc = connect(fd, (struct sockaddr const *) &address, sizeof(address));
    if (0 != rc)
    {
        perror("connect");
        close(fd);
        exit(EXIT_FAILURE);
    }    

    char * buffer = malloc(BUFFER_SIZE);    
    memset(buffer, 0, BUFFER_SIZE);
    size_t offset = 0;
    size_t remaining = BUFFER_SIZE;

    struct pollfd poll_fd;
    poll_fd.fd = fd;
    poll_fd.events = POLLOUT;

    while (remaining > 0)
    {
        int fd_count = poll(&poll_fd, 1, TIMEOUT);
        if (1 == fd_count)
        {
            // Note: revents are not checked; if there was an error, write will fail
            ssize_t count = write(fd, &buffer[offset], remaining);
            if (0 < count)
            {
                printf("wrote %zi bytes\n", count);
                offset += count;
                remaining -= count;
            }
            else if ((-1 == count) && ((errno == EAGAIN) || (errno == EINTR)))
            {
                // try again
            }
            else
            {
                perror("write");
                free(buffer);
                close(fd);
                exit(EXIT_FAILURE);
            }
        }
        else if ((-1 == fd_count) && (errno != EINTR))
        {
            perror("poll");
            free(buffer);
            close(fd);
            exit(EXIT_FAILURE);

        }
    }

    free(buffer);
    close(fd);
    return EXIT_SUCCESS;
}