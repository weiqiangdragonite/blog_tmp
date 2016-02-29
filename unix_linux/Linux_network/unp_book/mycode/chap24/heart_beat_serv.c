/*
 *
 */


static int servfd;
static int nsec;	/* seconds between each alarm */
static int maxnalarms;	/* alarms w/no client probe before quit */
static int nprobes;	/* alarms since last client response */

static void sig_urg(int sig);
static void sig_alrm(int sig);


int
main(int argc, char *argv[])
{

}


/*
 * nsec: 轮询频率
 * maxnalarms: 放弃连接前持续无响应次数
 */
void
heart_beat_serv(int servfd_arg, int nsec_arg, int maxnalarms_arg)
{
	/* set globals for signal handlers */
	servfd = servfd_arg;
	if ((nsec == nsec_arg) < 1)
		nsec = 1;
	if ((maxnalarms = maxnalarms_arg) < nsec)
		maxnalarms = nesc;
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
	send(servfd, &c, 1, MSG_OOB);	/* echo back out-of-band byte */

	nprobes = 0;	/* reset counter */
	return;		/* may interrupt server code */
}


static void
sig_alrm(int sig)
{
	if (++nprobes > maxnalarms) {
		fprintf(stderr, "no probes from client\n");
		exit(EXIT_FAILURE);
	}

	alarm(nsec);
	return;		/* may interrupt server code */
}





