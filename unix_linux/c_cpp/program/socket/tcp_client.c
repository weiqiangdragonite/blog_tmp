/*
 * File: tcp_client.c
 * ------------------
 * This is tcp client.
 */

#include <stdio.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>

int main(int argc, char *argv[])
{
    int sockfd;
    char buffer[80];
    struct sockaddr_in server_addr;
    struct hostent *host;

    // check the argv
    if (argc != 2) {
        printf("Usage: %s hostname\n", argv[0]);
        return -1;
    }

    // use hostname to get the host
    if ((host = (struct hostent *) gethostbyname(argv[1])) == NULL) {
        perror("get hostname error");
        return -2;
    }

    // 1. create socket - base on TCP
    if ((sockfd = socket(AF_INET, SOCK_STREAM, 0)) == -1) {
        perror("create socket error");
        return 1;
    }
    puts("create socket done!");

    // 2. set the server struct attr
    
    // init the server struct to '\0'
    bzero(&server_addr, sizeof(struct sockaddr_in));
    // set the server to IP portocol
    server_addr.sin_family = AF_INET;
    // set the server ip address
    server_addr.sin_addr.s_addr = inet_addr("192.168.2.110");
    // set the port
    server_addr.sin_port = htons(8888);

    // 3. connect to server
    puts("connect to server ...");
    if (connect(sockfd, (struct sockaddr *) &server_addr, sizeof(struct sockaddr)) == -1) {
        perror("connect to server error");
        return 3;
    }
    puts("connect server done!");

    // 4. send data to server
    //while (1) {
        // clean buffer
        memset(buffer, '\0', sizeof(buffer));

        // get user input
        puts("please input msg:");
        fgets(buffer, sizeof(buffer), stdin);

        // send data to server
        write(sockfd, buffer, sizeof(buffer));


        // receive data from server
        //

        // close
        close(sockfd);
    //}

    return 0;

}
