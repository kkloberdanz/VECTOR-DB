#![allow(non_upper_case_globals)]
#![allow(non_camel_case_types)]
#![allow(non_snake_case)]

use std::ffi::CString;

include!("./storage-bindings.rs");

pub fn print_hello(i: i32) -> i32 {
    unsafe { kt_print_hello(i) }
}

pub fn file_free(file: *mut libc::c_void) -> i32 {
    unsafe { kt_file_free(file) }
}

pub fn find_file(table: String, col: u64, row: u64) -> Result<*mut libc::c_void, String> {
    let c_str = CString::new(table.into_bytes()).expect("CString::new failed");
    let file = unsafe { kt_find_file(c_str.as_ptr(), col, row) };
    if file.is_null() {
        Err("failed to open file".to_string())
    } else {
        Ok(file)
    }
}

pub fn file_set_int(file: *mut libc::c_void, row: u64, value: i64) {
    unsafe { kt_file_set_int(file, row, value) };
}

pub fn file_get_int(file: *mut libc::c_void, row: u64) -> i64 {
    let x = unsafe { kt_file_get_int(file, row) };
    x
}

//pub fn kt_file_set_float(file: *mut libc::c_void, row: u64, value: f64);
//pub fn kt_file_get_float(file: *mut libc::c_void, row: u64);
