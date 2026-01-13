package main;

use async;

async fn fetchData(id: int) Future<string> {
    println("Fetching data for ID: " + id + "...");
    await async.delay(1000); // Simulate network delay
    println("Data fetched for ID: " + id);
    return "Data-" + id;
}

async fn processData(data: string) Future<string> {
    println("Processing " + data + "...");
    await async.delay(500); // Simulate processing time
    return "Processed-" + data;
}

async fn main() {
    println("Starting async demo...");

    val data = await fetchData(42);
    println("Got data: " + data);

    val result = await processData(data);
    println("Final Result: " + result);
}
