CC=clang
CFLAGS = -Wall -Wpedantic -Wextra
AR=ar
ARCMD = rcs

BUILD_DEFS = 

SRCS := logger.c
OBJS := logger.o

lib_out := liblogger.a

RM = rm -v 

REPORT_INFO_FMT = | awk '{print "[\033[1;34mINFO\033[0m] " $$0}' 
REPORT_NOEXIST  = printf '[\033[1;34mINFO\033[0m] File "%s" already cleaned \n'

.SUFFIXES: 
.SUFFIXES: .o .c

.PHONY: clean help

liblogger.a: logger.o
	${AR} ${ARCMD} $@ logger.o

.c.o:
	${CC} ${CFLAGS} ${BUILD_DEFS} -c $<

clean:
	@if [ -f logger.o ]; \
	then \
		${RM} logger.o ${REPORT_INFO_FMT}; \
	else \
		${REPORT_NOEXIST} logger.o; \
	fi

	@if [ -f liblogger.a ]; \
	then \
		${RM} liblogger.a ${REPORT_INFO_FMT}; \
	else \
		${REPORT_NOEXIST} liblogger.a; \
	fi
