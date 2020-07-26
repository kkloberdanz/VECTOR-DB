#![feature(proc_macro_hygiene, decl_macro)]

#[macro_use]
extern crate rocket;

mod vecstorage;

#[get("/hello/<name>/<age>")]
fn hello(name: String, age: u8) -> String {
    format!("Hello, {} year old named {}!", age, name)
}

fn main() {
    let x = vecstorage::print_hello(144);
    println!("i got back: {}", x);
    rocket::ignite().mount("/", routes![hello]).launch();
}
