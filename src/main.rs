/*
 * TLRW algorithm implementation for Rust.
 * Let's see what we can do here.
 * 
 * (c) Silly Hat, 2016.
 */
 
/*
 * Features, additional crates and constants go here.
 */
 
#![allow(dead_code)]

extern crate libc;
extern crate rand;
extern crate time;

use rand::Rng;

const NELEMS:   usize = 4;

/*
 * External C functions.
 */

extern {
    /* tut budet tlrw */
}

/* =======
 *  MAIN!
 * =======
 */

fn main() {
    println!("hellooooo");
    let mut stuff = vec![0; NELEMS];
    print_vec(&stuff);
}

/* ==============
 *  USEFUL STUFF
 * ==============
 */
 
/*
 * Getting current time.
 */
 
fn nownow() -> f64 {
    let timetime = time::get_time();
    let current: f64 = timetime.sec as f64 + (timetime.nsec as f64 /
        1000.0 / 1000.0 / 1000.0);
    current
}

/*
 * Printing a vector of unsigned 32-bit integers.
 */

fn print_vec(vec: &Vec<u32>) {
    for item in vec.iter() {
        print!("{}\t", item);
    }
    print!("\n");
}
