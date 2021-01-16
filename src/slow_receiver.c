#include <sys/types.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <unistd.h>

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define SOCKET_FILENAME "slow_receiver.sock"
#define DEFAULT_BACKLOG (5)
#define BUFFER_SIZE (20 * 1024)

int main(int argc, char* argv[])
{
    int const server_fd = socket(AF_LOCAL, SOCK_STREAM, 0);
    if (0 > server_fd)
    {
        perror("socket");
        exit(EXIT_FAILURE);
    }

    unlink(SOCKET_FILENAME);
    
    struct sockaddr_un address;
    memset(&address, 0, sizeof(address));
    address.sun_family = AF_LOCAL;
    strcpy(address.sun_path, SOCKET_FILENAME);

    int rc = bind(server_fd, (struct sockaddr const *) &address, sizeof(address));
    if (0 != rc)
    {
        perror("bind");
        close(server_fd);
        exit(EXIT_FAILURE);
    }

    rc = listen(server_fd, DEFAULT_BACKLOG);
    if (0 != rc)
    {
        perror("bind");
        close(server_fd);
        exit(EXIT_FAILURE);
    }

    for (;;)
    {
        int const client_fd = accept(server_fd, NULL, NULL);
        if (0 <= client_fd)
        {
            printf("connected\n");

            char buffer[BUFFER_SIZE];
            ssize_t count = read(client_fd, buffer, BUFFER_SIZE);
            while (0 < count)
            {
                printf("received %zi bytes...\n", count);
                sleep(1); // work
                count = read(client_fd, buffer, BUFFER_SIZE);
            }

            printf("connection closed\n");
            close(client_fd);
        }
    }


    close(server_fd);
    return EXIT_SUCCESS;
}