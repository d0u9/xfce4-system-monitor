/**
 * licensed under GPL 2.0
 * @d0u9
 */
#include "log.h"

const char *__log_level_str[] = {
				LOG_EMERG_STR,
				LOG_ALERT_STR,
				LOG_CRIT_STR,
				LOG_ERR_STR,
				LOG_WARNING_STR,
				LOG_NOTICE_STR,
				LOG_INFO_STR,
				LOG_DEBUG_STR,
				};
#ifndef LOG_DIS_COLOR
#include "color.h"
#define CRL_PRE	COLOR_START COLOR_BOLD_STR ";"
const char *__log_level_str_color[] = {
	CRL_PRE COLOR_BLACK_STR ";" COLOR_ON_RED_STR "m" "EMERG " COLOR_END,
	CRL_PRE COLOR_CYAN_STR ";" COLOR_ON_RED_STR "m" "ALERT " COLOR_END,
	CRL_PRE COLOR_GREEN_STR ";" COLOR_ON_RED_STR "m" "CRIT  " COLOR_END,
	CRL_PRE COLOR_RED_STR ";" COLOR_ON_BLACK_STR "m" "ERR   " COLOR_END,
	CRL_PRE COLOR_YELLOW_STR ";" COLOR_ON_BLACK_STR "m" "WARN  " COLOR_END,
	CRL_PRE COLOR_CYAN_STR ";" COLOR_ON_BLACK_STR "m" "NOTICE" COLOR_END,
	CRL_PRE COLOR_GREEN_STR ";" COLOR_ON_BLACK_STR "m" "INFO  " COLOR_END,
	CRL_PRE COLOR_WHITE_STR ";" COLOR_ON_BLACK_STR "m" "DEBUG " COLOR_END,
				};
#undef CRL_PRE
#endif /* LOG_DIS_COLOR */

#ifdef LOG_EN_FILE_LOG
FILE *__log_file_fp = NULL;
#endif

#ifdef LOG_EN_FILE_LOG
int init_log(const char *path, const char *mode)
{
	__log_file_fp = fopen(path, mode);
	return 0;
}
#endif


#ifdef LOG_EN_FILE_LOG
void exit_log(void)
{
	fclose(__log_file_fp);
}
#endif


