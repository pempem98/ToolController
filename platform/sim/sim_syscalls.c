#if defined(__arm__) || defined(__thumb__)
#include <sys/stat.h>
#include <errno.h>

void _exit(int status) {
    (void)status;
    while(1);
}

int _close(int file) {
    (void)file;
    return -1;
}

int _fstat(int file, struct stat *st) {
    (void)file;
    st->st_mode = S_IFCHR;
    return 0;
}

int _isatty(int file) {
    (void)file;
    return 1;
}

int _lseek(int file, int ptr, int dir) {
    (void)file; (void)ptr; (void)dir;
    return 0;
}

int _read(int file, char *ptr, int len) {
    (void)file; (void)ptr; (void)len;
    return 0;
}

void *_sbrk(ptrdiff_t incr) {
    extern char _end;
    static char *heap_end;
    char *prev_heap_end;

    if (heap_end == 0) {
        heap_end = &_end;
    }
    prev_heap_end = heap_end;
    heap_end += incr;

    return (void *)prev_heap_end;
}

int _write(int file, char *ptr, int len) {
    (void)file; (void)ptr; (void)len;
    return len;
}
#endif
