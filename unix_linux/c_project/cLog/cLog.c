/*
 * 第一种: 每次写日志时，都会从环境变量获取日志的配置模式，比如设置的
 *         等级，日志切换的形式、大小等。接着打开日志文件，追加内容，
 *         刷新缓存，最后关闭文件。
 *         这种模式每次写日志都要打开关闭文件，会导致一定的开销，在测试
 *         环境没所谓，在生产环境一定要注意日志文件的等级设置，不要频繁
 *         的写入调试日志，否则会影响性能。
 *
 * 
 */


#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>
#include <time.h>
#include <sys/timeb.h>
#include "cLog.h"


/*
 * check switch mode
 * if switch by date:
 *    check date
 * if switch by size:
 *    check file size
 * write to log file
 */
FILE *open_log_file()
{
	FILE *fp = fopen("/tmp/hwq.log", "a+");
	return fp;
}


/*
 *
 */
int WriteLog(int log_mode, char *filename, int line_num, char *fmt, ...)
{
	int param_log_mode;
	int param_log_switch_mode;
	int param_log_size;
	char param_log_file_path[LOG_PATH_LEN_MAX+1];
	struct timeb tb;
	char log_time[32];
	char date_time[16];
	char millitm[4];
	struct tm *local_tm;
	char *penv;
	FILE *fp;
	va_list ap;

	/* get from env */
	penv = getenv(LOG_MODE);
	param_log_mode = penv == NULL ? 0 : atoi(penv);
	if (param_log_mode == 0)
		param_log_mode = LOG_MODE_OFF;

	penv = getenv(LOG_SWITCH_MODE);
	param_log_switch_mode = penv == NULL ? 0 : atoi(penv);
	if (param_log_switch_mode == 0)
		param_log_switch_mode = LOG_SWITCH_MODE_SIZE;

	penv = getenv(LOG_SIZE);
	param_log_size = penv == NULL ? 0 : atoi(penv);
	if (param_log_size == 0)
		param_log_size = LOG_SIZE_DEFAULT;

	memset(param_log_file_path, 0, sizeof(param_log_file_path));
	penv = getenv(LOG_FILE_PATH);
	if (penv)
		strncpy(param_log_file_path, penv, LOG_PATH_LEN_MAX);
	else
		strncpy(param_log_file_path, ".", LOG_PATH_LEN_MAX);

	/* get current time */
	memset(log_time, 0, sizeof(log_time));
	memset(date_time, 0, sizeof(date_time));
	ftime(&tb);
	snprintf(millitm, sizeof(millitm), "%03d", tb.millitm);
	local_tm = localtime(&tb.time);
	strftime(log_time, sizeof(log_time), "%Y-%m-%d %H:%M:%S", local_tm);
	strcat(log_time, ":");
	strcat(log_time, millitm);
	strftime(date_time, sizeof(date_time), "%Y%m%d%H%M%S", local_tm);

	/* check whether the msg should be saved in log file */
	if (param_log_mode == LOG_MODE_OFF || log_mode < param_log_mode)
		return 0;

	/* open log file */
	//if (strlen(log_file_name) == 0)
	//	fp = open_log_file();
	//else
		fp = open_log_file();
	if (fp == NULL)
		return -1;

	/* write msg to log file */
	//fprintf(fp, "[%s][%s][%d]", log_time, filename, line_num);
	switch (log_mode) {
		case LOG_MODE_DEBUG:
			fprintf(fp, "[%s][%s][%d][DEBUG]: ", log_time, filename, line_num);
			break;
		case LOG_MODE_INFO:
			fprintf(fp, "[%s][%s][%d][INFO]: ", log_time, filename, line_num);
			break;
		case LOG_MODE_WARNING:
			fprintf(fp, "[%s][%s][%d][WARNING]: ", log_time, filename, line_num);
			break;
		case LOG_MODE_ERROR:
			fprintf(fp, "[%s][%s][%d][ERROR]: ", log_time, filename, line_num);
			break;
		case LOG_MODE_CRITICAL:
			fprintf(fp, "[%s][%s][%d][CRITICAL]: ", log_time, filename, line_num);
			break;
		default:
			fprintf(fp, "[%s][%s][%d]: ", log_time, filename, line_num);
			break;
	}

	va_start(ap, fmt);
	vfprintf(fp, fmt, ap);
	va_end(ap);

	fprintf(fp, "\n");
	fflush(fp);
	fclose(fp);

	return 0;
}
