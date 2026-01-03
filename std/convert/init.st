// Convert module - Type conversions and number formatting
// Provides functions for converting between types and formatting numbers

package convert;

// ============================================================================
// Type Conversion Functions (implemented in C++)
// ============================================================================

// Convert to integer
// Works with: string, double, bool
fn toInt(value: any) int;

// Convert to double
// Works with: string, int, bool
fn toDouble(value: any) double;

// Convert to string
// Works with: int, double, bool, char
fn toString(value: any) string;

// Convert to boolean
// Works with: int (0=false, other=true), double, string ("true"/"1"/"yes"=true)
fn toBool(value: any) bool;

// ============================================================================
// Number Formatting Functions
// ============================================================================

// Format double to fixed decimal places
// Example: toFixed(3.14159, 2) => "3.14"
fn toFixed(value: double, decimals: int) string;

// Format double with specific precision
// Example: toPrecision(123.456, 4) => "123.5"
fn toPrecision(value: double, precision: int) string;

// Format double in exponential notation
// Example: toExponential(1234.5, 2) => "1.23e+03"
fn toExponential(value: double, decimals: int) string;

// ============================================================================
// Base Conversion Functions
// ============================================================================

// Convert integer to hexadecimal string
// Example: toHex(255) => "0xff"
fn toHex(value: int) string;

// Convert integer to octal string
// Example: toOct(8) => "010"
fn toOct(value: int) string;

// Convert integer to binary string
// Example: toBinary(5) => "0b101"
fn toBinary(value: int) string;

// Parse hexadecimal string to integer
// Example: parseHex("0xff") => 255
// Example: parseHex("FF") => 255
fn parseHex(value: string) int;

// Parse octal string to integer
// Example: parseOct("010") => 8
// Example: parseOct("10") => 8
fn parseOct(value: string) int;

// Parse binary string to integer
// Example: parseBinary("0b101") => 5
// Example: parseBinary("101") => 5
fn parseBinary(value: string) int;
