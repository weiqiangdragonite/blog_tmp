/*
 * File: udp_client.c
 * ------------------
 * This is udp client.
 */

#include <stdio.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>

void udp_respon(int sockfd, const struct sockaddr *addr, int size);

int main(int argc, char *argv[])
{
    int sockfd;
    struct sockaddr_in server_addr;

    if (argc != 2) {
        printf("Usage: %s server_ip\n", argv[0]);
        return -1;
    }

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
    //server_addr.sin_addr.s_addr = htonl(INADDR_ANY);
    // or set to the sure address:
    // server_addr.sin_addr.s_addr = inet_addr("192.168.2.110");
    // set the port
    server_addr.sin_port = htons(8888);
    // set server ip address in another way
    if (inet_aton(argv[1], &server_addr.sin_addr) < 0) {
        perror("server ip address error");
        return -2;
    }

    // 2. bind to socket
    //if (bind(sockfd, (struct sockaddr *) &server_addr,
    //            sizeof(struct sockaddr)) == -1) {
    //    perror("bind socket error");
    //    return 2;
    //}
    //puts("bind socket done!");
    
    // 3. send and receive data
    udp_respon(sockfd, (struct sockaddr *) &server_addr, sizeof(struct sockaddr_in));

    // 4. close
    close(sockfd);

    return 0;

}

void udp_respon(int sockfd, const struct sockaddr *addr, int size)
{
    char buffer[80];

    while (1) {
        // clean up the buffer
        bzero(buffer, sizeof(buffer));
        
        // get user input
        puts("please input:");
        fgets(buffer, sizeof(buffer), stdin);

        // send to server
        sendto(sockfd, buffer, sizeof(buffer), 0, addr, size);
    }
}
