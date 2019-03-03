/*
 * File: tcp_server.c
 * ------------------
 * This is tcp server.
 */

#include <stdio.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>

int main(void)
{
    int sockfd, cli_fd;
    char buffer[80];
    struct sockaddr_in server_addr;
    struct sockaddr_in client_addr;

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
        int sock_size = sizeof(struct sockaddr);
        if ((cli_fd = accept(sockfd, (struct sockaddr *) &client_addr, &sock_size)) == -1) {
            perror("accept client error");
            return 4;
        }

        printf("server get connection form %s\n",
                inet_ntoa(client_addr.sin_addr));

        // clean the buffer
        memset(buffer, '\0', sizeof(buffer));
        // read data from client
        if (read(cli_fd, buffer, sizeof(buffer)) == -1) {
            perror("read data from client error");
            return 5;
        }

        // disconnect form client
        if (strcmp(buffer, "exit") == 0) {
            // send data to client exit
            // close the connect
            close(cli_fd);
        }

        // print the data from client
        printf("server receive data from %s is: %s",
                inet_ntoa(client_addr.sin_addr), buffer);
    }

    return 0;

}
