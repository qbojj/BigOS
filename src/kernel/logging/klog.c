#include "klog.h"

#include <stdarg.h>
#include <stdbigos/types.h>

// NOTE: In the future this lib should manage logging (formating and outputing to the screen or uart) on its own.
//  Right now I will leave it as is, will change once the basic kernel at least runs.
#include "debug/debug_stdio.h"

static u32 g_indent_level = 0;
static const char* g_prefixes[] = {"[ERROR]", "[WARNING]", "[ ]", "[~]"};

void klog_indent_increase() {
	++g_indent_level;
}

void klog_indent_decrease() {
	if (g_indent_level != 0)
		--g_indent_level;
}

void klog(klog_severity_level_t loglvl, const char* fmt, ...) {
	(void)loglvl;
	(void)fmt;
	va_list args;
	va_start(args, fmt);
	if (loglvl > KLSL_TRACE) {
		KLOGLN_ERROR("Invalid loglvl passed to klog");
		return;
	}
	DEBUG_PUTGAP(g_indent_level);
	DEBUG_PRINTF("%s ", g_prefixes[loglvl]);
	DEBUG_VPRINTF(fmt, args);
	va_end(args);
}
