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
#include "../globals.h"

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
    char *d = file->dat + 29127;

    file->header = header;
    file->type_info = ti;
    file->data.as_void = d;
    return 0;
}

int kt_file_free(struct kt_file *file) {
    if (file) {
        /*memset(file->dat, 0, file->map_size);*/
        if (msync(file->dat, file->map_size, MS_SYNC) == -1) {
            perror("failed to write to disk");
            return -1;
        }

        if (munmap(file->dat, file->map_size)) {
            perror("failed to unmap memory");
            return -2;
        }

        close(file->fd);

        free(file->fname);
        free(file->table);
        free(file);
    }
    return 0;
}

int kt_open(const char *fname) {
    int fd;
    if ((fd = open(fname, O_RDWR|O_CREAT, 0600)) == -1) {
        char buf[200];
        snprintf(buf, 200, "kt_open failed to open file: '%s'", fname);
        perror(buf);
    }
    return fd;
}

struct kt_file *kt_mmap(const char *fname) {
    char *dat;
    struct stat statbuf;
    size_t map_size;
    int fd = kt_open(fname);
    struct kt_file *file;

    if (fd == -1) {
        goto fail;
    }

    fstat(fd, &statbuf);
    map_size = statbuf.st_size > KT_MAP_SIZE ? statbuf.st_size : KT_MAP_SIZE;

    if (lseek(fd, map_size - 1, SEEK_SET) == -1) {
        perror("error calling lseek() to 'stretch' the file");
        goto close_fd;
    }

    if (write(fd, "", 1) == -1) {
        perror("error writing last byte of the file");
        goto close_fd;
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
    kt_file_set_segment_pointers(file);
    return file;

unmap_dat:
    munmap(dat, map_size);
close_fd:
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

static char kt_parse_data_filename(
    const char *str,
    char **tablename,
    int *colname,
    int *rowbegin,
    int *rowend
) {
    int i;
    int idx;
    enum State state = TABLE_NAME;
    *tablename = NULL;
    for (i = 0; str[i] != '\0'; i++) {
        if ((str[i] == '-') || (str[i] == '.')) {
            switch (state) {
                case TABLE_NAME:
                    *tablename = strdup(str);
                    (*tablename)[i] = '\0';
                    state++;
                    idx = ++i;
                    break;

                case COL_NAME:
                    i++;
                    *colname = strtol(str + idx, NULL, 10);
                    if (errno == ERANGE) {
                        goto fail;
                    }
                    state++;
                    idx = i;
                    break;

                case ROW_BEGIN:
                    i++;
                    *rowbegin = strtol(str + idx, NULL, 10);
                    if (errno == ERANGE) {
                        goto fail;
                    }
                    state++;
                    idx = i;
                    break;

                case ROW_END:
                    *rowend = strtol(str + idx, NULL, 10);
                    if (errno == ERANGE) {
                        goto fail;
                    }
                    return 1;
            }
        }
    }
fail:
    free(*tablename);
    return 0;
}

char kt_check_file(
    const char *target_table,
    const char *file_table,
    int target_col,
    int file_col,
    int target_row,
    int row_begin,
    int row_end
) {
    if (strcmp(target_table, file_table)) {
        return 0;
    }

    if (target_col != file_col) {
        return 0;
    }

    if ((target_row < row_begin) || (target_row > row_end)) {
        return 0;
    }

    return 1;
}

struct kt_file *kt_find_file(const char *table, int col, int row) {
    DIR *dir;
    struct dirent *ent;
    int rowbegin;
    int rowend;
    int colname;
    char *tablename = NULL;
    struct kt_file *file = NULL;
    char *fname = NULL;

    if ((dir = opendir(storage_dir)) != NULL) {
        while ((ent = readdir(dir)) != NULL) {
            if (!kt_parse_data_filename(
                    ent->d_name,
                    &tablename,
                    &colname,
                    &rowbegin,
                    &rowend
                )
            ) {
                continue;
            } else {
                if (kt_check_file(
                        table,
                        tablename,
                        col,
                        colname,
                        row,
                        rowbegin,
                        rowend)
                ) {
                    size_t dir_len = strlen(storage_dir);
                    size_t dat_file_len = strlen(ent->d_name);
                    fname = malloc(dir_len + dat_file_len + 2);
                    if (!fname) {
                        goto leave;
                    }
                    sprintf(fname, "%s/%s", storage_dir, ent->d_name);
                    file = kt_mmap(fname);
                    if (!file) {
                        goto free_fname;
                    }
                    file->col = colname;
                    file->table = tablename;
                    file->row_begin = rowbegin;
                    file->row_end = rowend;
                    file->fname = fname;
                    goto leave;
                } else {
                    free(tablename);
                    tablename = NULL;
                }
            }
        }
    } else {
        perror("failed to open storage directory");
    }
free_fname:
    free(fname);
leave:
    closedir(dir);
    return file;
}

void kt_file_set_int(struct kt_file *file, int row, int64_t value) {
    file->type_info[row] = KT_INT;
    file->data.as_i64[row] = value;
}

void kt_file_set_float(struct kt_file *file, int row, double value) {
    file->type_info[row] = KT_FLOAT;
    file->data.as_f64[row] = value;
}

void kt_file_set_str(struct kt_file *file, int row, const char *value) {
    UNUSED(value);
    file->type_info[row] = KT_STR;
    /* TODO: instead of setting string literal, make a file to store the
     * string and use the data.as_str memory to point to that file */
}

i64 kt_file_get_int(struct kt_file *file, int row) {
    return file->data.as_i64[row];
}

f64 kt_file_get_float(struct kt_file *file, int row) {
    return file->data.as_f64[row];
}

void kt_file_print_cell(struct kt_file *file, int row) {
    switch (file->type_info[row]) {
        case KT_INT:
            fprintf(stderr, "%ld\n", file->data.as_i64[row]);
            break;

        case KT_FLOAT:
            fprintf(stderr, "%f\n", file->data.as_f64[row]);
            break;

        case KT_STR:
            /* TODO: lookup file pointed to by file->data.as_u64[row] */
            break;

        case KT_NIL:
            break;
    }
}

void assert(bool expr, const char *msg) {
    if (!expr) {
        fprintf(stderr, "%s\n", msg);
        exit(-1);
    }
}

#ifdef KT_TEST_MMAP
int main(void) {
    int i;
    struct kt_file *file;
    int status = 0;

    file = kt_find_file("employee", 0, 30);
    assert(file, "could not find data file\n");
    printf("found data file: '%s'\n", file->fname);

    if (sizeof(file->data) != 8) {
        printf("sizeof(data) = %lu\n", sizeof(file->data));
        return -1;
    }

    for (i = 0; i < 10; i++) {
        kt_file_set_int(file, i, i | 0xa);
    }

    for (i = 0; i < 10; i++) {
        const char *fail_msg = "data integrity lost getting int";
        i64 num = kt_file_get_int(file, i);
        assert(num == (i | 0xa), fail_msg);
        printf("0x%02lx\n", num);
    }

    if (kt_file_free(file)) {
        const char *msg = "*** Very bad things have happened, "
            "data lost, failed to write to disk ***";
        fprintf(stderr, "%s\n", msg);
        status = -1;
    }
    return status;
}
#endif
