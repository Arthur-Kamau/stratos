package main;

use async;

async fn test() {
    val x = await async.delay(100);
    println(x);
}

fn main() {
    println("done");
}
