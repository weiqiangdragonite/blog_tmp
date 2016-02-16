/*
 * rlsd.c
 * a remote ls server
 */

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <time.h>


#define BUFSIZE		1024
#define PORTNUM		15000
#define oops(msg)	{ perror(msg); exit(1); }


void sanitize(char *str);

int
main(int argc, char *argv[])
{
	int serv_sock, cli_sock;

	struct sockaddr_in serv_addr, cli_addr;
	socklen_t cli_size;
	int c;
	char cmd[BUFSIZE], dirname[BUFSIZE];
	FILE *sock_fpin, *sock_fpout, *pipe_fp;

	/* step 1 */
	serv_sock = socket(AF_INET, SOCK_STREAM, 0);
	if (serv_sock == -1)
		oops("socket() failed");

	/* step 2 */
	memset(&serv_addr, 0, sizeof(serv_addr));
	serv_addr.sin_family = AF_INET;
	serv_addr.sin_addr.s_addr = htonl(INADDR_ANY);
	serv_addr.sin_port = htons(PORTNUM);

	if (bind(serv_sock, (struct sockaddr *) &serv_addr, sizeof(serv_addr)) == -1)
		oops("bind() failed");

	/* step 3 */
	if (listen(serv_sock, 5) == -1)
		oops("listen() failed");

	/* main loop */
	while (1) {
		cli_size = sizeof(cli_addr);
		cli_sock = accept(serv_sock, (struct sockaddr *) &cli_addr, &cli_size);
		if (cli_sock == -1)
			oops("accept() failed");

		/* open sock for reading */
		sock_fpin = fdopen(cli_sock, "r");
		if (sock_fpin == NULL)
			oops("fdopen() reading failed");


		if (fgets(dirname, BUFSIZE, sock_fpin) == NULL)
			oops("fgets() failed");
		sanitize(dirname);

		/* open sock for writing */
		sock_fpout = fdopen(cli_sock, "w");
		if (sock_fpout == NULL)
			oops("fdopen() writing failed");


		snprintf(cmd, BUFSIZE, "ls %s", dirname);
		if ((pipe_fp = popen(cmd, "r")) == NULL)
			oops("popen() failed");

		//char buf[BUFSIZE];
		//while (fgets(buf, BUFSIZE, pipe_fp) != NULL) {
		//	fprintf(stdout, "%s", buf);
		//	fprintf(sock_fpout, "%s", buf);
		//	fflush(sock_fpout);
		//}


		/* transfer data from ls to socket */
		while ((c = getc(pipe_fp)) != EOF)
			putc(c, sock_fpout);
		fflush(sock_fpout);

		pclose(pipe_fp);
		fclose(sock_fpin);
		fclose(sock_fpout);
	}

	return 0;
}


/*
 * it would be very dangerous if someone passed us and dirname like
 * "; rm *" and we naively created a command "ls ; rm *"
 *
 * so we remove everything but slashed and alphanumerics
 */
void
sanitize(char *str)
{
	char *src, *dest;

	for (src = dest = str; *src; *src++) {
		if (*src == '/' || isalnum(*src))
			*dest++ = *src;
	}
	*dest = '\0';
}


