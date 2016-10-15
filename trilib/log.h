/**
 * licensed under GPL 2.0
 * @d0u9
 */
#ifndef _TRI_LOG_H
#define _TRI_LOG_H

#include <stdarg.h>
#include <stdio.h>
#include <time.h>
#include <string.h>
#include <sys/types.h>
#include <unistd.h>
#include <pthread.h>

#include "core.h"
#include "log_level.h"

#define MAX_LOG_BUFFER	512

#ifndef LOG_EN_PREFIX_PID
# define LOG_DIS_PREFIX_PID	/* by default, prefix pid is disabled */
#endif

/* by default, prefix tid is disbaled, it is largely due to the fact that the
 * method of obtaining tid is varied between libraries.
 *
 * if you need this info, in addition to uncomment the below line, you should
 * implement the get_tid() funtion as well.
 */
#ifndef LOG_EN_PREFIX_TID
# define LOG_DIS_PREFIX_TID
#endif

#ifndef LOG_LEVEL
# ifdef DEBUG
#  define LOG_LEVEL	LOG_DEBUG
# else
#  define LOG_LEVEL	LOG_INFO
# endif
#endif

#ifndef LOG_STD_FP
#define LOG_STD_FP	stdout
#endif

#ifndef LOG_STD_LEVEL
# define LOG_STD_LEVEL	LOG_LEVEL
#endif

#ifndef LOG_FILE_LEVEL
# define LOG_FILE_LEVEL	LOG_LEVEL
#endif

#ifdef LOG_DIS_PREFIX_TIME
#define LOG_DIS_STD_PREFIX_TIME
#define LOG_DIS_FILE_PREFIX_TIME
#endif

#ifdef LOG_DIS_PREFIX_FILE
#define LOG_DIS_STD_PREFIX_FILE
#define LOG_DIS_FILE_PREFIX_FILE
#endif

#ifdef LOG_DIS_PREFIX_LINE
#define LOG_DIS_STD_PREFIX_LINE
#define LOG_DIS_FILE_PREFIX_LINE
#endif

#ifdef LOG_DIS_PREFIX_LEVEL
#define LOG_DIS_STD_PREFIX_LEVEL
#define LOG_DIS_FILE_PREFIX_LEVEL
#endif

#ifdef LOG_DIS_PREFIX_PID
#define LOG_DIS_STD_PREFIX_PID
#define LOG_DIS_FILE_PREFIX_PID
#endif

#ifdef LOG_DIS_PREFIX_TID
#define LOG_DIS_STD_PREFIX_TID
#define LOG_DIS_FILE_PREFIX_TID
#endif

#define printl(level, fmt, ...)					\
	do {							\
		__printl(level, __stringify(__LINE__),		\
			 __FILE__, fmt, ##__VA_ARGS__);		\
	} while(0)

#define printl_emerg(fmt, ...)	\
	printl(LOG_EMERG, fmt, ##__VA_ARGS__)
#define printl_alert(fmt, ...)	\
	printl(LOG_ALERT, fmt, ##__VA_ARGS__)
#define printl_crit(fmt, ...)	\
	printl(LOG_CRIT, fmt, ##__VA_ARGS__)
#define printl_err(fmt, ...)	\
	printl(LOG_ERR, fmt, ##__VA_ARGS__)
#define printl_warning(fmt, ...)	\
	printl(LOG_WARNING, fmt, ##__VA_ARGS__)
#define printl_notice(fmt, ...)	\
	printl(LOG_NOTICE, fmt, ##__VA_ARGS__)
#define printl_info(fmt, ...)	\
	printl(LOG_INFO, fmt, ##__VA_ARGS__)
#define printl_debug(fmt, ...)	\
	printl(LOG_DEBUG, fmt, ##__VA_ARGS__)

extern const char *__log_level_str[];

#ifndef LOG_DIS_COLOR
extern const char *__log_level_str_color[];
#endif

#ifdef LOG_EN_FILE_LOG
extern FILE *__log_file_fp;
extern int init_log(const char *path, const char *mode);
extern void exit_log(void);
#else
static inline int init_log(const char *path, const char *mode) { return 0;}
static inline void exit_log(void) {}
#endif


#ifndef LOG_DIS_PREFIX_TID
static inline void get_tid(char *buf, int len)
{
	pthread_t tid = pthread_self();
	snprintf(buf, len, "%lu", (unsigned long)tid);
	return;
}
#endif

#ifndef LOG_DIS_PREFIX_TIME
/* keep in mind that the size of buf is at least 16 bytes */
static inline int get_time(char *buf)
{
	time_t t = time(NULL);
	int ret = 0;
	ret = strftime(buf, 16, "%b %d %T", localtime(&t));
	return (!ret ? -1 : ret);
}
#endif

static inline
int make_symbol(char *buf, int len, int color, int bold,
		const char *l_delimiter, const char *r_delimiter,
		const char *content, const char *fmt)
{
	int actual_len  = 0;
	actual_len = strnlen(content, 32) +
		     strnlen(l_delimiter, 4) +
		     strnlen(r_delimiter, 4);

	if (!buf) return actual_len;

	if (actual_len > len) return -1;

	char fmt_buf[32];
	snprintf(fmt_buf, 32, "%%s%s%%s", fmt);
	snprintf(buf, len, fmt_buf, l_delimiter, content, r_delimiter);

	return actual_len;
}

#ifdef LOG_EN_FILE_LOG
static inline
void __printl_file(char *msg, const char *level,
		  const char *line, const char *file, const char *cur_time,
		  const char *pid, const char *tid,
		  const char *fmt, va_list args)
{
#ifdef LOG_DIS_FILE_PREFIX_LEVEL
	const char *pre_level = "";
#else
# ifdef LOG_EN_FILE_LEVEL_NUM
	char pre_level[5];
	make_symbol(pre_level, 7, 0, 0, "[", "] ", level, "%s");
# else
	char pre_level[10];
	make_symbol(pre_level, 10, 0, 0, "[", "] ",
		    __log_level_str[LOG_LEVEL_INT(level)], "%-6s");
# endif
#endif

#ifdef LOG_DIS_FILE_PREFIX_TIME
	const char *pre_time = "";
#else
	char pre_time[17];
	make_symbol(pre_time, 17, 0, 0, "", " ", cur_time, "%s");
#endif

#ifdef LOG_DIS_FILE_PREFIX_FILE
	const char *pre_file = "";
#else
	char pre_file[128];
	make_symbol(pre_file, 128, 0, 0, "", " ", file, "%-s");
#endif

#ifdef LOG_DIS_FILE_PREFIX_LINE
	const char *pre_line = "";
#else
# ifdef LOG_DIS_FILE_PREFIX_FILE
	char pre_line[5];
	make_symbol(pre_line, 128, 0, 0, "", " ", line, "%-4s");
# else
	char pre_line[5];
	make_symbol(pre_line, 128, 0, 0, "", "@", line, "%4s");
# endif
#endif

#ifdef LOG_DIS_FILE_PREFIX_PID
	const char *pre_pid = "";
#else
	char pre_pid[32];
	make_symbol(pre_pid, 32, 0, 0, "pid:", " ", pid, "%s");
#endif

#ifdef LOG_DIS_FILE_PREFIX_TID
	const char *pre_tid = "";
#else
	char pre_tid[32];
	make_symbol(pre_tid, 32, 0, 0, "tid:", " ", tid, "%s");
#endif

	snprintf(msg, MAX_LOG_BUFFER, "%s%s%s%s%s%s-> %s",
		 pre_level, pre_time, pre_pid, pre_tid, pre_line, pre_file, fmt);
	vfprintf(__log_file_fp, msg, args);
	fflush(__log_file_fp);
}
#endif /* LOG_DIS_FILE_LOG */

#ifndef LOG_DIS_STD_LOG
static inline
void __printl_std(char *msg, const char *level,
		  const char *line, const char *file, const char *cur_time,
		  const char *pid, const char *tid,
		  const char *fmt, va_list args)
{
#ifdef LOG_DIS_STD_PREFIX_LEVEL
	const char *pre_level = "";
#else
# ifdef LOG_EN_STD_LEVEL_NUM
	char pre_level[5];
	make_symbol(pre_level, 7, 0, 0, "[", "] ", level, "%s");
# else
#  ifdef LOG_DIS_COLOR
	char pre_level[10];
	make_symbol(pre_level, 10, 0, 0, "[", "] ",
		    __log_level_str[LOG_LEVEL_INT(level)], "%-6s");
#  else

	char pre_level[32];
	make_symbol(pre_level, 32, 0, 0, "", " ",
		    __log_level_str_color[LOG_LEVEL_INT(level)], "%-20s");
#  endif
# endif
#endif

#ifdef LOG_DIS_STD_PREFIX_TIME
	const char *pre_time = "";
#else
	char pre_time[17];
	make_symbol(pre_time, 17, 0, 0, "", " ", cur_time, "%s");
#endif

#ifdef LOG_DIS_STD_PREFIX_FILE
	const char *pre_file = "";
#else
	char pre_file[128];
	make_symbol(pre_file, 128, 0, 0, "", " ", file, "%-s");
#endif

#ifdef LOG_DIS_STD_PREFIX_LINE
	const char *pre_line = "";
#else
# ifdef LOG_DIS_STD_PREFIX_FILE
	char pre_line[5];
	make_symbol(pre_line, 128, 0, 0, "", " ", line, "%-4s");
# else
	char pre_line[5];
	make_symbol(pre_line, 128, 0, 0, "", "@", line, "%4s");
# endif
#endif

#ifdef LOG_DIS_STD_PREFIX_PID
	const char *pre_pid = "";
#else
	char pre_pid[32];
	make_symbol(pre_pid, 32, 0, 0, "pid:", " ", pid, "%-5s");
#endif

#ifdef LOG_DIS_STD_PREFIX_TID
	const char *pre_tid = "";
#else
	char pre_tid[32];
	make_symbol(pre_tid, 32, 0, 0, "tid:", " ", tid, "%s");
#endif

	snprintf(msg, MAX_LOG_BUFFER, "%s%s%s%s%s%s-> %s",
		 pre_level, pre_time, pre_pid, pre_tid, pre_line, pre_file, fmt);
	vfprintf(LOG_STD_FP, msg, args);
	fflush(LOG_STD_FP);
}
#endif /* LOG_DIS_STD_LOG */


static inline void __printl(const char *level, const char *line,
			    const char *file, const char *fmt, ...)
{
	char msg_buff[MAX_LOG_BUFFER];
	char *msg = msg_buff;
	va_list args;

#ifdef LOG_DIS_PREFIX_TIME
	const char *cur_time = "";
#else
	char cur_time[16] = {0};
	get_time(cur_time);
#endif

#ifdef LOG_DIS_PREFIX_PID
	const char *pid = "";
#else
	char pid[32] = {0};
	pid_t pid_i = getpid();
	snprintf(pid, 32, "%lu", (unsigned long)pid_i);
#endif

#ifdef LOG_DIS_PREFIX_TID
	const char *tid = "";
#else
	char tid[32]= {0};
	get_tid(tid, 32);
#endif

#ifndef LOG_DIS_STD_LOG
	va_start(args, fmt);
	if (LOG_LEVEL_INT(level) <= LOG_LEVEL_INT(LOG_STD_LEVEL)) {
		__printl_std(msg, level, line, file, cur_time, pid, tid, fmt, args);
	}
	va_end(args);
#endif

#ifdef LOG_EN_FILE_LOG
	va_start(args, fmt);
	if (LOG_LEVEL_INT(level) <= LOG_LEVEL_INT(LOG_FILE_LEVEL)) {
		__printl_file(msg, level, line, file, cur_time, pid, tid, fmt, args);
	}
	va_end(args);

#endif

}


#endif /* _TRI_LOGGER_H */
