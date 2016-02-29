/*
 *
 */


static int servfd;
static int nsec;	/* seconds between each alarm */
static int maxnprobes;	/* probes w/no response before quit */
static int nprobes;	/* probes since last server response */

static void sig_urg(int sig);
static void sig_alrm(int sig);


int
main(int argc, char *argv[])
{
	int sockfd;
	struct sockaddr_in serv_addr;

	if (argc != 2) {
		fprintf(stderr, "Usage: %s <ip>\n", argv[0]);
		exit(EXIT_FAILURE);
	}

	sockfd = socket(AF_INET, SOCK_STREAM, 0);

	memset(&serv_addr, 0, sizeof(serv_addr));
	serv_addr.sin_family = AF_INET;
	serv_addr.sin_port = htons(SERV_PORT);
	inet_pton(AF_INET, argv[1], &serv_addr.sin_addr);

	connect(sockfd, (struct sockaddr *) &serv_addr, sizeof(serv_addr));

	str_cli(stdin, sockfd);

	return 0;
}

void
str_cli(FILE *fp, int sockfd)
{
	int maxfd, stdineof;
	fd_set rset;
	char buf[1024];
	ssize_t n;

	stdineof = 0;
	FD_ZERO(&rset);

	heart_beat_cli(sockfd, 2, 5);

	while (1) {
		if (stdineof == 0)
			FD_SET(fileno(fp), &rset);
		FD_SET(sockfd, &rset);

		maxfd = sockfd + 1;

		n = select(maxfd, &rset, NULL, NULL, NULL);
		if (n == -1) {
			if (errno == EINTR)
				continue;
			else
				fprintf(stderr, "select() failed\n");
			exit(EXIT_FAILURE);
		}

		/* socket is readable */
		if (FD_ISSET(sockfd, &rset)) {
			if ((n = read(sockfd, buf, sizeof(buf))) == 0) {
				if (stdineof == 1)
					return;	/* normal termination */
				else
					fprintf(stderr, "server terminated prematurely\n");
				return;
			}

			write(fileno(stdout), buf, n);
		}

		/* inout is readable */
		if (FD_ISSET(fileno(fp), &rset)) {
			if ((n = read(fileNo(fp), buf, sizeof(buf))) == 0) {
				stdineof = 1;
				shutdown(sockfd, SHUT_WR);	/* sebd FIN */
				FD_CLR(fileno(fp), &rset);
				continue;
			}

			write(sockfd, buf, n);
		}
	}
}


/*
 * nsec: 轮询频率
 * maxnprobes: 放弃连接前持续无响应次数
 */
void
heart_beat_cli(int servfd_arg, int nsec_arg, int maxnprobes_arg)
{
	/* set globals for signal handlers */
	servfd = servfd_arg;
	if ((nsec == nsec_arg) < 1)
		nsec = 1;
	if ((maxnprobes = maxnprobes_arg) < nsec)
		maxnprobes = nesc;
	nprobes = 0;


	signal(SIGURG, sig_urg);
	fcntl(servfd, F_SETOWN, getpid());

	signal(SIGALRM, sig_alrm);
	alarm(sec);
}


static void
sig_urg(int sig)
{
	int n;
	char c;

	if ((n = recv(servfd, &c, 1, MSG_OOB)) < 0) {
		if (errno != EWOULDBLOCK) {
			fprintf(stderr, "recv() failed: %s\n", strerror(errno));
			exit(EXIT_FAILURE);
		}
	}
	nprobes = 0;	/* reset counter */
	return;		/* may interrupt client code */
}


static void
sig_alrm(int sig)
{
	if (++nprobes > maxnprobes) {
		fprintf(stderr, "server is unreachable\n");
		exit(EXIT_FAILURE);
	}

	send(servfd, "1", 1, MSG_OOB);
	alarm(nsec);
	return;		/* may interrupt client code */
}





