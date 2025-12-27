; ModuleID = 'stratos_module'
source_filename = "stratos_source"
target datalayout = "e-m:w-p270:32:32-p271:32:32-p272:64:64-i64:64-f80:128-n8:16:32:64-S128"
target triple = "x86_64-pc-windows-msvc"

declare i32 @printf(i8*, ...)

declare i8* @malloc(i64)

; Format specifiers for print
@.str_specifier = private unnamed_addr constant [4 x i8] c"%d\0A\00"

define void @main() {
  %t0 = getelementptr inbounds [28 x i8], [28 x i8]* @.str0, i64 0, i64 0
  call void @__native_log_info(i8* %t0)
  %t1 = call i32 (i8*, ...) @printf(i8* getelementptr inbounds ([4 x i8], [4 x i8]* @.str1, i32 0, i32 0), void )
  %sqrtResult_2 = alloca i32
  %t3 = call double @__native_math_sqrt(double 25.0)
  store i32 %t3, i32* %sqrtResult_2
  %sinResult_4 = alloca i32
  %t5 = call double @__native_math_sin(double 0.0)
  store i32 %t5, i32* %sinResult_4
  %cosResult_6 = alloca i32
  %t7 = call double @__native_math_cos(double 0.0)
  store i32 %t7, i32* %cosResult_6
  %t8 = getelementptr inbounds [31 x i8], [31 x i8]* @.str2, i64 0, i64 0
  call void @__native_log_info(i8* %t8)
  %t9 = call i32 (i8*, ...) @printf(i8* getelementptr inbounds ([4 x i8], [4 x i8]* @.str1, i32 0, i32 0), void )
  %upperCase_10 = alloca i32
  %t11 = getelementptr inbounds [6 x i8], [6 x i8]* @.str3, i64 0, i64 0
  %t12 = call i8* @__native_strings_toUpper(i8* %t11)
  store i32 %t12, i32* %upperCase_10
  %lowerCase_13 = alloca i32
  %t14 = getelementptr inbounds [6 x i8], [6 x i8]* @.str4, i64 0, i64 0
  %t15 = call i8* @__native_strings_toLower(i8* %t14)
  store i32 %t15, i32* %lowerCase_13
  %length_16 = alloca i32
  %t17 = getelementptr inbounds [8 x i8], [8 x i8]* @.str5, i64 0, i64 0
  %t18 = call i32 @__native_strings_length(i8* %t17)
  store i32 %t18, i32* %length_16
  %t19 = getelementptr inbounds [27 x i8], [27 x i8]* @.str6, i64 0, i64 0
  call void @__native_log_info(i8* %t19)
  %t20 = call i32 (i8*, ...) @printf(i8* getelementptr inbounds ([4 x i8], [4 x i8]* @.str1, i32 0, i32 0), void )
  %t21 = getelementptr inbounds [27 x i8], [27 x i8]* @.str7, i64 0, i64 0
  call void @__native_log_info(i8* %t21)
  %t22 = call i32 (i8*, ...) @printf(i8* getelementptr inbounds ([4 x i8], [4 x i8]* @.str1, i32 0, i32 0), void )
  %t23 = getelementptr inbounds [44 x i8], [44 x i8]* @.str8, i64 0, i64 0
  call void @__native_log_info(i8* %t23)
  %t24 = call i32 (i8*, ...) @printf(i8* getelementptr inbounds ([4 x i8], [4 x i8]* @.str1, i32 0, i32 0), void )
  %t25 = getelementptr inbounds [21 x i8], [21 x i8]* @.str9, i64 0, i64 0
  call void @__native_log_info(i8* %t25)
  %t26 = call i32 (i8*, ...) @printf(i8* getelementptr inbounds ([4 x i8], [4 x i8]* @.str1, i32 0, i32 0), void )
  ret void
}


define i32 @main() {
  ret i32 0
}


; String Literals
@.str9 = private unnamed_addr constant [21 x i8] c"Integration: SUCCESS\00"
@.str5 = private unnamed_addr constant [8 x i8] c"stratos\00"
@.str8 = private unnamed_addr constant [44 x i8] c"Strings: toUpper, toLower, length processed\00"
@.str3 = private unnamed_addr constant [6 x i8] c"hello\00"
@.str6 = private unnamed_addr constant [27 x i8] c"=== All Tests Complete ===\00"
@.str2 = private unnamed_addr constant [31 x i8] c"=== Testing Strings Module ===\00"
@.str7 = private unnamed_addr constant [27 x i8] c"Math: sqrt(25.0) processed\00"
@.str1 = private unnamed_addr constant [6 x i8] c"%d\0A\00"
@.str4 = private unnamed_addr constant [6 x i8] c"WORLD\00"
@.str0 = private unnamed_addr constant [28 x i8] c"=== Testing Math Module ===\00"
