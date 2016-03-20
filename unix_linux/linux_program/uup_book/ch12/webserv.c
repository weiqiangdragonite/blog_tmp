/*
 * webserv.c - a minimal web server
 * usage: webserv portnumber
 *
 * features: supports the GET command only
 *           run in the current directory
 *           forks a new child to handle each request
 *           has MAJOR security holes, for demo purposes only
 *           has many other weakness, but it is a good start
 */

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <time.h>
#include <sys/stat.h>

#define BUFSIZE		1024
#define oops(msg)	{ perror(msg); exit(1); }


int make_server_socket(int portnum);
void read_til_crnl(FILE *fp);
void process_rq(char *rq, int fd);
void cannot_do(int fd);
void do_404(char *item, int fd);
void do_ls(char *dir, int fd);
void do_exec(char *prog, int fd);
void do_cat(char *f, int fd);
int isadir(char *f);
int not_exist(char *f);
int ends_in_cgi(char *f);



int
main(int argc,char *argv[])
{
	int sock, fd;
	FILE *fpin;
	char request[BUFSIZE];

	if (argc != 2) {
		fprintf(stderr, "Usage: %s <portnum>\n",argv[0]);
		exit(1);
	}

	sock = make_server_socket(atoi(argv[1]));
	if (sock == -1)
		exit(2);

	/* main loop */
	while (1) {
		fd = accept(sock, NULL, NULL);
		fpin = fdopen(fd, "r");

		/* read request */
		fgets(request, BUFSIZE, fpin);
		printf("got a call: request = %s", request);

		read_til_crnl(fpin);

		/* do what clients asks */
		process_rq(request, fd);

		fclose(fpin);
	}
	close(sock);
	return 0;
}


int
make_server_socket(int portnum)
{
	int serv_sock;
	struct sockaddr_in serv_addr;

	/* step 1 */
	serv_sock = socket(AF_INET, SOCK_STREAM, 0);
	if (serv_sock == -1)
		return -1;

	/* step 2 */
	memset(&serv_addr, 0, sizeof(serv_addr));
	serv_addr.sin_family = AF_INET;
	serv_addr.sin_addr.s_addr = htonl(INADDR_ANY);
	serv_addr.sin_port = htons(portnum);

	if (bind(serv_sock, (struct sockaddr *) &serv_addr, sizeof(serv_addr)) == -1)
		return -1;

	/* step 3 */
	if (listen(serv_sock, 5) == -1)
		return -1;

	return serv_sock;
}


/*
 * skip over all request info until a CRNL is read
 */
void
read_til_crnl(FILE *fp)
{
	char buf[BUFSIZE];
	while (fgets(buf, BUFSIZE, fp) != NULL && strcmp(buf, "\r\n") != 0)
		continue;
}


/*
 * rq is http command like: GET /foo/bar HTTP/1.0
 */
void
process_rq(char *rq, int fd)
{
	char cmd[BUFSIZE], arg[BUFSIZE];

	/* create child procress */
	if (fork() != 0)
		return;

	/* child go here */

	strcpy(arg, "./");	/* start form local dir */
	/* just read: GET /foo/bar */
	if (sscanf(rq, "%s %s", cmd, arg+2) != 2)
		return;

	if (strcmp(cmd, "GET") != 0)
		cannot_do(fd);
	else if (not_exist(arg))
		do_404(arg, fd);
	else if (isadir(arg))
		do_ls(arg, fd);
	else if (ends_in_cgi(arg))
		do_exec(arg, fd);
	else
		do_cat(arg, fd);
}


void
header(FILE *fp, char *content_type)
{
	fprintf(fp, "HTTP/1.0 200 OK\r\n");
	if (content_type)
		fprintf(fp, "Content-type: %s\r\n", content_type);
}


void
cannot_do(int fd)
{
	FILE *fp = fdopen(fd, "w");

	fprintf(fp, "HTTP/1.0 501 Not Implemented\r\n");
	fprintf(fp, "Content-Type: text/plain\r\n");
	fprintf(fp, "\r\n");

	fprintf(fp, "That command is not yet implemented\r\n");
	fclose(fp);
}


void
do_404(char *item, int fd)
{
	FILE *fp = fdopen(fd, "w");

	fprintf(fp, "HTTP/1.0 404 Not Found\r\n");
	fprintf(fp, "Content-Type: text/plain\r\n");
	fprintf(fp, "\r\n");

	fprintf(fp, "Tha item you request: '%s' is not found\r\n", item);
	fclose(fp);
}


int
isadir(char *f)
{
	struct stat info;
	return (stat(f, &info) != -1 && S_ISDIR(info.st_mode));
}

int
not_exist(char *f)
{
	struct stat info;
	return (stat(f, &info) == -1);
}



void
do_ls(char *dir, int fd)
{
	FILE *fp = fdopen(fd, "w");

	header(fp, "text/plain");
	fprintf(fp, "\r\n");
	fflush(fp);

	dup2(fd, 1);
	dup2(fd, 2);
	close(fd);
	execlp("ls", "ls", "-l", dir, NULL);

	perror("execlp() failed");
	exit(1);
}


char *file_type(char *f)
{
	char *cp;
	if ((cp = strrchr(f, '.')) != NULL)
		return cp+1;
	return "";
}

int
ends_in_cgi(char *f)
{
	return (strcmp(file_type(f), "cgi") == 0);
}


void
do_exec(char *prog, int fd)
{
	FILE *fp = fdopen(fd, "w");

	header(fp, NULL);
	fprintf(fp, "\r\n");
	fflush(fp);

	dup2(fd, 1);
	dup2(fd, 2);
	close(fd);
	execlp(prog, prog, NULL);

	perror("execlp() failed");
	exit(1);
}


void
do_cat(char *f, int fd)
{
	char *extension = file_type(f);
	char content[20] = "text/plain";
	FILE *sock_fp, *file_fp;
	int c;

	if (strcmp(extension, "html") == 0)
		snprintf(content, sizeof(content), "text/html");
	else if (strcmp(extension, "gif") == 0)
		snprintf(content, sizeof(content), "text/gif");
	else if (strcmp(extension, "jpg") == 0)
		snprintf(content, sizeof(content), "text/jpeg");
	else if (strcmp(extension, "jpeg") == 0)
		snprintf(content, sizeof(content), "text/jpeg");

	sock_fp = fdopen(fd, "w");
	file_fp = fopen(f, "r");
	if (sock_fp != NULL && file_fp != NULL) {
		header(sock_fp, content);
		fprintf(sock_fp, "\r\n");

		while ((c = getc(file_fp)) != EOF)
			putc(c, sock_fp);

		fclose(sock_fp);
		fclose(file_fp);
	}
	exit(0);
}


