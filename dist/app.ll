; ModuleID = 'stratos_module'
source_filename = "stratos_source"
target datalayout = "e-m:e-p:32:32-p10:8:8-p20:8:8-i64:64-n32:64-S128-ni:1:10:20"
target triple = "wasm32-unknown-wasi"

; WASI imports for standalone WASM
declare i32 @__wasi_fd_write(i32, i32, i32, i32)

declare i8* @malloc(i64)

; Host-imported functions
declare void @__stratos_print_str(i8*, i32)
declare void @__stratos_print_int(i32)
declare void @__stratos_print_float(double)

declare i32 @printf(i8*, ...)

; Format specifiers for print
@.str_int = private unnamed_addr constant [4 x i8] c"%d\0A\00"
@.str_float = private unnamed_addr constant [4 x i8] c"%f\0A\00"
@.str_string = private unnamed_addr constant [4 x i8] c"%s\0A\00"

define void @main() {
  %t0 = getelementptr inbounds [16 x i8], [16 x i8]* @.str0, i64 0, i64 0
  %t1 = call i32 @println(i8* %t0)
  %x_2 = alloca i32
  store i32 42, i32* %x_2
  %t3 = load i32, i32* %x_2
  %t4 = call i32 @println(i32 %t3)
  ret void
}


; String Literals
@.str0 = private unnamed_addr constant [16 x i8] c"Hello, Stratos!\00"
