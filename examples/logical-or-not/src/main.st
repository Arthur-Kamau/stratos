 // Demonstration of new features:
 // 1. Logical operators (and, or, not)
 // 2. Convert module
 
 use convert;
 
 fn main() {
     println("=== Stratos Feature Demo ===");
     println("");
   
       // Feature 1: Logical Operators
       println("--- Logical Operators ---");
   
       val age: int = 25;
       val hasLicense: bool = true;
       val hasInsurance: bool = true;
   
       if (age >= 18 and hasLicense and hasInsurance) {
           println("✓ Can drive: age >= 18 AND has license AND has insurance");
       }
   
       val isWeekend: bool = false;
       val isHoliday: bool = false;
   
       if (not isWeekend and not isHoliday) {
           println("✓ It's a workday: NOT weekend AND NOT holiday");
       }
   
       val temp: int = 75;
       if (temp < 60 or temp > 85) {
           println("Temperature out of comfort range");
       } else {
           println("✓ Temperature comfortable: NOT (temp < 60 OR temp > 85)");
       }
   
       println("");
   
       // Feature 2: Convert Module
       println("--- Type Conversions ---");
   
       // String to number
       val priceStr: string = "19.99";
       val price: double = convert.toDouble(priceStr);
       println("Price from string: $" + convert.toFixed(price, 2));
   
       // Number formatting
       val pi: double = 3.14159265359;
       println("Pi to 2 decimals: " + convert.toFixed(pi, 2));
       println("Pi to 5 decimals: " + convert.toFixed(pi, 5));
   
       // Base conversions
       val colorValue: int = 255;
       println("RGB Red component: " + colorValue + " = " + convert.toHex(colorValue));
   
       // Binary flags
       val permissions: int = 7;
       println("File permissions: " + convert.toBinary(permissions) + " (rwx)");
   
       println("");
   
       // Combined example
       println("--- Combined Example ---");
   
       val input: string = "128";
       val value: int = convert.toInt(input);
   
       if (value >= 0 and value <= 255) {
           println("Value " + value + " is valid byte");
           println("  Decimal: " + value);
           println("  Hex: " + convert.toHex(value));
           println("  Binary: " + convert.toBinary(value));
       } else {
           println("Value out of byte range");
       }
   
       println("");
       println("=== Demo Complete ===");
 }