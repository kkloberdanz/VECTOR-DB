#ifndef KT_FILE_H
#define KT_FILE_H

#include <stdio.h>
#include <stdint.h>

enum kt_cell_type {
    KT_NIL,
    KT_INT,
    KT_FLOAT,
    KT_STR
};

struct kt_file {
    int fd;
    char *dat;
    size_t map_size;
    char *fname;
    int col;
    char *table;
    size_t row_begin;
    size_t row_end;
    char *header;
    char *type_info;
    union {
        int64_t *as_i64;
        double *as_f64;
        char *as_str[8]; /* contains name of file to get string from */
        void *as_void;
    } data;
};

int kt_file_free(struct kt_file *file);

int kt_open(const char *fname);

struct kt_file *kt_mmap(const char *fname);

struct kt_file *kt_find_file(const char *table, int col, int row);

#endif /* KT_FILE_H */
