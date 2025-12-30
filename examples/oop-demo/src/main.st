package main;

use models;

fn main() {
    val rect = Rectangle(10.0, 5.0);
    println(rect.area());
    println(rect.perimeter());
    println(rect.getWidth());
}
