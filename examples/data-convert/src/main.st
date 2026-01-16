package main;

use log;
use json;
use yaml;

struct Address {
   pub street: string;
   pub city: string;
   pub zip: int;
}

struct User {
   pub id: int;
   pub name: string;
   pub active: bool;
   pub address: Address;
}

fn main() {
    log.info("Starting Data Convert Example");

    val addr = Address {
        street: "123 Solar Way",
        city: "Neo Tokyo",
        zip: 90210
    };

    val user = User {
        id: 1,
        name: "Kamau",
        active: true,
        address: addr
    };

    log.info("Converting Struct to JSON:");
    //todo add a second parameter to format the output
    val jsonStr: string = json.jsonMarshal(user);
    log.info(jsonStr);

    log.info("Converting Struct to YAML:");
    val yamlStr: string = yaml.yamlMarshal(user);
    log.info(yamlStr);

    log.info("Unmarshalling JSON to Struct:");
    // Create a new JSON string to parse
    val inputJson: string = "{\"id\": 2, \"name\": \"Akira\", \"active\": false, \"address\": {\"street\": \"456 Lunar Ave\", \"city\": \"Moon City\", \"zip\": 12345}}";
    
    // Unmarshal using 'user' instance as a prototype/template
    val newUser = json.jsonUnmarshal(inputJson, user) as User;
    
    log.info("Restored User Name: " + newUser.name);
    log.info("Restored User City: " + newUser.address.city);
    
    log.info("Re-serialized New User:");
    log.info(json.jsonMarshal(newUser));
}
