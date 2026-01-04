package main;

use log;

fn processOrder(orderId: int, amount: int) {
    log.info("Processing order");
    log.debug("Order details received");

    if amount < 0 {
        log.error("Invalid order amount: negative value");
        return;
    }

    if amount > 10000 {
        log.warn("Large order amount detected");
    }

    log.info("Order processed successfully");
}

fn main() {
    log.info("Application starting");
    log.debug("Initializing system components");

    processOrder(1001, 250);
    processOrder(1002, 15000);
    processOrder(1003, -50);

    log.info("Application completed");
}
