/**
 * @file logger.c
 * @author Snqzs' PG (snqzspg@gmail.com)
 * @brief 
 * @version 1.2
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
#include <stdlib.h>
#include <string.h>

#ifdef USE_SYSCALL
#ifdef __linux__
#include <sys/syscall.h>
#include <unistd.h>
#endif // __linux__
#endif // USE_SYSCALL

#include "logger.h"

struct logging_config_s current_config = {
	.level      = LOGGER_NOTE,
	.show_color = 0
};

static char*  __fmtted_txt_buffer  = NULL;
static size_t __fmtted_txt_buf_cap = 0;

#ifdef USE_SYSCALL
static char*  __lbled_txt_buffer  = NULL;
static size_t __lbled_txt_buf_cap = 0;
#endif // USE_SYSCALL


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
	case LOGGER_DEBUG:
		return current_config.show_color
			? DEBUG_COLOURED_LABEL    : DEBUG_LABEL;
	case LOGGER_INFO:
		return current_config.show_color
			? INFO_COLOURED_LABEL     : INFO_LABEL;
	case LOGGER_NOTE:
		return current_config.show_color
			? NOTE_COLOURED_LABEL     : NOTE_LABEL;
	case LOGGER_WARNING:
		return current_config.show_color
			? WARNING_COLOURED_LABEL  : WARNING_LABEL;
	case LOGGER_ERROR:
		return current_config.show_color
			? ERROR_COLOURED_LABEL    : ERROR_LABEL;
	case LOGGER_CRITICAL:
		return current_config.show_color
			? CRITICAL_COLOURED_LABEL : CRITICAL_LABEL;
	}

	return BLANK_LABEL;
}

int is_logging_type_enabled(enum logging_level type) {
    return type >= current_config.level;
}

static void __expand_fmtted_buf_memory(size_t new_cap_fmtted) {
	if (new_cap_fmtted <= __fmtted_txt_buf_cap) {
		return;
	}

	char* new_mem;

	if (__fmtted_txt_buffer == NULL) {
		new_mem = (char*) malloc(new_cap_fmtted * sizeof(char));
	} else {
		new_mem = (char*) realloc(
			__fmtted_txt_buffer,
			new_cap_fmtted * sizeof(char)
		);
	}

	if (new_mem != NULL) {
		__fmtted_txt_buffer  = new_mem;
		__fmtted_txt_buf_cap = new_cap_fmtted;
	}
}

#ifdef USE_SYSCALL
static void __expand_lbled_buf_memory(size_t new_cap) {
	if (new_cap <= __lbled_txt_buf_cap) {
		return;
	}

	char* new_mem;

	if (__lbled_txt_buffer == NULL) {
		new_mem = (char*) malloc(new_cap * sizeof(char));
	} else {
		new_mem = (char*) realloc(__lbled_txt_buffer, new_cap * sizeof(char));
	}

	if (new_mem != NULL) {
		__lbled_txt_buffer  = new_mem;
		__lbled_txt_buf_cap = new_cap;
	}
}
#endif // USE_SYSCALL

// Includes a space
static const char __open_brackets[] = "([{< ";

static int __is_an_open_bracket(char c) {
	for (const char* c1 = __open_brackets; *c1 != '\0'; c1++) {
		if (c == *c1) {
			return 1;
		}
	}

	return 0;
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

	va_list args_clone;
	va_copy(args_clone, args);

	const char* label        = get_logging_label(logging_type);
	const int fmtted_str_len = vsnprintf(NULL, 0, fmt, args_clone);

	va_end(args_clone);

	__expand_fmtted_buf_memory(fmtted_str_len + 1);

	if (__fmtted_txt_buf_cap < (size_t) fmtted_str_len + 1) {
		return -1;
	}

	char* fmtted_str = __fmtted_txt_buffer;
	int r = vsnprintf(fmtted_str, __fmtted_txt_buf_cap, fmt, args);

#ifdef USE_SYSCALL
	int labelled_text_len = snprintf(
		NULL,
		0,
		"[%s]%s%s",
		label,
		__is_an_open_bracket(fmtted_str[0]) ? "" : " ",
		fmtted_str
	);

	__expand_lbled_buf_memory(labelled_text_len + 1);
	if (__lbled_txt_buf_cap < (size_t) labelled_text_len + 1) {
		return -1;
	}

	(void) snprintf(
		__lbled_txt_buffer,
		__lbled_txt_buf_cap,
		"[%s]%s%s",
		label,
		__is_an_open_bracket(fmtted_str[0]) ? "" : " ",
		fmtted_str
	);

	int sr = __exec_write_stderr_syscall(__lbled_txt_buffer, labelled_text_len);
	if (sr != 0) {
		r = sr;
	}
#else
	(void) fprintf(
		stderr,
		"[%s]%s%s",
		label,
		__is_an_open_bracket(fmtted_str[0]) ? "" : " ",
		fmtted_str
	);
#endif // USE_SYSCALL

	return r;
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

void logging_cleanup(void) {
	if (__fmtted_txt_buffer != NULL) {
		free(__fmtted_txt_buffer);
		__fmtted_txt_buffer  = NULL;
		__fmtted_txt_buf_cap = 0;
	}

#ifdef USE_SYSCALL
	if (__lbled_txt_buffer != NULL) {
		free(__lbled_txt_buffer);
		__lbled_txt_buffer  = NULL;
		__lbled_txt_buf_cap = 0;
	}
#endif // USE_SYSCALL
}

int debug_printf(const char* fmt, ...) {
	va_list args;
	va_start(args, fmt);
	int ret = logging_vprintf(LOGGER_DEBUG, fmt, args);
	va_end(args);
	return ret;
}

void debug_perror(const char* prefix) {
	logging_perror(LOGGER_DEBUG, prefix);
}

int info_printf(const char* fmt, ...) {
	va_list args;
	va_start(args, fmt);
	int ret = logging_vprintf(LOGGER_INFO, fmt, args);
	va_end(args);
	return ret;
}

void info_perror(const char* prefix) {
	logging_perror(LOGGER_INFO, prefix);
}

int note_printf(const char* fmt, ...) {
	va_list args;
	va_start(args, fmt);
	int ret = logging_vprintf(LOGGER_NOTE, fmt, args);
	va_end(args);
	return ret;
}

void note_perror(const char* prefix) {
	logging_perror(LOGGER_NOTE, prefix);
}

int warning_printf(const char* fmt, ...) {
	va_list args;
	va_start(args, fmt);
	int ret = logging_vprintf(LOGGER_WARNING, fmt, args);
	va_end(args);
	return ret;
}

void warning_perror(const char* prefix) {
	logging_perror(LOGGER_WARNING, prefix);
}

int error_printf(const char* fmt, ...) {
	va_list args;
	va_start(args, fmt);
	int ret = logging_vprintf(LOGGER_ERROR, fmt, args);
	va_end(args);
	return ret;
}

void error_perror(const char* prefix) {
	logging_perror(LOGGER_ERROR, prefix);
}

int critical_printf(const char* fmt, ...) {
	va_list args;
	va_start(args, fmt);
	int ret = logging_vprintf(LOGGER_CRITICAL, fmt, args);
	va_end(args);
	return ret;
}

void critical_perror(const char* prefix) {
	logging_perror(LOGGER_CRITICAL, prefix);
}
