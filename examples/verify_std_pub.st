package main;

use std.collections;
use std.encoding.json;
use std.crypto;

// Mock Http Request since it might be complex to instantiate
class MockRequest {
    pub fn getMethod() string { return "GET"; }
}

class Test {
    pub fn run() void {
        println("Verifying std lib public access...");
        
        // 1. Collections
        println("Testing Collections...");
        val list = collections.newList<string>();
        list.add("test"); 
        println("List size: " + list.length()); 

        // 2. JSON
        println("Testing JSON...");
        val m = collections.newMap<string, json.JsonValue>();
        val j = json.jsonObject(m);
        if j.isObject() {
            println("JSON is object");
        }

        // 3. Crypto
        println("Testing Crypto...");
        val hasher = crypto.newHash("sha256");
        hasher.updateString("hello");
        println("Hash computed");

        println("Std lib verification passed!");
    }
}

fn main() {
    Test().run();
}
