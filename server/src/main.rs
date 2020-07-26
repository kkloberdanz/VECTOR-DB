#![feature(proc_macro_hygiene, decl_macro)]

#[macro_use] extern crate rocket;

mod lib;

#[get("/hello/<name>/<age>")]
fn hello(name: String, age: u8) -> String {
    format!("Hello, {} year old named {}!", age, name)
}

fn main() {
    unsafe {
        let x = lib::kt_print_hello(155);
        println!("i got back: {}", x);
    }
    rocket::ignite().mount("/", routes![hello]).launch();
}
