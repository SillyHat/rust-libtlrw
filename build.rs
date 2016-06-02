extern crate gcc;

fn main() {
    gcc::Config::new()
        .file("src/tlrw.c")
        .compile("lib-tlrw.a");
}
