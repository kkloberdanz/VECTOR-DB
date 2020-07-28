#![feature(proc_macro_hygiene, decl_macro)]

#[macro_use]
extern crate rocket;

mod vecstorage;

use rocket::response::status::NotFound;
use rocket::response::status::BadRequest;

#[get("/hello/<name>/<age>")]
fn hello(name: String, age: u8) -> String {
    format!("Hello, {} year old named {}!", age, name)
}

#[get("/get/int/<table>/<col>/<row>")]
fn get_int(
    table: String,
    col: u64,
    row: u64,
) -> Result<String, BadRequest<String>> {
    let file = vecstorage::find_file(table, col, row);
    match file {
        Ok(f) => {
            let x = vecstorage::file_get_int(f, row);
            vecstorage::file_free(f);
            Ok(format!("{}", x))
        }
        Err(e) => Err(BadRequest(Some(format!("{}", e)))),
    }
}

#[get("/get/float/<table>/<col>/<row>")]
fn get_float(table: String, col: u64, row: u64) -> Result<String, String> {
    let file = vecstorage::find_file(table, col, row)?;
    let x = vecstorage::file_get_float(file, row);
    vecstorage::file_free(file);
    Ok(format!("{}", x))
}

#[post("/set/int/<table>/<col>/<row>/<value>")]
fn set_int(
    table: String,
    col: u64,
    row: u64,
    value: i64,
) -> Result<String, String> {
    let file = vecstorage::find_file(table, col, row)?;
    vecstorage::file_set_int(file, row, value);
    vecstorage::file_free(file);
    Ok("ok".to_string())
}

#[post("/set/float/<table>/<col>/<row>/<value>")]
fn set_float(
    table: String,
    col: u64,
    row: u64,
    value: f64,
) -> Result<String, String> {
    let file = vecstorage::find_file(table, col, row)?;
    vecstorage::file_set_float(file, row, value);
    vecstorage::file_free(file);
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
    let file = vecstorage::find_file(table, col, row_begin)?;
    let ret = vecstorage::sum(file, row_begin, row_end, dst);
    vecstorage::file_free(file);
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
    let file = vecstorage::find_file(table, col, row_begin)?;
    let x = vecstorage::mean(file, row_begin, row_end, dst);
    vecstorage::file_free(file);
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
    let file = vecstorage::find_file(table, col, row_begin)?;
    let x = vecstorage::product(file, row_begin, row_end, dst);
    vecstorage::file_free(file);
    if x >= 0 {
        Ok("ok".to_string())
    } else {
        Err("summing over invalid types".to_string())
    }
}

fn main() {
    let x = vecstorage::print_hello(144);
    println!("i got back: {}", x);
    rocket::ignite()
        .mount(
            "/",
            routes![
                hello, get_int, get_float, set_int, set_float, sum, mean,
                product
            ],
        )
        .launch();
}
