/*
 * smsh1.c small - shell version 1
 *
 * first really useful version after prompting shell
 * this one parses the command line into strings
 * uses fork, exec, wait, and ignores signals
 */

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <signal.h>

#define YES		1
#define NO		0
#define MAXARGS		20	/* cmdline args */
#define ARGLEN		100	/* token length */
#define DFL_PROMPT	">>> "
#define BUFSIZE		1024


char *next_cmd(char *prompt, FILE *fp);
char **splitline(char *line);
void freelist(char **list);
int execute(char **arglist);


char *make_string(char *buf);

int
main(int argc, char *argv[])
{
	char *cmdline, *prompt, **arglist;
	int result;


	prompt =  DFL_PROMPT;

	/* ignore signals */
	signal(SIGINT, SIG_IGN);
	signal(SIGQUIT, SIG_IGN);

	while ((cmdline = next_cmd(prompt, stdin)) != NULL) {
		if ((arglist = splitline(cmdline)) != NULL) {
			result = execute(arglist);
			freelist(arglist);
		}
		free(cmdline);
	}

	return 0;
}

void
fatal(char *s1, char *s2, int n)
{
	fprintf(stderr, "%s%s\n", s1, s2);
	exit(n);
}


void *
emalloc(size_t n)
{
	void *rv;
	if ((rv = malloc(n)) == NULL)
		fatal("malloc() failed: ", strerror(errno), 1);
	return rv;
}

void *
erealloc(void *p, size_t n)
{
	void *rv;
	if ((rv = realloc(p, n)) == NULL)
		fatal("realloc() failed: ", strerror(errno), 1);
	return rv;
}

/*
 * read next command line from fp
 */
char *
next_cmd(char *prompt, FILE *fp)
{
	char *buf;
	int buf_space = 0;	/* total size */
	int pos = 0;		/* current position */
	int c = 0;		/* input char */

	printf("%s", prompt);
	while ((c = getc(fp)) != EOF) {
		/* need space */
		if (pos + 1 >= buf_space) {
			if (buf_space == 0)
				buf = emalloc(BUFSIZE);
			else
				buf = erealloc(buf, buf_space + BUFSIZE);
			buf_space += BUFSIZE;	/* update size */
		}

		/* end of command */
		if (c == '\n')
			break;

		/* add to buffer */
		buf[pos++] = c;
	}

	if (c == EOF && pos == 0)
		return NULL;

	buf[pos] = '\0';
	return buf;
}


char *
newstr(char *s, int n)
{
	char *rv = emalloc(n + 1);
	rv[n] = '\0';
	strncpy(rv, s, n);
	return rv;
}


#define is_delim(x)	((x) == ' ' || (x) == '\t')
/*
 * splitline (parse a line into an array of strings)
 */
char **
splitline(char *line)
{
	char **args;
	int spots = 0;
	int buf_space = 0;
	int arg_num = 0;
	char *cp = line;
	char *start;
	int len;

	if (line == NULL)
		return NULL;

	args = emalloc(BUFSIZE);
	buf_space = BUFSIZE;
	spots = BUFSIZE / sizeof(char *);

	while (*cp != '\0') {
		while (is_delim(*cp))
			++cp;
		if (*cp == '\0')
			break;

		/* make sure the array has room (+1 for NULL) */
		if (arg_num + 1 >= spots) {
			args = erealloc(args, buf_space + BUFSIZE);
			buf_space += BUFSIZE;
			spots += BUFSIZE / sizeof(char *);
		}

		/* make start, then find the end of word */
		start = cp;
		len = 1;
		while (*++cp != '\0' && !(is_delim(*cp)))
			++len;
		args[arg_num++] = newstr(start, len);
	}
	args[arg_num] = NULL;
	return args;
}


void
freelist(char *list[])
{
	char **cp = list;
	while (*cp)
		free(*cp++);
	free(list);
}


int
execute(char *arglist[])
{
	pid_t pid;
	int exit_status;

	if (arglist[0] == NULL)
		return 0;

	pid = fork();
	if (pid == -1) {
		perror("fork() failed");
		exit(1);
	} else if (pid == 0) {
		signal(SIGINT, SIG_DFL);
		signal(SIGQUIT, SIG_DFL);

		execvp(arglist[0], arglist);
		perror("execvp() failed");
		exit(1);
	}

	/* parent go here */
	while (1) {
		if (wait(&exit_status) == pid)
			break;
	}
	printf("Child exited with status %d, %d\n",
		exit_status >> 8, exit_status & 0x7F);

	return exit_status;
}


