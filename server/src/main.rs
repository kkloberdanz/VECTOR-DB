#![feature(proc_macro_hygiene, decl_macro)]

#[macro_use]
extern crate rocket;

#[macro_use]
extern crate lazy_static;

extern crate libc;

mod vecstorage;

use rocket::response::status::BadRequest;
use std::collections::HashMap;
use std::panic;
use std::process;
use std::sync::Mutex;

lazy_static! {
    static ref FILES: Mutex<HashMap<String, vecstorage::VecFile>> =
        Mutex::new(HashMap::new());
}

fn get_file(
    table: &String,
    col: u64,
    row: u64,
) -> Result<vecstorage::VecFile, String> {
    let fname = vecstorage::get_fname(table, col, row);
    let mut files = FILES.lock().unwrap();
    match files.get(&fname) {
        Some(my_file) => Ok(*my_file),
        None => {
            let file = vecstorage::find_file(table, col, row)?;
            files.insert(fname, file);
            Ok(file)
        }
    }
}

#[get("/hello")]
fn hello() -> String {
    format!("hello world!")
}

#[get("/get/int/<table>/<col>/<row>")]
fn get_int(
    table: String,
    col: u64,
    row: u64,
) -> Result<String, BadRequest<String>> {
    let file = get_file(&table, col, row);
    match file {
        Ok(f) => {
            let x = vecstorage::file_get_int(&f, row);
            Ok(format!("{}", x))
        }
        Err(e) => Err(BadRequest(Some(format!("{}", e)))),
    }
}

#[get("/get/float/<table>/<col>/<row>")]
fn get_float(table: String, col: u64, row: u64) -> Result<String, String> {
    let file = get_file(&table, col, row)?;
    let x = vecstorage::file_get_float(&file, row);
    Ok(format!("{}", x))
}

#[post("/set/int/<table>/<col>/<row>/<value>")]
fn set_int(
    table: String,
    col: u64,
    row: u64,
    value: i64,
) -> Result<String, String> {
    let file = get_file(&table, col, row)?;
    vecstorage::file_set_int(&file, row, value);
    Ok("ok".to_string())
}

#[post("/clear/<table>/<col>/<row>")]
fn clear_cell(
    table: String,
    col: u64,
    row: u64,
) -> Result<String, String> {
    let file = get_file(&table, col, row)?;
    vecstorage::file_set_invalid(&file, row);
    Ok("ok".to_string())
}

#[post("/set/float/<table>/<col>/<row>/<value>")]
fn set_float(
    table: String,
    col: u64,
    row: u64,
    value: f64,
) -> Result<String, String> {
    let file = get_file(&table, col, row)?;
    vecstorage::file_set_float(&file, row, value);
    Ok("ok".to_string())
}

#[post("/sum/<table>/<col>/<row_begin>/<row_end>/<dst>")]
fn sum(
    table: String,
    col: u64,
    row_begin: u64,
    row_end: u64,
    dst: u64,
) -> Result<String, String> {
    // TODO: handle operations over multiple files
    let file = get_file(&table, col, row_begin)?;
    let ret = vecstorage::sum(&file, row_begin, row_end, dst);
    if ret >= 0 {
        Ok("ok".to_string())
    } else {
        Err("summing over invalid types".to_string())
    }
}

#[post("/mean/<table>/<col>/<row_begin>/<row_end>/<dst>")]
fn mean(
    table: String,
    col: u64,
    row_begin: u64,
    row_end: u64,
    dst: u64,
) -> Result<String, String> {
    // TODO: handle operations over multiple files
    let file = get_file(&table, col, row_begin)?;
    let x = vecstorage::mean(&file, row_begin, row_end, dst);
    if x >= 0 {
        Ok("ok".to_string())
    } else {
        Err("summing over invalid types".to_string())
    }
}

#[post("/product/<table>/<col>/<row_begin>/<row_end>/<dst>")]
fn product(
    table: String,
    col: u64,
    row_begin: u64,
    row_end: u64,
    dst: u64,
) -> Result<String, String> {
    // TODO: handle operations over multiple files
    let file = get_file(&table, col, row_begin)?;
    let x = vecstorage::product(&file, row_begin, row_end, dst);
    if x >= 0 {
        Ok("ok".to_string())
    } else {
        Err("summing over invalid types".to_string())
    }
}

fn cleanup_cache() {
    let mut files = FILES.lock().unwrap();
    for (_, file) in files.iter() {
        vecstorage::file_free(*file);
    }
    files.clear();
}

// in case of panic, be sure to free the cache
fn sigint_handler() {
    cleanup_cache();
    process::exit(0);
}

fn main() {
    ctrlc::set_handler(move || sigint_handler())
        .expect("Error setting Ctrl-C handler");

    panic::set_hook(Box::new(|_| {
        // in case of panic, ensure that the cache is written to disk
        cleanup_cache();
        eprintln!("panic, but was able to write cache to disk");
        process::exit(-1);
    }));

    rocket::ignite()
        .mount(
            "/",
            routes![
                hello, get_int, get_float, set_int, set_float, sum, mean,
                product, clear_cell
            ],
        )
        .launch();

    cleanup_cache();
}

#[cfg(test)]
mod tests {
    use super::*;
}
