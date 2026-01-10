use collections;

fn testDefault(m: Map<string, int> = newMap<string, int>()) {
    println("Map size: " + m.length());
}

fn main() {
    testDefault();
}
