#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

#include "file.h"
#include "ktmath.h"

static void assert(bool expr, const char *msg) {
    if (!expr) {
        fprintf(stderr, "%s\n", msg);
        exit(-1);
    }
}

int main(void) {
    i64 i;
    struct kt_file *file;
    int status = 0;

    file = kt_find_file("employee", 0, 30);
    assert(file, "could not find data file\n");
    printf("found data file: '%s'\n", file->fname);

    if (sizeof(file->data) != 8) {
        printf("sizeof(data) = %lu\n", sizeof(file->data));
        return -1;
    }

    kt_file_set_int(file, file->num_cells - 1, 0xabcdef1deadbeef);

    for (i = 0; i < 10; i++) {
        kt_file_set_int(file, i, i | 0xa);
    }

    for (i = 0; i < 10; i++) {
        const char *fail_msg = "data integrity lost getting int";
        i64 num = kt_file_get_int(file, i);
        assert(num == (i | 0xa), fail_msg);
        printf("0x%02lx\n", num);
    }

    for (i = 0; i < (i64)file->num_cells; i++) {
        kt_file_set_int(file, i, i);
    }

    for (i = 0; i < (i64)file->num_cells; i++) {
        char fail_msg[200];
        i64 num = kt_file_get_int(file, i);
        sprintf(fail_msg, "data integrity lost getting int: %ld\n", i);
        assert(num == i, fail_msg);
    }

    kt_mean(file, 1, 200001, 0);
    printf("avg from 1 to 200001 is: %ld\n", kt_file_get_int(file, 0));

    for (i = 0; i < (i64)file->num_cells; i++) {
        kt_file_set_float(file, i, (double)i);
    }

    kt_mean(file, 1, 200001, 0);
    printf("avg from 1.0 to 20000.0 is: %f\n", kt_file_get_float(file, 0));

    if (kt_file_free(file)) {
        const char *msg = "*** Very bad things have happened, "
            "data lost, failed to write to disk ***";
        fprintf(stderr, "%s\n", msg);
        status = -1;
    }
    return status;
}
