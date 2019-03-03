/*
 * File: udp_server.c
 * ------------------
 * This is udp server.
 */

#include <stdio.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>

void udp_respon(int sockfd);

int main(void)
{
    int sockfd;
    struct sockaddr_in server_addr;

    // 1. create socket - base on UDP
    if ((sockfd = socket(AF_INET, SOCK_DGRAM, 0)) == -1) {
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
    
    // 3. send and receive data
    udp_respon(sockfd);

    // 4. close
    close(sockfd);

    return 0;

}

void udp_respon(int sockfd)
{
    struct sockaddr_in addr;
    int add_size;
    char buffer[80];

    while (1) {
        // clean up the buffer
        bzero(buffer, sizeof(buffer));
        
        add_size = sizeof(struct sockaddr);
        recvfrom(sockfd, buffer, sizeof(buffer), 0,
                (struct sockaddr *) &addr, &add_size);
        printf("server receive: %s\n", buffer);
    }
}
