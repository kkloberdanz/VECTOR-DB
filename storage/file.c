#include <stdio.h>
#include <stdlib.h>
#include <err.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/mman.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>
#include <dirent.h>
#include <errno.h>
#include <stdbool.h>

#include "file.h"
#include "ktmath.h"
#include "../globals.h"

const size_t STEP_SIZE = 233016;

const char *storage_dir = "worksheetsdb";

char *strdup(const char *);

/**
 * The storage file consists of
 * 1) Header - contains enum of what kind of storage and version number
 * 2) Type Info - byte stream of enumerations of which type each cell in the
 *    data section are.
 *    Type Info abbreviated as ti, data section abbreviated as d
 *        ti[0] -> typeof(d[0])
 *        ti[1] -> typeof(d[1])
 *        ti[2] -> typeof(d[2])
 * 3) Data - contains the actual data being stored. Each cell in the data
 *    section is 8 bytes. The type of each cell can vary. It could be an int64,
 *    float64, the name of the file containing a string, etc
 */
static int kt_file_set_segment_pointers(struct kt_file *file) {
    char *header = file->dat;
    char *ti = file->dat + 8;
    char *d = file->dat + STEP_SIZE + 8;

    file->header = header;
    file->type_info = ti;
    file->data.as_void = d;
    return 0;
}

int kt_file_free(struct kt_file *file) {
    if (file) {
        if (msync(file->dat, file->map_size, MS_SYNC) == -1) {
            perror("failed to write to disk");
            return -1;
        }

        if (munmap(file->dat, file->map_size)) {
            perror("failed to unmap memory");
            return -2;
        }

        file->lock.l_type = F_UNLCK;
        fcntl(file->fd, F_SETLKW, &file->lock);

        close(file->fd);

        free(file->fname);
        free(file->table);
        free(file);
    }
    return 0;
}

static int kt_open(const char *fname) {
    int fd = -1;
    if ((fd = open(fname, O_RDWR|O_CREAT, 0600)) == -1) {
        char buf[200];
        snprintf(buf, 200, "kt_open failed to open file: '%s'", fname);
        perror(buf);
    }
    return fd;
}

static struct kt_file *kt_mmap(const char *fname) {
    char *dat;
    struct stat statbuf;
    size_t map_size;
    int fd = kt_open(fname);
    struct kt_file *file;
    struct flock lock;

    if (fd == -1) {
        goto fail;
    }

    memset(&lock, 0, sizeof(lock));
    lock.l_type = F_WRLCK;
    fcntl(fd, F_SETLKW, &lock);

    fstat(fd, &statbuf);
    map_size = statbuf.st_size > KT_MAP_SIZE ? statbuf.st_size : KT_MAP_SIZE;

    if (lseek(fd, map_size - 1, SEEK_SET) == -1) {
        perror("error calling lseek() to 'stretch' the file");
        goto close_fd;
    }

    if (statbuf.st_size < KT_MAP_SIZE) {
        /* expand file to full size */
        if (write(fd, "", 1) == -1) {
            perror("error writing last byte of the file");
            goto close_fd;
        }
    }

    /* TODO: allocate huge pages */
    dat = mmap(
        NULL,
        map_size,
        PROT_READ|PROT_WRITE,
        MAP_SHARED,
        fd,
        0
    );

    if (dat == MAP_FAILED) {
        perror("failed to map file");
        goto close_fd;
    }

    file = malloc(sizeof(*file));
    if (!file) {
        goto unmap_dat;
    }

    file->fd = fd;
    file->dat = dat;
    file->map_size = map_size;
    file->num_cells = STEP_SIZE;
    file->lock = lock;
    kt_file_set_segment_pointers(file);
    return file;

unmap_dat:
    munmap(dat, map_size);
close_fd:
    lock.l_type = F_UNLCK;
    fcntl(fd, F_SETLKW, &lock);
    close(fd);
fail:
    return NULL;
}

enum State {
    TABLE_NAME,
    COL_NAME,
    ROW_BEGIN,
    ROW_END
};

/*
 * get the name of the file that contains the table at the given row and col
 */
char *kt_file_get_fname(const char *table, size_t col, size_t row) {
    size_t lower_bound = (row / STEP_SIZE) * STEP_SIZE; /* floor division */
    size_t upper_bound = lower_bound + STEP_SIZE;
    size_t fname_sz = strlen(table) + strlen(storage_dir) + 100;
    char *fname = malloc(fname_sz);

    if (!fname) {
        return NULL;
    }

    snprintf(
        fname,
        fname_sz,
        "%s/%s-%lu-%lu-%lu.db",
        storage_dir,
        table,
        col,
        lower_bound,
        upper_bound
    );

    return fname;
}

struct kt_file *kt_find_file(const char *table, size_t col, size_t row) {
    size_t lower_bound = (row / STEP_SIZE) * STEP_SIZE; /* floor division */
    size_t upper_bound = lower_bound + STEP_SIZE;
    struct kt_file *file = NULL;
    char *fname = kt_file_get_fname(table, col, row);

    if (!fname) {
        return NULL;
    }

    file = kt_mmap(fname);
    if (file) {
        file->col = col;
        file->table = strdup(table);
        file->row_begin = lower_bound;
        file->row_end = upper_bound;
        file->fname = fname;
    }
    return file;
}

void kt_file_set_nil(struct kt_file *file, size_t row) {
    file->type_info[row] = KT_NIL;
    file->data.as_i64[row] = 0;
}

void kt_file_set_int(struct kt_file *file, size_t row, int64_t value) {
    file->type_info[row] = KT_INT;
    file->data.as_i64[row] = value;
}

void kt_file_set_float(struct kt_file *file, size_t row, double value) {
    file->type_info[row] = KT_FLOAT;
    file->data.as_f64[row] = value;
}

void kt_file_set_str(struct kt_file *file, size_t row, const char *value) {
    UNUSED(value);
    file->type_info[row] = KT_STR;
    /* TODO: instead of setting string literal, make a file to store the
     * string and use the data.as_str memory to point to that file */
}

enum kt_cell_type kt_file_get_cell_type(struct kt_file *file, size_t row) {
    return file->type_info[row];
}

i64 kt_file_get_int(struct kt_file *file, size_t row) {
    return file->data.as_i64[row];
}

f64 kt_file_get_float(struct kt_file *file, size_t row) {
    return file->data.as_f64[row];
}

int kt_print_hello(int i) {
    printf("hello, your number is: %d\n", i);
    return 42;
}

void kt_file_print_cell(struct kt_file *file, size_t row) {
    switch (file->type_info[row]) {
        case KT_INT:
            fprintf(stderr, "%ld\n", file->data.as_i64[row]);
            break;

        case KT_FLOAT:
            fprintf(stderr, "%f\n", file->data.as_f64[row]);
            break;

        case KT_STR:
            /* TODO: lookup file pointed to by file->data.as_i64[row] */
            break;

        case KT_NIL:
            break;
    }
}
