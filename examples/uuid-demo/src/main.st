package main;

use crypto;
use log;

fn main() {
    log.info("=== UUID v4 Generation Demo ===");

    // Generate and display 5 UUIDs
    log.info("Generating 5 random UUIDs:");
    var count = 0;
    while (count < 5) {
        val uuid = crypto.uuid();
        log.info("  UUID: " + uuid);
        count = count + 1;
    }

    log.info("");
    log.info("=== UUID Format Explanation ===");
    log.info("Format: xxxxxxxx-xxxx-4xxx-yxxx-xxxxxxxxxxxx");
    log.info("  - The '4' at position 14 indicates UUID version 4 (random)");
    log.info("  - The 'y' character is one of [8, 9, a, b] (RFC 4122 variant)");
    log.info("  - All other characters are random hexadecimal digits");

    log.info("");
    log.info("=== Example Use Cases ===");
    log.info("1. Database primary keys:");
    val userId = crypto.uuid();
    log.info("   User ID: " + userId);

    log.info("");
    log.info("2. Session identifiers:");
    val sessionId = crypto.uuid();
    log.info("   Session: " + sessionId);

    log.info("");
    log.info("3. Request tracking:");
    val requestId = crypto.uuid();
    log.info("   Request: " + requestId);
}
