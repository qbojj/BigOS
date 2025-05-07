#ifndef _DEBUG_STDIO_
#define _DEBUG_STDIO_

#include <stdio.h>

#ifndef NDEBUG

	#define DEBUG_PUTC(c)		   fputc(c, stderr)
	#define DEBUG_PUTS(s)		   fputs(s, stderr)
	#define DEBUG_PRINTF(fmt, ...) fprintf(stderr, fmt __VA_OPT__(, ) __VA_ARGS__)

#else

// for argument checking
[[gnu::format(printf, 1, 2)]] static inline void noop_printf(const char*, ...) {
}

	#define DEBUG_PUTC(c)
	#define DEBUG_PUTS(s)
	#define DEBUG_PRINTF(fmt, ...) noop_printf(fmt __VA_OPT__(, ) __VA_ARGS__)

#endif
#endif
