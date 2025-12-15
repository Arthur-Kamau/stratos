; ModuleID = 'stratos_module'
source_filename = "stratos_source"
target datalayout = "e-m:w-p270:32:32-p271:32:32-p272:64:64-i64:64-f80:128-n8:16:32:64-S128"
target triple = "x86_64-pc-windows-msvc"

declare i32 @printf(i8*, ...)


define i32 @add(i32 %arg0, i32 %arg1) {
  %a.addr = alloca i32
  store i32 %arg0, i32* %a.addr
  %b.addr = alloca i32
  store i32 %arg1, i32* %b.addr
  %t0 = load i32, i32* %a.addr
  %t1 = load i32, i32* %b.addr
  %t2 = add nsw i32 %t0, %t1
  ret i32 %t2
  ret i32 0
}


define void @greet(i8* %arg0) {
  %name.addr = alloca i8*
  store i8* %arg0, i8** %name.addr
  %t0 = getelementptr inbounds [7 x i8], [7 x i8]* @.str0, i64 0, i64 0
  %t1 = load i8*, i8** %name.addr
  %t2 = add nsw i8* %t0, %t1
  %t4 = call i32 (i8*, ...) @printf(i8* getelementptr inbounds ([4 x i8], [4 x i8]* @.str_specifier, i32 0, i32 0), i8* %t2)
  %t5 = call i32 (i8*, ...) @printf(i8* getelementptr inbounds ([4 x i8], [4 x i8]* @.str_specifier, i32 0, i32 0), i32 %t4)
  ret void
}


define i32 @square(i32 %arg0) {
  %x.addr = alloca i32
  store i32 %arg0, i32* %x.addr
  %t0 = load i32, i32* %x.addr
  %t1 = load i32, i32* %x.addr
  %t2 = mul nsw i32 %t0, %t1
  ret i32 %t2
  ret i32 0
}


define i32 @makeDouble(i32 %arg0) {
  %x.addr = alloca i32
  store i32 %arg0, i32* %x.addr
  %t0 = load i32, i32* %x.addr
  %t1 = mul nsw i32 %t0, 2
  ret i32 %t1
  ret i32 0
}


define i32 @main() {
  %result_2 = alloca i32
  %t3 = call i32 @square()
  %t4 = add i32 5, %t3
  %t5 = call i32 @makeDouble()
  %t6 = add i32 %t4, %t5
  store i32 %t6, i32* %result_2
  ret i32 0
}


; String Literals
@.str1 = private unnamed_addr constant [3 x i8] c"%s\00"
@.str0 = private unnamed_addr constant [7 x i8] c"Hello \00"
