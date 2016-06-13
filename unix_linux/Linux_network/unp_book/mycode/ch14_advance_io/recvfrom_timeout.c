

void
dg_cli(FILE *fp, int sockfd, const struct sockaddr *svaddr, socklen_t svlen)
{
	int n;
	char sendline[1024];
	char recvline[1024+1];

	signal(SIGALRM, sig_alrm);

	while (fgets(sendline, sizeof(sendline), fp) != NULL) {
		sendto(sockfd, sendline, strlen(sendline), 0, svaddr, svlen);

		alarm(5);
		if ((n = recvfrom(sockfd, recvline, sizeof(recvline)-1, 0, NULL, NULL)) < 0) {
			if (errno == EINTR)
				fprintf(stderr, "socket timeout\n");
			else {
				perror("recvfrom() failed");
				exit(-1);
			}
		} else {
			alarm(0);

			recvline[n] = '\0';
			fputs(recvline, fp);
		}
	}
}


