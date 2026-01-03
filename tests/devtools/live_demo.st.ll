; ModuleID = 'stratos_module'
source_filename = "stratos_source"
target datalayout = "e-m:w-p270:32:32-p271:32:32-p272:64:64-i64:64-f80:128-n8:16:32:64-S128"
target triple = "x86_64-pc-windows-msvc"

declare i32 @printf(i8*, ...)

declare i8* @malloc(i64)

; Format specifiers for print
@.str_specifier = private unnamed_addr constant [4 x i8] c"%d\0A\00"

define void @main() {
  %t0 = getelementptr inbounds [30 x i8], [30 x i8]* @.str0, i64 0, i64 0
  call void @__native_log_info(i8* %t0)
  %t1 = getelementptr inbounds [37 x i8], [37 x i8]* @.str1, i64 0, i64 0
  call void @__native_log_info(i8* %t1)
  %t2 = getelementptr inbounds [55 x i8], [55 x i8]* @.str2, i64 0, i64 0
  call void @__native_log_info(i8* %t2)
  %t3 = getelementptr inbounds [41 x i8], [41 x i8]* @.str3, i64 0, i64 0
  call void @__native_log_info(i8* %t3)
  %count_4 = alloca i32
  store i32 0, i32* %count_4
  br label %L0

L0:
  %t5 = load i32, i32* %count_4
  %t6 = icmp slt i32 %t5, 30
  br i1 %t6, label %L1, label %L2

L1:
  %count_7 = alloca i32
  %t8 = load i32, i32* %count_7
  %t9 = add nsw i32 %t8, 1
  store i32 %t9, i32* %count_7
  %t10 = load i32, i32* %count_7
  %t11 = srem i32 %t10, 3
  %t12 = icmp eq i32 %t11, 0
  br i1 %t12, label %L3, label %L4

L3:
  %t13 = getelementptr inbounds [23 x i8], [23 x i8]* @.str4, i64 0, i64 0
  call void @__native_log_debug(i8* %t13)
  br label %L5

L4:
  br label %L5

L5:
  %t14 = load i32, i32* %count_7
  %t15 = srem i32 %t14, 5
  %t16 = icmp eq i32 %t15, 0
  br i1 %t16, label %L6, label %L7

L6:
  %t17 = getelementptr inbounds [17 x i8], [17 x i8]* @.str5, i64 0, i64 0
  call void @__native_log_warn(i8* %t17)
  br label %L8

L7:
  br label %L8

L8:
  %t18 = load i32, i32* %count_7
  %t19 = srem i32 %t18, 10
  %t20 = icmp eq i32 %t19, 0
  br i1 %t20, label %L9, label %L10

L9:
  %t21 = getelementptr inbounds [15 x i8], [15 x i8]* @.str6, i64 0, i64 0
  call void @__native_log_error(i8* %t21)
  br label %L11

L10:
  %t22 = getelementptr inbounds [22 x i8], [22 x i8]* @.str7, i64 0, i64 0
  call void @__native_log_info(i8* %t22)
  br label %L11

L11:
  %sleep_count_23 = alloca i32
  store i32 0, i32* %sleep_count_23
  br label %L12

L12:
  %t24 = load i32, i32* %sleep_count_23
  %t25 = icmp slt i32 %t24, 100000000
  br i1 %t25, label %L13, label %L14

L13:
  %sleep_count_26 = alloca i32
  %t27 = load i32, i32* %sleep_count_26
  %t28 = add nsw i32 %t27, 1
  store i32 %t28, i32* %sleep_count_26
  br label %L12

L14:
  br label %L0

L2:
  %t29 = getelementptr inbounds [41 x i8], [41 x i8]* @.str3, i64 0, i64 0
  call void @__native_log_info(i8* %t29)
  %t30 = getelementptr inbounds [47 x i8], [47 x i8]* @.str8, i64 0, i64 0
  call void @__native_log_info(i8* %t30)
  %t31 = getelementptr inbounds [42 x i8], [42 x i8]* @.str9, i64 0, i64 0
  call void @__native_log_info(i8* %t31)
  %final_sleep_32 = alloca i32
  store i32 0, i32* %final_sleep_32
  br label %L15

L15:
  %t33 = load i32, i32* %final_sleep_32
  %t34 = icmp slt i32 %t33, 500000000
  br i1 %t34, label %L16, label %L17

L16:
  %final_sleep_35 = alloca i32
  %t36 = load i32, i32* %final_sleep_35
  %t37 = add nsw i32 %t36, 1
  store i32 %t37, i32* %final_sleep_35
  br label %L15

L17:
  ret void
}


define i32 @main() {
  ret i32 0
}


; String Literals
@.str9 = private unnamed_addr constant [42 x i8] c"This gives you time to see the final logs\00"
@.str8 = private unnamed_addr constant [47 x i8] c"Demo complete - program will exit in 5 seconds\00"
@.str7 = private unnamed_addr constant [22 x i8] c"Info message at count\00"
@.str6 = private unnamed_addr constant [15 x i8] c"Error at count\00"
@.str5 = private unnamed_addr constant [17 x i8] c"Warning at count\00"
@.str4 = private unnamed_addr constant [23 x i8] c"Debug message at count\00"
@.str3 = private unnamed_addr constant [41 x i8] c"========================================\00"
@.str1 = private unnamed_addr constant [37 x i8] c"This program will run for 30 seconds\00"
@.str2 = private unnamed_addr constant [55 x i8] c"Open http://localhost:8080 in your browser to see logs\00"
@.str0 = private unnamed_addr constant [30 x i8] c"DevTools Live Demo - Starting\00"
