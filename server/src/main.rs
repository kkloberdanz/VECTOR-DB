#![feature(proc_macro_hygiene, decl_macro)]

#[macro_use]
extern crate rocket;

mod cstorage;

#[get("/hello/<name>/<age>")]
fn hello(name: String, age: u8) -> String {
    format!("Hello, {} year old named {}!", age, name)
}

fn print_hello(i: i32) -> i32 {
    unsafe { cstorage::kt_print_hello(i) }
}

fn main() {
    let x = print_hello(144);
    println!("i got back: {}", x);
    rocket::ignite().mount("/", routes![hello]).launch();
}
