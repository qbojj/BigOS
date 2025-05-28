/******************************************************************************
 *
 *  File:			bootloader/log.c
 *  Author:			Maciej Zgierski
 *
 ******************************************************************************/

#include "log.h"

#include <efi.h>
#include <efilib.h>
#include <efidef.h>
#include <efistdarg.h>

#define LOG_INDENT 4

static UINTN procedure_level = 0;

void prefix(CHAR16 icon) {
	for(UINTN i = 0; i < procedure_level * LOG_INDENT; ++i)
		Print(L" ");
	Print(L" [%lc] ", icon);
}

void log(const CHAR16* message, ...) {
	va_list args;

	prefix(' ');

	va_start(args, message);
	VPrint(message, args);
	va_end(args);

	Print(L"\n");
}

void err(const CHAR16* message, ...) {
	va_list args;

	prefix('X');

	va_start(args, message);
	VPrint(message, args);
	va_end(args);

	Print(L"\n");
}

void log_procedure_start() {
	procedure_level++;
}

void log_procedure_end() {
	procedure_level--;
}
