#pragma once

#include <cstdio>
#include <cstdarg>

/* 
 * Wrappers for logging with variable arguments.
 * see:
 * (var arg wrapper) https://www.zhihu.com/question/51102741
 * and
 * (colored output) https://blog.csdn.net/yetyongjin/article/details/120972642
 */

extern const int loglevel;
#define LOGLV_ERROR 0
#define LOGLV_WARNING 1
#define LOGLV_INFO 2
#define LOGLV_VERBOSE 3 
#define DEFAULT_LOGLEVEL LOGLV_INFO

#define TTY_INTERACT

#ifdef TTY_INTERACT
/* if tty is available, use colorful logging to convey more information. */

#define info(fmt) \
	if (loglevel >= LOGLV_INFO) \
	printf("\033[32m[INFO] \033[0m" fmt);

#define verbose(fmt) \
	if (loglevel >= LOGLV_VERBOSE) \
	printf("\033[34m[VERBOSE] \033[0m" fmt);

#define warning(fmt) \
	if (loglevel >= LOGLV_WARNING) \
	printf("\033[33m[WARNING] \033[0m" fmt);
	
#define error(fmt) \
	if (loglevel >= LOGLV_ERROR) \
	fprintf(stderr, "\033[31m[ERROR] \033[0m" fmt);
	
#define infof(fmt, ...) \
	if (loglevel >= LOGLV_INFO) \
	printf("\033[32m[INFO] \033[0m" fmt, __VA_ARGS__);

#define verbosef(fmt, ...) \
	if (loglevel >= LOGLV_VERBOSE) \
	printf("\033[34m[VERBOSE] \033[0m" fmt, __VA_ARGS__);

#define warningf(fmt, ...) \
	if (loglevel >= LOGLV_WARNING) \
	printf("\033[33m[WARNING] \033[0m" fmt, __VA_ARGS__);
	
#define errorf(fmt, ...) \
	if (loglevel >= LOGLV_ERROR) \
	fprintf(stderr, "\033[31m[ERROR] \033[0m" fmt, __VA_ARGS__);

#else /* TTY_INTERACT */
/* plain logging, which is suitable for being redirect to file output. */

#define info(fmt) \
	if (loglevel >= LOGLV_INFO) \
	printf("[INFO] " fmt);

#define verbose(fmt) \
	if (loglevel >= LOGLV_VERBOSE) \
	printf("[VERBOSE] " fmt);

#define warning(fmt) \
	if (loglevel >= LOGLV_WARNING) \
	printf("[WARNING] " fmt);
	
#define error(fmt) \
	if (loglevel >= LOGLV_ERROR) \
	printf("[ERROR] " fmt);
	
#define infof(fmt, ...) \
	if (loglevel >= LOGLV_INFO) \
	printf("[INFO] " fmt, __VA_ARGS__);

#define verbosef(fmt, ...) \
	if (loglevel >= LOGLV_VERBOSE) \
	printf("[VERBOSE] " fmt, __VA_ARGS__);

#define warningf(fmt, ...) \
	if (loglevel >= LOGLV_WARNING) \
	printf("[WARNING] " fmt, __VA_ARGS__);
	
#define errorf(fmt, ...) \
	if (loglevel >= LOGLV_ERROR) \
	printf("[ERROR] " fmt, __VA_ARGS__);

#endif /* TTY_INTERACT */