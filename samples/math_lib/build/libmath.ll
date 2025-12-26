; ModuleID = 'stratos_module'
source_filename = "stratos_source"
target datalayout = "e-m:w-p270:32:32-p271:32:32-p272:64:64-i64:64-f80:128-n8:16:32:64-S128"
target triple = "x86_64-pc-windows-msvc"

declare i32 @printf(i8*, ...)

declare i8* @malloc(i64)

; Format specifiers for print
@.str_specifier = private unnamed_addr constant [4 x i8] c"%d\0A\00"

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


define i32 @multiply(i32 %arg0, i32 %arg1) {
  %a.addr = alloca i32
  store i32 %arg0, i32* %a.addr
  %b.addr = alloca i32
  store i32 %arg1, i32* %b.addr
  %t0 = load i32, i32* %a.addr
  %t1 = load i32, i32* %b.addr
  %t2 = mul nsw i32 %t0, %t1
  ret i32 %t2
  ret i32 0
}


define double @square(double %arg0) {
  %x.addr = alloca double
  store double %arg0, double* %x.addr
  %t0 = load double, double* %x.addr
  %t1 = load double, double* %x.addr
  %t2 = fmul double %t0, %t1
  ret double %t2
  ret double 0.0
}


; String Literals
