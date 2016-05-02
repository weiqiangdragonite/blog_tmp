
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <stdarg.h>
#include <errno.h>
#include <string.h>


#define BUF_SIZE 1024
char log_buf[BUF_SIZE];

void
write_log_file(int status, const char *msg)
{
	char log_buf[BUF_SIZE];

	/* 需要用mutex吗 */
	//snprintf(log_buf, BUF_SIZE, "[%s] [%s] %s\n", get_current_time(NULL),
	//	(status == 0) ? "OK" : "ERROR", msg);
	//write(log_fd, log_buf, strlen(log_buf));
}


static void
output_error_msg(const char *format, va_list ap)
{
	char msg[BUF_SIZE];

	vsnprintf(msg, sizeof(msg), format, ap);

	/* Flush any pending stdout */
	fflush(stdout);
	fputs(msg, stderr);
	/* In case stderr is not line-buffered */
	fflush(stderr);

	/* Write to log file */
	write_log_file(-1, msg);
}

/*
 *
 */
void
error_exit(const char *format, ...)
{
	va_list arg_list;
	va_start(arg_list, format);

	output_error_msg(format, arg_list);

	exit(EXIT_FAILURE);
}

/*
 *
 */
void
error_msg(const char *format, ...)
{
	va_list arg_list;
	va_start(arg_list, format);

	output_error_msg(format, arg_list);
}


int
main(void)
{
	error_msg("send data failed: %s\n", "open error");
	return 0;
}

