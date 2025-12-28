; ModuleID = 'stratos_module'
source_filename = "stratos_source"
target datalayout = "e-m:w-p270:32:32-p271:32:32-p272:64:64-i64:64-f80:128-n8:16:32:64-S128"
target triple = "x86_64-pc-windows-msvc"

declare i32 @printf(i8*, ...)

declare i8* @malloc(i64)

; Format specifiers for print
@.str_specifier = private unnamed_addr constant [4 x i8] c"%d\0A\00"

define void @main() {
  %t0 = getelementptr inbounds [47 x i8], [47 x i8]* @.str0, i64 0, i64 0
  %t1 = call i32 @println(i8* %t0)
  %greeting_2 = alloca i32
  %t3 = getelementptr inbounds [13 x i8], [13 x i8]* @.str1, i64 0, i64 0
  store i32 0, i32* %greeting_2
  %t4 = load i32, i32* %greeting_2
  %t5 = call i32 @println(i32 %t4)
  %sum_6 = alloca i32
  store i32 0, i32* %sum_6
  %t7 = getelementptr inbounds [11 x i8], [11 x i8]* @.str2, i64 0, i64 0
  %t8 = load i32, i32* %sum_6
  %t9 = add nsw i8* %t7, %t8
  %t10 = call i32 @println(i8* %t9)
  %product_11 = alloca i32
  store i32 0, i32* %product_11
  %t12 = getelementptr inbounds [9 x i8], [9 x i8]* @.str3, i64 0, i64 0
  %t13 = load i32, i32* %product_11
  %t14 = add nsw i8* %t12, %t13
  %t15 = call i32 @println(i8* %t14)
  %fact5_16 = alloca i32
  store i32 0, i32* %fact5_16
  %t17 = getelementptr inbounds [6 x i8], [6 x i8]* @.str4, i64 0, i64 0
  %t18 = load i32, i32* %fact5_16
  %t19 = add nsw i8* %t17, %t18
  %t20 = call i32 @println(i8* %t19)
  %maximum_21 = alloca i32
  store i32 0, i32* %maximum_21
  %t22 = getelementptr inbounds [15 x i8], [15 x i8]* @.str5, i64 0, i64 0
  %t23 = load i32, i32* %maximum_21
  %t24 = add nsw i8* %t22, %t23
  %t25 = call i32 @println(i8* %t24)
  %minimum_26 = alloca i32
  store i32 0, i32* %minimum_26
  %t27 = getelementptr inbounds [15 x i8], [15 x i8]* @.str6, i64 0, i64 0
  %t28 = load i32, i32* %minimum_26
  %t29 = add nsw i8* %t27, %t28
  %t30 = call i32 @println(i8* %t29)
  %t31 = getelementptr inbounds [43 x i8], [43 x i8]* @.str7, i64 0, i64 0
  %t32 = call i32 @println(i8* %t31)
  ret void
}


define i32 @main() {
  ret i32 0
}


; String Literals
@.str7 = private unnamed_addr constant [43 x i8] c"
=== All tests completed successfully! ===\00"
@.str6 = private unnamed_addr constant [15 x i8] c"min(42, 17) = \00"
@.str4 = private unnamed_addr constant [6 x i8] c"5! = \00"
@.str3 = private unnamed_addr constant [9 x i8] c"5 * 6 = \00"
@.str5 = private unnamed_addr constant [15 x i8] c"max(42, 17) = \00"
@.str1 = private unnamed_addr constant [13 x i8] c"Stratos User\00"
@.str2 = private unnamed_addr constant [11 x i8] c"10 + 20 = \00"
@.str0 = private unnamed_addr constant [47 x i8] c"=== Testing Stratos Dependency Management ===
\00"
