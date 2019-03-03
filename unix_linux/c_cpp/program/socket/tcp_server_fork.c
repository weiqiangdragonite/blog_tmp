/*
 * File: tcp_server_fork.c
 * ----------------------
 * This is tcp server.
 */

#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>

int main(void)
{
    int sockfd, cli_fd;
    struct sockaddr_in server_addr;

    // 1. create socket - base on TCP
    if ((sockfd = socket(AF_INET, SOCK_STREAM, 0)) == -1) {
        perror("create socket error");
        return 1;
    }
    puts("create socket done!");

    // init the server struct to '\0'
    bzero(&server_addr, sizeof(struct sockaddr_in));
    // set the server to IP portocol
    server_addr.sin_family = AF_INET;
    // this will set the server bind to any ip address(0.0.0.0)
    server_addr.sin_addr.s_addr = htonl(INADDR_ANY);
    // or set to the sure address:
    // server_addr.sin_addr.s_addr = inet_addr("192.168.2.110");
    // set the port
    server_addr.sin_port = htons(8888);

    // 2. bind to socket
    // if the server stop, this will let server start again very soon
    int optval = 1;
    setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, &optval, sizeof(int));
    if (bind(sockfd, (struct sockaddr *) &server_addr,
                sizeof(struct sockaddr)) == -1) {
        perror("bind socket error");
        return 2;
    }
    puts("bind socket done!");

    // 3. set the server max connect
    if (listen(sockfd, 5) == -1) {
        perror("set the server max connect error");
        return 3;
    }
    puts("set the server max connect done!");

    // 4. wait for client to connect
    puts("wait for connect");
    while (1) {
        // the server is block, wait for connect

        cli_fd = accept(sockfd, NULL, NULL);
        if ((cli_fd < 0) && (errno == EINTR)) {
            continue;
        } else if (cli_fd < 0) {
            perror("client connect error");
            sleep(1);
            continue;
        }

        if (fork() == 0) {
            // child process
            char buffer[80];

            memset(buffer, 0, sizeof(buffer));
            read(cli_fd, buffer, sizeof(buffer));

            printf("server receive: %s", buffer);

            //
            close(sockfd);
            close(cli_fd);

            return 0;
        } else {
            close(cli_fd);
        }
    }

    return 0;

}
