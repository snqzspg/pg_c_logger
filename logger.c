/**
 * @file logger.c
 * @author Snqzs' PG (snqzspg@gmail.com)
 * @brief 
 * @version 1.1
 * @date 2026-04-24
 * 
 * @copyright Snqzs' PG (c) 2026
 * 
 * BSD Zero Clause License
 * 
 * Copyright (c) 2026 Snqzs' PG
 * 
 * Permission to use, copy, modify, and/or distribute this software for any
 * purpose with or without fee is hereby granted.
 * 
 * THE SOFTWARE IS PROVIDED "AS IS" AND THE AUTHOR DISCLAIMS ALL WARRANTIES WITH
 * REGARD TO THIS SOFTWARE INCLUDING ALL IMPLIED WARRANTIES OF MERCHANTABILITY
 * AND FITNESS. IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR ANY SPECIAL, DIRECT,
 * INDIRECT, OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES WHATSOEVER RESULTING FROM
 * LOSS OF USE, DATA OR PROFITS, WHETHER IN AN ACTION OF CONTRACT, NEGLIGENCE OR
 * OTHER TORTIOUS ACTION, ARISING OUT OF OR IN CONNECTION WITH THE USE OR
 * PERFORMANCE OF THIS SOFTWARE.
 */

#include <stdarg.h>
#include <stdio.h>
#include <string.h>

#ifdef USE_SYSCALL
#ifdef __linux__
#include <sys/syscall.h>
#include <unistd.h>
#endif // __linux__
#endif // USE_SYSCALL

#include "logger.h"

struct logging_config_s current_config = {
	.level      = NOTE,
	.show_color = 0
};

const char BLANK_LABEL   [] = "NOTSET";
const char DEBUG_LABEL   [] = "DEBUG";
const char INFO_LABEL    [] = "INFO";
const char NOTE_LABEL    [] = "NOTE";
const char WARNING_LABEL [] = "WARNING";
const char ERROR_LABEL   [] = "ERROR";
const char CRITICAL_LABEL[] = "CRITICAL";

const char BLANK_COLOURED_LABEL   [] = "\033[1;35mNOTSET\033[0m";
const char DEBUG_COLOURED_LABEL   [] = "\033[1;32mDEBUG\033[0m";
const char INFO_COLOURED_LABEL    [] = "\033[1;34mINFO\033[0m";
const char NOTE_COLOURED_LABEL    [] = "\033[1;36mNOTE\033[0m";
const char WARNING_COLOURED_LABEL [] = "\033[1;33mWARNING\033[0m";
const char ERROR_COLOURED_LABEL   [] = "\033[1;31mERROR\033[0m";
const char CRITICAL_COLOURED_LABEL[] = "\033[1;31mCRITICAL\033[0m";

static const char* get_logging_label(enum logging_level logging_type) {
	switch (logging_type) {
	case DEBUG:
		return current_config.show_color
			? DEBUG_COLOURED_LABEL    : DEBUG_LABEL;
	case INFO:
		return current_config.show_color
			? INFO_COLOURED_LABEL     : INFO_LABEL;
	case NOTE:
		return current_config.show_color
			? NOTE_COLOURED_LABEL     : NOTE_LABEL;
	case WARNING:
		return current_config.show_color
			? WARNING_COLOURED_LABEL  : WARNING_LABEL;
	case ERROR:
		return current_config.show_color
			? ERROR_COLOURED_LABEL    : ERROR_LABEL;
	case CRITICAL:
		return current_config.show_color
			? CRITICAL_COLOURED_LABEL : CRITICAL_LABEL;
	}

	return BLANK_LABEL;
}

int is_logging_type_enabled(enum logging_level type) {
    return type >= current_config.level;
}

#ifdef USE_SYSCALL
static int __exec_write_stderr_syscall(const char* s, int slen) {
	return syscall(SYS_write, STDERR_FILENO, s, slen);
}
#endif // USE_SYSCALL

static int logging_vprintf(
	enum logging_level logging_type,
	const char*        fmt,
	va_list            args
) {
	if (!is_logging_type_enabled(logging_type)) {
		return 0;
	}

	const char* label               = get_logging_label(logging_type);
	const int   extended_fmt_length = snprintf(NULL, 0, "[%s]%s", label, fmt);

	char  extended_fmt[extended_fmt_length + 1];
	(void) snprintf(
		extended_fmt,
		extended_fmt_length + 1,
		"[%s]%s",
		label,
		fmt
	);

#ifdef USE_SYSCALL
	va_list args_clone;
	va_copy(args_clone, args);

	int print_s_len = vsnprintf(NULL, 0, extended_fmt, args_clone);

	va_end(args_clone);

	char print_s[print_s_len + 1];
	int r = vsnprintf(print_s, print_s_len + 1, extended_fmt, args);

	int sr = __exec_write_stderr_syscall(print_s, print_s_len);
	if (sr != 0) {
		r = sr;
	}
	return r;
#else
	return vfprintf(stderr, extended_fmt, args);
#endif // USE_SYSCALL
}

void logging_perror(enum logging_level logging_type, const char* prefix) {
	if (!is_logging_type_enabled(logging_type)) {
		return;
	}

	const char* label = get_logging_label(logging_type);
	size_t l = snprintf(NULL, 0, "[%s]%s", label, prefix);
	char error_pfx[l + 1];
	(void) snprintf(error_pfx, l + 1, "[%s]%s", label, prefix);
	perror(error_pfx);
}

void config_logging(struct logging_config_s config) {
	current_config = config;
}

int debug_printf(const char* fmt, ...) {
	va_list args;
	va_start(args, fmt);
	int ret = logging_vprintf(DEBUG, fmt, args);
	va_end(args);
	return ret;
}

void debug_perror(const char* prefix) {
	logging_perror(DEBUG, prefix);
}

int info_printf(const char* fmt, ...) {
	va_list args;
	va_start(args, fmt);
	int ret = logging_vprintf(INFO, fmt, args);
	va_end(args);
	return ret;
}

void info_perror(const char* prefix) {
	logging_perror(INFO, prefix);
}

int note_printf(const char* fmt, ...) {
	va_list args;
	va_start(args, fmt);
	int ret = logging_vprintf(NOTE, fmt, args);
	va_end(args);
	return ret;
}

void note_perror(const char* prefix) {
	logging_perror(NOTE, prefix);
}

int warning_printf(const char* fmt, ...) {
	va_list args;
	va_start(args, fmt);
	int ret = logging_vprintf(WARNING, fmt, args);
	va_end(args);
	return ret;
}

void warning_perror(const char* prefix) {
	logging_perror(WARNING, prefix);
}

int error_printf(const char* fmt, ...) {
	va_list args;
	va_start(args, fmt);
	int ret = logging_vprintf(ERROR, fmt, args);
	va_end(args);
	return ret;
}

void error_perror(const char* prefix) {
	logging_perror(ERROR, prefix);
}

int critical_printf(const char* fmt, ...) {
	va_list args;
	va_start(args, fmt);
	int ret = logging_vprintf(CRITICAL, fmt, args);
	va_end(args);
	return ret;
}

void critical_perror(const char* prefix) {
	logging_perror(CRITICAL, prefix);
}
