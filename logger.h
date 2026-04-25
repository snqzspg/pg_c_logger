#ifndef LOGGER_H_INCLUDED
#define LOGGER_H_INCLUDED

enum logging_level {
	DEBUG,
	INFO,
	NOTE,
	WARNING,
	ERROR,
	CRITICAL
};

struct logging_config_s {
	enum logging_level level;
	int                show_color;
};

void config_logging(struct logging_config_s config);
int is_logging_type_enabled(enum logging_level type);

int debug_printf(const char* fmt, ...);
int info_printf(const char* fmt, ...);
int note_printf(const char* fmt, ...);
int warning_printf(const char* fmt, ...);
int error_printf(const char* fmt, ...);
int critical_printf(const char* fmt, ...);

void debug_perror(const char* prefix);
void info_perror(const char* prefix);
void note_perror(const char* prefix);
void warning_perror(const char* prefix);
void error_perror(const char* prefix);
void critical_perror(const char* prefix);

#endif // LOGGER_H_INCLUDED
