/*
 *
 */

#ifndef C_LOG_H
#define C_LOG_H


/* 日志文件路径长度的最大值 */
#define LOG_PATH_LEN_MAX	128
/* 日志文件名长度的最大值 */
#define LOG_NAME_LEN_MAX	32

#define LOG_MODE			"LOG_MODE"
#define LOG_SWITCH_MODE		"LOG_SWITCH_MODE"
#define LOG_FILE_PATH		"LOG_FILE_PATH"
#define LOG_SIZE			"LOG_SIZE"
#define LOG_SIZE_DEFAULT	5


/* 日志切换模式，按文件大小或按日期 */
#define LOG_SWITCH_MODE_SIZE	1
#define LOG_SWITCH_MODE_DATE	2

/* */
#define LOG_MODE_OFF		0
#define LOG_MODE_DEBUG		1
#define LOG_MODE_INFO		2
#define LOG_MODE_WARNING	3
#define LOG_MODE_ERROR		4
#define LOG_MODE_CRITICAL	5

#define LOG_OFF			LOG_MODE_OFF,		__FILE__, __LINE__
#define LOG_DEBUG		LOG_MODE_DEBUG,		__FILE__, __LINE__
#define LOG_INFO		LOG_MODE_INFO,		__FILE__, __LINE__
#define LOG_WARNING		LOG_MODE_WARNING,	__FILE__, __LINE__
#define LOG_ERROR		LOG_MODE_ERROR,		__FILE__, __LINE__
#define LOG_CRITICAL	LOG_MODE_CRITICAL,	__FILE__, __LINE__


/**/
int WriteLog(int log_mode, char *filename, int line_num, char *fmt, ...);

#endif /* C_LOG_H */
