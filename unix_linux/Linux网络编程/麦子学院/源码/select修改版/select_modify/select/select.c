#include "socket_includes.h"



int mz_ipv4_tcp_create_socket(void)
{
	int listenfd, sockfd, opt = 1;
	struct sockaddr_in server, client;
	socklen_t len;
	int timep;
	int ret;

	listenfd = socket(AF_INET, SOCK_STREAM, 0);
	if(listenfd < 0){
		perror("Create socket fail.");
		return -1;
	} 

	if((ret = setsockopt(listenfd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt))) < 0){
		perror("Error, set socket reuse addr failed");  
		return -1;
	}

	bzero(&server, sizeof(server));
	server.sin_family = AF_INET;
	server.sin_port   = htons(SERV_PORT);
	server.sin_addr.s_addr  = htonl(INADDR_ANY);
	
	len = sizeof(struct sockaddr);
	if(bind(listenfd, (struct sockaddr *)&server, len)<0){
			  perror("bind error.");
		return -1;
	}
	
	  
	listen(listenfd, MAX_LISTEN_QUE);

	return listenfd;
}




int main(int argc, char *argv[])
{
	int listenfd, sockfd;
	struct sockaddr_in server, client;
	socklen_t len;
	int bytes =0 ;
	fd_set global_rdfs,current_rdfs;	
	int maxfd;	
	int i;
	char buf[MAX_BUFFER_SIZE];
	int client_fd[FD_SETSIZE];

	len = sizeof(struct sockaddr_in);

	printf("FD_SETSIZE=%d\n", FD_SETSIZE);

	listenfd = mz_ipv4_tcp_create_socket();
	FD_ZERO(&global_rdfs);
	FD_SET(listenfd, &global_rdfs);
	maxfd = listenfd;
	for(i = 0; i < FD_SETSIZE; i++ )
		client_fd[i] = -1;

	while(1){
		current_rdfs = global_rdfs;
		if(select(maxfd + 1, &current_rdfs, NULL, NULL, NULL)<0){
			perror("select error.\n");
			return RT_ERR;
		}

		if(FD_ISSET(listenfd, &current_rdfs)){
			
			if((sockfd = accept(listenfd, (struct sockaddr*)&client, (socklen_t*)&len))<0){
				perror("accept error.\n");
				return RT_ERR;
			}
			printf("sockfd:%d\n", sockfd);
			FD_CLR(i, &current_rdfs);
			maxfd = maxfd > sockfd ? maxfd :sockfd;
			FD_SET(sockfd, &global_rdfs);
			for(i = 0; i < maxfd; i++){
				if(-1 == client_fd[i]){
					client_fd[i] = sockfd;
					break;
				}
			}
		}

		for(i = 0; i <= maxfd; i++){
			if(-1 == client_fd[i]){
				continue;
			}	
			if(FD_ISSET(client_fd[i], &current_rdfs)){
				printf("read socket:%d\n", client_fd[i]);
				bytes = recv(client_fd[i], buf, MAX_BUFFER_SIZE, 0);
				if(bytes < 0){
					perror("recv error.\n");
					return RT_ERR;
				}
				if(bytes == 0){
					FD_CLR(client_fd[i], &global_rdfs);
					close(client_fd[i]);
					client_fd[i] = -1;
					continue;
				}
				printf("buf:%s\n", buf);
				send(client_fd[i], buf, strlen(buf), 0);

			}
		
		}
	}
}







