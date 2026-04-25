# Logging System

**NOTE**: This project currently only works in Linux due to use of Linux system calls instead of standard libc.

A simple logging system for Snqzs' PG and michisei's projects, and may also be used in internal projects in internal systems for tech firms.

This is just a DIY reusable part done in my style for multiple projects and is not meant for general purpose logging unlike [python's logging](https://docs.python.org/3/library/logging.html) module or [log.c project](https://github.com/rxi/log.c). 

---
## Building
If you somehow still want to use this, there are multiple ways to include it in your project:
### Copying the source and header file
The most obvious way is to just copy the source and header files into your project and include the header.
### Building static library
#### Manual build
To build a static library, you can run:
```shell
gcc -o logger.o -c logger.c
ar rcs liblogger.a logger.o
```
### Including into your project
Include the `liblogger.a` and `logger.h` files into your project.

Compile your project with `-I[path_to_logger.h]/logger.h` if necessary and then link the project with `-L[path_to_liblogger.a]` (if necessary) and `-llogger`.
