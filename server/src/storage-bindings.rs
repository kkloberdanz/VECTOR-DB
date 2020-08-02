extern crate libc;

use std::os::raw::c_int;
use std::os::raw::c_char;

#[link(name = "vectrix-storage")]
extern "C" {
    pub fn kt_print_hello(i: c_int) -> c_int;
    pub fn kt_file_free(file: *mut libc::c_void) -> c_int;
    pub fn kt_find_file(table: *const c_char, col: u64, row: u64) -> *mut libc::c_void;
    pub fn kt_file_set_int(file: *mut libc::c_void, row: u64, value: i64);
    pub fn kt_file_set_float(file: *mut libc::c_void, row: u64, value: f64);
    pub fn kt_file_get_int(file: *mut libc::c_void, row: u64) -> i64;
    pub fn kt_file_get_float(file: *mut libc::c_void, row: u64) -> f64;
    pub fn kt_mean(file: *mut libc::c_void, begin: u64, end: u64, dst: u64) -> i32;
    pub fn kt_sum(file: *mut libc::c_void, begin: u64, end: u64, dst: u64) -> i32;
    pub fn kt_product(file: *mut libc::c_void, begin: u64, end: u64, dst: u64) -> i32;
}
