#ifndef KT_FILE_H
#define KT_FILE_H

#include <stdio.h>
#include <stdint.h>

#include "../globals.h"

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
        uint64_t *as_u64;
        double *as_f64;
        void *as_void;
    } data;
};

int kt_file_free(struct kt_file *file);

struct kt_file *kt_find_file(const char *table, int col, int row);

/* set memory segment with values */
void kt_file_set_int(struct kt_file *file, int row, int64_t value);
void kt_file_set_float(struct kt_file *file, int row, double value);

/* get value from memory segment */
i64 kt_file_get_int(struct kt_file *file, int row);
f64 kt_file_get_float(struct kt_file *file, int row);

#endif /* KT_FILE_H */
