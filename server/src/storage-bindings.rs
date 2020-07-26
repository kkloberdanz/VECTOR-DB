extern crate libc;

use std::os::raw::c_int;

#[link(name = "vectordb-storage")]
extern "C" {
    pub fn kt_print_hello(i: c_int) -> c_int;
    pub fn kt_file_free(file: *mut libc::c_void) -> c_int;
    pub fn kt_find_file(table: *const i8, col: u64, row: u64) -> *mut libc::c_void;
    pub fn kt_file_set_int(file: *mut libc::c_void, row: u64, value: i64);
    pub fn kt_file_set_float(file: *mut libc::c_void, row: u64, value: f64);
    pub fn kt_file_get_int(file: *mut libc::c_void, row: u64) -> i64;
    pub fn kt_file_get_float(file: *mut libc::c_void, row: u64);
}
