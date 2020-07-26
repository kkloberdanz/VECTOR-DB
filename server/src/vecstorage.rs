#![allow(non_upper_case_globals)]
#![allow(non_camel_case_types)]
#![allow(non_snake_case)]

include!("./storage-bindings.rs");

pub fn print_hello(i: i32) -> i32 {
    unsafe { kt_print_hello(i) }
}
