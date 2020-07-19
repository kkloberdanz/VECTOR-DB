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

#include "file.h"
#include "../globals.h"

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

        close(file->fd);

        free(file->col);
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
    char **colname,
    int *rowbegin,
    int *rowend
) {
    int i;
    int j;
    int idx;
    enum State state = TABLE_NAME;
    *tablename = NULL;
    *colname = NULL;
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
                    *colname = strdup(str + idx);
                    for (j = 0; (*colname)[j] != '-'; j++);
                    (*colname)[j] = '\0';
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
    free(*colname);
    return 0;
}

char kt_check_file(
    const char *target_table,
    const char *file_table,
    const char *target_col,
    const char *file_col,
    int target_row,
    int row_begin,
    int row_end
) {
    if (strcmp(target_table, file_table)) {
        return 0;
    }

    if (strcmp(target_col, file_col)) {
        return 0;
    }

    if ((target_row < row_begin) || (target_row > row_end)) {
        return 0;
    }

    return 1;
}

struct kt_file *kt_find_file(const char *table, const char *col, int row) {
    DIR *dir;
    struct dirent *ent;
    int rowbegin;
    int rowend;
    char *tablename = NULL;
    char *colname = NULL;
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
                    free(colname);
                    tablename = NULL;
                    colname = NULL;
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

#ifdef KT_TEST_MMAP
int main(void) {
    int i;
    struct kt_file *file;
    const char *str = "this is some data";
    size_t len = strlen(str);
    int status = 0;

    file = kt_find_file("employee", "A", 30);
    if (!file) {
        return -1;
    }
    printf("found data file: '%s'\n", file->fname);
    for (i = 0; i < 10; i++) {
        fprintf(stderr, "%c", (file->dat + 40)[i]);
    }

    memcpy(file->dat + 10, str, len);

    if (kt_file_free(file)) {
        const char *msg = "*** Very bad things have happened, "
            "data lost, failed to write to disk ***";
        fprintf(stderr, "%s\n", msg);
        status = -1;
    }
    return status;
}
#endif
