; ModuleID = 'stratos_module'
source_filename = "stratos_source"
target datalayout = "e-m:w-p270:32:32-p271:32:32-p272:64:64-i64:64-f80:128-n8:16:32:64-S128"
target triple = "x86_64-pc-windows-msvc"

declare i32 @printf(i8*, ...)

declare i8* @malloc(i64)

; Format specifiers for print
@.str_specifier = private unnamed_addr constant [4 x i8] c"%d\0A\00"

define void @main() {
  %t0 = getelementptr inbounds [34 x i8], [34 x i8]* @.str0, i64 0, i64 0
  %t1 = call i32 @println(i8* %t0)
  %t2 = call i32 @__native_terminal_clear()
  %t3 = getelementptr inbounds [16 x i8], [16 x i8]* @.str1, i64 0, i64 0
  %t4 = call i32 @println(i8* %t3)
  ret void
}


define i32 @main() {
  ret i32 0
}


; String Literals
@.str1 = private unnamed_addr constant [16 x i8] c"Screen cleared!\00"
@.str0 = private unnamed_addr constant [34 x i8] c"Starting minimal terminal test...\00"
