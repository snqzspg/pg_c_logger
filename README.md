# Logging System

A simple logging system for Snqzs' PG and michisei's projects, and may also be used in internal projects in internal systems for tech firms.

This is just a DIY reusable part done in my style for multiple projects and is not meant for general purpose logging unlike [python's logging](https://docs.python.org/3/library/logging.html) module or [log.c project](https://github.com/rxi/log.c). 

---
## Building
If you somehow still want to use this, there are multiple ways to include it in your project:
### Copying the source and header file
The most obvious way is to just copy the source and header files into your project and include the header.
### Building static library
### Makefile
You can run `make` to build the library. It will create `liblogger.a` and `logger.h` files, which you'd need for your project.
```shell
make
```

You can also clean up using
```shell
make clean
```

Note that you can also compile a version that directly uses write system calls to write instead of using printf (sprintf is still used for format parsing).
This still uses libc for string substitution.
```shell
make BUILD_DEFS="-DUSE_SYSCALL"
```
#### Manual build
To build a static library, you can run:
```shell
gcc -o logger.o -c logger.c
ar rcs liblogger.a logger.o
```
### Including into your project
Include the `liblogger.a` and `logger.h` files into your project.

Compile your project with `-I[path_to_logger.h]/logger.h` if necessary and then link the project with `-L[path_to_liblogger.a]` (if necessary) and `-llogger`.
