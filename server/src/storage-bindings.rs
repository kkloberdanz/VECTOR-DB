use core::ffi::c_void;
use std::os::raw::c_int;

#[link(name = "vectordb-storage")]
extern "C" {
    pub fn kt_print_hello(i: c_int) -> c_int;
    pub fn kt_file_free(file: c_void) -> c_int;
    pub fn kt_file_is_null(file: &c_void) -> c_int;
    pub fn kt_find_file(table: *const i8, col: u64, row: u64) -> c_void;
    pub fn kt_file_set_int(file: c_void, row: u64, value: i64);
    pub fn kt_file_set_float(file: c_void, row: u64, value: f64);
    pub fn kt_file_get_int(file: c_void, row: u64);
    pub fn kt_file_get_float(file: c_void, row: u64);
}
