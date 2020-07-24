#include "file.h"
#include "ktmath.h"
#include "math.h"

int kt_mean(struct kt_file *file, size_t begin, size_t end, size_t dst) {
    /*
     * To avoid overflow, don't accumulate everything into a single int,
     * instead multiply each element by the ratio of the number of elements
     */
    double num_elements = end - begin;
    double ratio = 1.0 / num_elements;
    size_t i;
    double mean = 0.0;
    enum kt_cell_type first_type;
    enum kt_cell_type type = 0;

    first_type = file->type_info[begin];

    for (i = begin; i < end; i++) {
        type |= file->type_info[begin];
    }

    if (type != first_type) {
        return -1;
    }

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
            return -2;
    }

    return 0;
}
