/*
 * use exec to create a new process ps, same as shell:
 * ps -ax
 */

#include <unistd.h>

/* example of an argument list */
char * const ps_argv[] = {"ps", "ax", NULL};

/* example environment, not terribly useful */
char * const ps_envp[] = {"PATH=/bin:/usr/bin", "TERM=console", NULL};

/* exec functions */
execl("/bin/ps", "ps", "ax", NULL);
execlp("ps", "ps", "ax", NULL);
execle("/bin/ps", "ps", "ax", NULL, ps_envp);

execv("/bin/ps", ps_argv);
execvp("ps", ps_argv);
execve("/bin/ps", ps_argv, ps_envp);




