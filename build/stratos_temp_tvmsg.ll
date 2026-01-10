; ModuleID = 'stratos_module'
source_filename = "stratos_source"
target datalayout = "e-m:w-p270:32:32-p271:32:32-p272:64:64-i64:64-f80:128-n8:16:32:64-S128"
target triple = "x86_64-pc-windows-msvc"

declare i32 @printf(i8*, ...)

declare i8* @malloc(i64)

; Format specifiers for print
@.str_specifier = private unnamed_addr constant [4 x i8] c"%d\0A\00"

define void @main() {
  %t0 = getelementptr inbounds [5 x i8], [5 x i8]* @.str0, i64 0, i64 0
  %t1 = call i32 @println(i8* %t0)
  ret void
}


define void @test(i32 %arg0) {
  %f.addr = alloca i32
  store i32 %arg0, i32* %f.addr
  %t0 = getelementptr inbounds [5 x i8], [5 x i8]* @.str1, i64 0, i64 0
  %t1 = call i32 @println(i8* %t0)
  ret void
}


; String Literals
@.str1 = private unnamed_addr constant [5 x i8] c"Func\00"
@.str0 = private unnamed_addr constant [5 x i8] c"Test\00"
