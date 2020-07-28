#include <math.h>

#include "file.h"
#include "ktmath.h"

enum kt_cell_type kt_get_type_range(
    struct kt_file *file,
    size_t begin,
    size_t end
) {
    size_t i;
    enum kt_cell_type type = file->type_info[begin];
    for (i = begin; i < end; i++) {
        if (unlikely(type != (enum kt_cell_type)file->type_info[i])) {
            return KT_INVALID;
        }
    }
    return type;
}

int kt_mean(struct kt_file *file, size_t begin, size_t end, size_t dst) {
    /*
     * To avoid overflow, don't accumulate everything into a single int,
     * instead multiply each element by the ratio of the number of elements
     */
    double num_elements = end - begin;
    double ratio = 1.0 / num_elements;
    size_t i;
    double mean = 0.0;
    enum kt_cell_type type = kt_get_type_range(file, begin, end);

    /*
     * These loops are trivial for a compiler to vectorize
     * See: https://godbolt.org/z/vv4GPP
     */
    switch (type) {
        case KT_INT:
            for (i = begin; i < end; i++) {
                mean += ratio * file->data.as_i64[i];
            }
            mean = round(mean);
            kt_file_set_int(file, dst, mean);
            break;

        case KT_FLOAT:
            for (i = begin; i < end; i++) {
                mean += ratio * file->data.as_f64[i];
            }
            kt_file_set_float(file, dst, mean);
            break;

        case KT_NIL:
        case KT_STR:
        case KT_INVALID:
            return -2;
    }

    return 0;
}

int kt_sum(struct kt_file *file, size_t begin, size_t end, size_t dst) {
    size_t i;
    enum kt_cell_type type = kt_get_type_range(file, begin, end);
    size_t acc = 0;
    switch (type) {
        case KT_INT:
            for (i = begin; i < end; i++) {
                acc += file->data.as_i64[i];
            }
            kt_file_set_int(file, dst, acc);
            break;

        case KT_FLOAT:
            for (i = begin; i < end; i++) {
                acc += file->data.as_f64[i];
            }
            kt_file_set_float(file, dst, acc);
            break;

        case KT_NIL:
        case KT_STR:
        case KT_INVALID:
            return -2;
    }
    return 0;
}

int kt_product(struct kt_file *file, size_t begin, size_t end, size_t dst) {
    size_t i;
    enum kt_cell_type type = kt_get_type_range(file, begin, end);
    size_t acc = 1;
    switch (type) {
        case KT_INT:
            for (i = begin; i < end; i++) {
                acc *= file->data.as_i64[i];
                if (unlikely(!acc)) {
                    break;
                }
            }
            kt_file_set_int(file, dst, acc);
            break;

        case KT_FLOAT:
            for (i = begin; i < end; i++) {
                acc *= file->data.as_f64[i];
                if (unlikely(!acc)) {
                    break;
                }
            }
            kt_file_set_float(file, dst, acc);
            break;

        case KT_NIL:
        case KT_STR:
        case KT_INVALID:
            return -2;
    }

    return 0;
}
