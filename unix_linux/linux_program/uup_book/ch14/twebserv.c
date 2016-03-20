/*
 * twebserv.c - a thread minimal web server
 * usage: twebserv portnumber
 *
 * features: supports the GET command only
 *           run in the current directory
 *           create a thread to handle each request
 *           support a special status URL to report internal state
 */

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <time.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <sys/stat.h>
#include <pthread.h>
#include <dirent.h>

#define BUFSIZE		1024
#define oops(msg)	{ perror(msg); exit(1); }


/* server facts here */
time_t server_started;
int server_bytes_sent;
int server_requests;


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

void setup(pthread_attr_t *attrp);
void *handle_call(void *arg);
void sanitize(char *str);
int http_reply(int fd, FILE **fpp, int code, char *msg, char *type, char *content);
int built_in(char *arg, int fd);


int
main(int argc,char *argv[])
{
	int sock, fd;
	pthread_t worker;
	pthread_attr_t attr;
	int *fdptr;

	if (argc != 2) {
		fprintf(stderr, "Usage: %s <portnum>\n",argv[0]);
		exit(1);
	}

	sock = make_server_socket(atoi(argv[1]));
	if (sock == -1)
		exit(2);


	setup(&attr);	/* init status variablea */


	/* main loop */
	while (1) {
		fd = accept(sock, NULL, NULL);
		++server_requests;
		if (fd == -1)
			continue;

		fdptr = malloc(sizeof(int));
		*fdptr = fd;
		pthread_create(&worker, &attr, handle_call, (void *) fdptr);
	}
	close(sock);
	return 0;
}


void
setup(pthread_attr_t *attrp)
{
	pthread_attr_init(attrp);
	pthread_attr_setdetachstate(attrp, PTHREAD_CREATE_DETACHED);

	time(&server_started);
	server_requests = 0;
	server_bytes_sent = 0;
}


void *
handle_call(void *arg)
{
	FILE *fpin;
	char request[BUFSIZE];
	int fd;

	fd = *(int *) arg;
	free(arg);		/* !important */

	
	fpin = fdopen(fd, "r");

	/* read request */
	fgets(request, BUFSIZE, fpin);
	printf("got a call: request = %s", request);

	read_til_crnl(fpin);

	/* do what clients asks */
	process_rq(request, fd);

	fclose(fpin);
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


	/* just read: GET /foo/bar */
	if (sscanf(rq, "%s %s", cmd, arg) != 2)
		return;

	sanitize(arg);	/* !important */

	if (strcmp(cmd, "GET") != 0)
		cannot_do(fd);
	else if (built_in(arg, fd))
		;	/* do nothing */
	else if (not_exist(arg))
		do_404(arg, fd);
	else if (isadir(arg))
		do_ls(arg, fd);
	//else if (ends_in_cgi(arg))
	//	do_exec(arg, fd);
	else
		do_cat(arg, fd);
}


/*
 * make sure all paths are below the current directory
 */
void
sanitize(char *str)
{
	char *src, *dest;
	src = dest = str;

	while (*src) {
		if (strncmp(src, "/../", 4) == 0)
			src += 3;
		else if (strncmp(src, "//", 2) == 0)
			++src;
		else
		*dest++ = *src++;
	}
	*dest = '\0';

	if (*str == '/')
		strcpy(str, str+1);

	if (str[0] == '\0' || strcmp(str, "./") == 0 || strcmp(str, "./..") == 0)
		strcpy(str, ".");
}


int
http_reply(int fd, FILE **fpp, int code, char *msg, char *type, char *content)
{
	FILE *fp = fdopen(fd, "w");
	int bytes = 0;

	if (fp != NULL) {
		bytes = fprintf(fp, "HTTP/1.0 %d %s\r\n", code, msg);
		bytes += fprintf(fp, "Content-type: %s\r\n\r\n", type);
		if (content)
			bytes += fprintf(fp, "%s\r\n", content);
	}
	fflush(fp);

	if (fpp)
		*fpp = fp;
	else
		fclose(fp);
	return bytes;
}


/*
 * handle built-in URLs here, only for so far is "status"
 */
int
built_in(char *arg, int fd)
{
	FILE *fp;

	if (strcmp(arg, "status") != 0)
		return 0;

	fp = (FILE *) 1;
	http_reply(fd, &fp, 200, "OK", "text/plain", NULL);

	fprintf(fp, "Server started: %s", ctime(&server_started));
	fprintf(fp, "Total requests: %d\n", server_requests);
	fprintf(fp, "Bytes sent out: %d\n", server_bytes_sent);
	fclose(fp);

	return 1;
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
/*
	FILE *fp = fdopen(fd, "w");

	fprintf(fp, "HTTP/1.0 501 Not Implemented\r\n");
	fprintf(fp, "Content-Type: text/plain\r\n");
	fprintf(fp, "\r\n");

	fprintf(fp, "That command is not yet implemented\r\n");
	fclose(fp);
*/
	http_reply(fd, NULL, 501, "Not Implemented", "text/plain",
		"This command is not yet implemented");
}


void
do_404(char *item, int fd)
{
/*
	FILE *fp = fdopen(fd, "w");

	fprintf(fp, "HTTP/1.0 404 Not Found\r\n");
	fprintf(fp, "Content-Type: text/plain\r\n");
	fprintf(fp, "\r\n");

	fprintf(fp, "Tha item you request: '%s' is not found\r\n", item);
	fclose(fp);
*/
	http_reply(fd, NULL, 404, "Not Found", "text/plain",
		"The item you seek is not here");
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
	DIR *dirptr;
	struct dirent *direntp;
	FILE *fp;
	int bytes = 0;

	fp = (FILE *) 0x1;
	bytes = http_reply(fd, &fp, 200, "OK", "text/plain", NULL);
	bytes += fprintf(fp, "Listing of Directory %s\n", dir);

	if ((dirptr = opendir(dir)) != NULL) {
		while (direntp = readdir(dirptr))
			bytes += fprintf(fp, "%s\n", direntp->d_name);
		closedir(dirptr);
	}
	fclose(fp);
	server_bytes_sent += bytes;
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
do_cat(char *fname, int fd)
{
	char *extension = file_type(fname);
	char type[20] = "text/plain";
	FILE *sock_fp, *file_fp;
	int c;
	int bytes = 0;

	if (strcmp(extension, "html") == 0)
		snprintf(type, sizeof(type), "text/html");
	else if (strcmp(extension, "gif") == 0)
		snprintf(type, sizeof(type), "text/gif");
	else if (strcmp(extension, "jpg") == 0)
		snprintf(type, sizeof(type), "text/jpeg");
	else if (strcmp(extension, "jpeg") == 0)
		snprintf(type, sizeof(type), "text/jpeg");

	sock_fp = fdopen(fd, "w");
	file_fp = fopen(fname, "r");
	if (sock_fp != NULL && file_fp != NULL) {
		bytes = http_reply(fd, &sock_fp, 200, "OK", type, NULL);

		while ((c = getc(file_fp)) != EOF) {
			putc(c, sock_fp);
			++bytes;
		}

		fclose(sock_fp);
		fclose(file_fp);
	}
	server_bytes_sent += bytes;
}


