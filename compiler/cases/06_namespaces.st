// 06_namespaces.st
// Namespaces and Use

namespace MathUtils {
    fn add(a: int, b: int) int {
        return a + b;
    }
}

namespace App {
    use MathUtils;

    fn main() {
        val sum = MathUtils.add(10, 20);
        // or if 'use' imports symbols directly:
        // val sum2 = add(10, 20);
    }
}
