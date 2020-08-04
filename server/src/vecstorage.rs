#![allow(non_upper_case_globals)]
#![allow(non_camel_case_types)]
#![allow(non_snake_case)]

use std::ffi::CString;
use std::ffi::CStr;

include!("./storage-bindings.rs");

// TODO: implement drop and share Arc pointers for this object instead
#[derive(Clone, Copy)]
pub struct VecFile {
    ptr: *mut libc::c_void
}

unsafe impl Send for VecFile {}

pub fn print_hello(i: i32) -> i32 {
    unsafe { kt_print_hello(i) }
}

pub fn file_free(file: VecFile) -> i32 {
    unsafe { kt_file_free(file.ptr) }
}

pub fn find_file(
    table: &String,
    col: u64,
    row: u64,
) -> Result<VecFile, String> {
    let c_str = CString::new(String::from(table).into_bytes()).expect("CString::new failed");
    let cfile = unsafe { kt_find_file(c_str.as_ptr(), col, row) };
    if cfile.is_null() {
        Err("failed to open file".to_string())
    } else {
        let file = VecFile{ptr: cfile};
        Ok(file)
    }
}

pub fn get_fname(table: &String, col: u64, row: u64) -> String {
    let c_str = CString::new(String::from(table).into_bytes()).expect("CString::new failed");
    let fname_c_str = unsafe { kt_file_get_fname(c_str.as_ptr(), col, row) };

    if fname_c_str.is_null() {
        // TODO: handle this gracefully
        panic!("failed to allocate string, out of memory");
    }

    let fname_rust_str = unsafe {
        CStr::from_ptr(fname_c_str).to_str().unwrap().to_owned()
    };

    unsafe {
        libc::free(fname_c_str as *mut libc::c_void);
    }
    fname_rust_str
}

pub fn file_set_int(file: &VecFile, row: u64, value: i64) {
    unsafe { kt_file_set_int(file.ptr, row, value) };
}

pub fn file_get_int(file: &VecFile, row: u64) -> i64 {
    let x = unsafe { kt_file_get_int(file.ptr, row) };
    x
}

pub fn file_set_float(file: &VecFile, row: u64, value: f64) {
    unsafe { kt_file_set_float(file.ptr, row, value) };
}

pub fn file_get_float(file: &VecFile, row: u64) -> f64 {
    let x = unsafe { kt_file_get_float(file.ptr, row) };
    x
}

pub fn sum(file: &VecFile, begin: u64, end: u64, dst: u64) -> i32 {
    let x = unsafe { kt_sum(file.ptr, begin, end, dst) };
    x
}

pub fn mean(file: &VecFile, begin: u64, end: u64, dst: u64) -> i32 {
    let x = unsafe { kt_mean(file.ptr, begin, end, dst) };
    x
}

pub fn product(
    file: &VecFile,
    begin: u64,
    end: u64,
    dst: u64,
) -> i32 {
    let x = unsafe { kt_product(file.ptr, begin, end, dst) };
    x
}
