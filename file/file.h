#ifndef KT_FILE_H
#define KT_FILE_H

#include <stdio.h>
#include <stdlib.h>
#include <err.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/mman.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>

struct kt_file {
    int fd;
    char *dat;
    size_t map_size;
};

int kt_file_free(struct kt_file *file);

int kt_open(const char *fname);

struct kt_file *kt_mmap(const char *fname);

#endif /* KT_FILE_H */
