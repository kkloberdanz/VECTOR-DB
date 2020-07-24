#ifndef KTMATH_H
#define KTMATH_H

#include "file.h"

enum kt_cell_type kt_get_type_range(
    struct kt_file *file,
    size_t begin,
    size_t end
);

int kt_mean(struct kt_file *file, size_t begin, size_t end, size_t dst);
int kt_sum(struct kt_file *file, size_t begin, size_t end, size_t dst);
int kt_product(struct kt_file *file, size_t begin, size_t end, size_t dst);

#endif /* MATH_H */
