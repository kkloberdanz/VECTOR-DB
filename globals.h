#ifndef KT_GLOBALS_H
#define KT_GLOBALS_H

#include <stdint.h>

#define UNUSED(x) (void)(x)

typedef uint64_t u64;
typedef int64_t i64;
typedef int32_t i32;
typedef uint32_t u32;
typedef double f64;
typedef float f32;

enum kt_constants {
    KT_MAP_SIZE = (2 << 20)
};

#endif /* KT_GLOBALS_H */
