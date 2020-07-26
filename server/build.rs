fn main() {
    println!("cargo:rustc-link-search=native=../storage");
    println!("cargo:rustc-link-lib=static=vectordb-storage");
}
