/*
 * A simple proxy server
 */


#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>

#include <arpa/inet.h>
#include <sys/socket.h>

#include <pthread.h>


#define BACK_LOG	5
#define BUF_SIZE	1024



int create_socket(int port);
void *handle_client(void *arg);
void process_req_header(char *str);



int
main(int argc, char *argv[])
{
	int sockfd, clifd;
	struct sockaddr_in cli_addr;
	socklen_t cli_len;
	pthread_t tid;

	if (argc != 2) {
		fprintf(stderr, "Usage: %s <port>\n", argv[0]);
		exit(EXIT_FAILURE);
	}

	sockfd = create_socket(atoi(argv[1]));
	if (sockfd == -1)
		exit(EXIT_FAILURE);


	cli_len = sizeof(cli_addr);
	while (1) {
		clifd = accept(sockfd, (struct sockaddr *) &cli_addr, &cli_len);
		if (clifd == -1) {
			perror("accept() failed");
			continue;
		}

		char str[INET_ADDRSTRLEN];
		if (inet_ntop(AF_INET, &cli_addr.sin_addr, str, sizeof(str)) == NULL) {
			fprintf(stderr, "inet_ntop() failed\n");
		} else {
			printf("client[%d] connect, ip = %s, port = %d\n",
			clifd, str, cli_addr.sin_port);
		}

		pthread_create(&tid, NULL, handle_client, (void *) &clifd);
		pthread_detach(tid);
	}
	close(sockfd);

	return 0;
}


/*
 * create ipv4 socket
 */
int
create_socket(int port)
{
	struct sockaddr_in serv_addr;
	int fd;
	int opt = 1;

	fd = socket(AF_INET, SOCK_STREAM, 0);
	if (fd == -1) {
		perror("socket() failed");
		return -1;
	}

	memset(&serv_addr, 0, sizeof(serv_addr));
	serv_addr.sin_family = AF_INET;
	serv_addr.sin_addr.s_addr = htonl(INADDR_ANY);
	serv_addr.sin_port = htons(port);


	if(setsockopt(fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt)) < 0){
		perror("setsockopt() failed");  
		return -1;
	}

	if (bind(fd, (struct sockaddr *) &serv_addr, sizeof(serv_addr)) == -1) {
		perror("bind() failed");
		return -1;
	}

	if (listen(fd, BACK_LOG) == -1) {
		perror("listen() failed");
		return -1;
	}

	return fd;
}


/*
 * get the client request, send to server and received the data, send back to
 * client
 */
void *
handle_client(void *arg)
{
	char buf[BUF_SIZE];
	int str_len;

	int fd = *((int *) arg);

	while (1) {
		str_len = read(fd, buf, sizeof(buf) - 1);
		if (str_len == -1) {
			perror("read() failed");
			break;
		} else if (str_len == 0) {
			printf("client[%d] closed\n", fd);
			break;
		}
		buf[str_len] = '\0';
		//printf("%s", buf);

		// TODO
		// we must ensure get all the request headers before process
		process_header(buf);

		// 
		send_server();

		//
		recv_data();

		//
		send_client();
	}

	close(fd);
	return NULL;
}


/*
 * for http header
 * header: request + \r\n + body
 */
void
process_req_header(char *str)
{


}


void
send_server()
{
	struct addrinfo *res;
	struct addrinfo hints;

	memset(&hints, 0, sizeof(hints));
	hints.ai_family = AF_UNSPEC;
	hints.ai_socktype = SOCK_STREAM;
	getaddrinfo(host, "80", &hints, &res);

	sockfd = socket(res->ai_family, res->ai_socktype, res->ai_protocol);
	if (sockfd == -1) {
		perror("socket() failed");
	}

	if (connect(sockfd, res->ai_addr, res->ai_addrlen) == -1) {
		perror("connect() failed");
	}

	freeaddrinfo(res);

	

}

