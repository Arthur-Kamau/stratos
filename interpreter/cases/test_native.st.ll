; ModuleID = 'stratos_module'
source_filename = "stratos_source"
target datalayout = "e-m:w-p270:32:32-p271:32:32-p272:64:64-i64:64-f80:128-n8:16:32:64-S128"
target triple = "x86_64-pc-windows-msvc"

declare i32 @printf(i8*, ...)

declare i8* @malloc(i64)

; Format specifiers for print
@.str_specifier = private unnamed_addr constant [4 x i8] c"%d\0A\00"

define void @main() {
  %t0 = getelementptr inbounds [27 x i8], [27 x i8]* @.str0, i64 0, i64 0
  call void @__native_log_info(i8* %t0)
  %t1 = call i32 (i8*, ...) @printf(i8* getelementptr inbounds ([4 x i8], [4 x i8]* @.str1, i32 0, i32 0), void )
  %sqrtResult_2 = alloca i32
  %t3 = call double @__native_math_sqrt(double 16.0)
  store i32 %t3, i32* %sqrtResult_2
  %t4 = getelementptr inbounds [9 x i8], [9 x i8]* @.str2, i64 0, i64 0
  %t5 = load i32, i32* %sqrtResult_2
  %t6 = add nsw i8* %t4, %t5
  call void @__native_log_info(i8* %t6)
  %t7 = call i32 (i8*, ...) @printf(i8* getelementptr inbounds ([4 x i8], [4 x i8]* @.str1, i32 0, i32 0), void )
  %t8 = getelementptr inbounds [25 x i8], [25 x i8]* @.str3, i64 0, i64 0
  call void @__native_log_info(i8* %t8)
  %t9 = call i32 (i8*, ...) @printf(i8* getelementptr inbounds ([4 x i8], [4 x i8]* @.str1, i32 0, i32 0), void )
  %sinResult_10 = alloca i32
  %t11 = call double @__native_math_sin(double 0.0)
  store i32 %t11, i32* %sinResult_10
  %t12 = getelementptr inbounds [9 x i8], [9 x i8]* @.str2, i64 0, i64 0
  %t13 = load i32, i32* %sinResult_10
  %t14 = add nsw i8* %t12, %t13
  call void @__native_log_info(i8* %t14)
  %t15 = call i32 (i8*, ...) @printf(i8* getelementptr inbounds ([4 x i8], [4 x i8]* @.str1, i32 0, i32 0), void )
  %t16 = getelementptr inbounds [36 x i8], [36 x i8]* @.str4, i64 0, i64 0
  call void @__native_log_info(i8* %t16)
  %t17 = call i32 (i8*, ...) @printf(i8* getelementptr inbounds ([4 x i8], [4 x i8]* @.str1, i32 0, i32 0), void )
  %upperResult_18 = alloca i32
  %t19 = getelementptr inbounds [6 x i8], [6 x i8]* @.str5, i64 0, i64 0
  %t20 = call i8* @__native_strings_toUpper(i8* %t19)
  store i32 %t20, i32* %upperResult_18
  %t21 = getelementptr inbounds [9 x i8], [9 x i8]* @.str2, i64 0, i64 0
  %t22 = load i32, i32* %upperResult_18
  %t23 = add nsw i8* %t21, %t22
  call void @__native_log_info(i8* %t23)
  %t24 = call i32 (i8*, ...) @printf(i8* getelementptr inbounds ([4 x i8], [4 x i8]* @.str1, i32 0, i32 0), void )
  %t25 = getelementptr inbounds [34 x i8], [34 x i8]* @.str6, i64 0, i64 0
  call void @__native_log_info(i8* %t25)
  %t26 = call i32 (i8*, ...) @printf(i8* getelementptr inbounds ([4 x i8], [4 x i8]* @.str1, i32 0, i32 0), void )
  %lenResult_27 = alloca i32
  %t28 = getelementptr inbounds [5 x i8], [5 x i8]* @.str7, i64 0, i64 0
  %t29 = call i32 @__native_strings_length(i8* %t28)
  store i32 %t29, i32* %lenResult_27
  %t30 = getelementptr inbounds [9 x i8], [9 x i8]* @.str2, i64 0, i64 0
  %t31 = load i32, i32* %lenResult_27
  %t32 = add nsw i8* %t30, %t31
  call void @__native_log_info(i8* %t32)
  %t33 = call i32 (i8*, ...) @printf(i8* getelementptr inbounds ([4 x i8], [4 x i8]* @.str1, i32 0, i32 0), void )
  %t34 = getelementptr inbounds [21 x i8], [21 x i8]* @.str8, i64 0, i64 0
  call void @__native_log_info(i8* %t34)
  %t35 = call i32 (i8*, ...) @printf(i8* getelementptr inbounds ([4 x i8], [4 x i8]* @.str1, i32 0, i32 0), void )
  %csvData_36 = alloca i32
  %t37 = getelementptr inbounds [27 x i8], [27 x i8]* @.str9, i64 0, i64 0
  store i32 %t37, i32* %csvData_36
  %records_38 = alloca i32
  %t39 = load i32, i32* %csvData_36
  %t40 = call i32 @__native_csv_parse(i32 %t39)
  store i32 %t40, i32* %records_38
  %t41 = getelementptr inbounds [24 x i8], [24 x i8]* @.str10, i64 0, i64 0
  call void @__native_log_info(i8* %t41)
  %t42 = call i32 (i8*, ...) @printf(i8* getelementptr inbounds ([4 x i8], [4 x i8]* @.str1, i32 0, i32 0), void )
  %t43 = getelementptr inbounds [32 x i8], [32 x i8]* @.str11, i64 0, i64 0
  call void @__native_log_info(i8* %t43)
  %t44 = call i32 (i8*, ...) @printf(i8* getelementptr inbounds ([4 x i8], [4 x i8]* @.str1, i32 0, i32 0), void )
  %hexResult_45 = alloca i32
  %t46 = call i32 @__native_crypto_randomHex(i32 16)
  store i32 %t46, i32* %hexResult_45
  %t47 = getelementptr inbounds [13 x i8], [13 x i8]* @.str12, i64 0, i64 0
  %t48 = load i32, i32* %hexResult_45
  %t49 = add nsw i8* %t47, %t48
  call void @__native_log_info(i8* %t49)
  %t50 = call i32 (i8*, ...) @printf(i8* getelementptr inbounds ([4 x i8], [4 x i8]* @.str1, i32 0, i32 0), void )
  %t51 = getelementptr inbounds [37 x i8], [37 x i8]* @.str13, i64 0, i64 0
  call void @__native_log_info(i8* %t51)
  %t52 = call i32 (i8*, ...) @printf(i8* getelementptr inbounds ([4 x i8], [4 x i8]* @.str1, i32 0, i32 0), void )
  ret void
}


define i32 @main() {
  ret i32 0
}


; String Literals
@.str13 = private unnamed_addr constant [37 x i8] c"All native function tests completed!\00"
@.str12 = private unnamed_addr constant [13 x i8] c"Random hex: \00"
@.str3 = private unnamed_addr constant [25 x i8] c"Testing math.sin(0.0)...\00"
@.str4 = private unnamed_addr constant [36 x i8] c"Testing strings.toUpper('hello')...\00"
@.str5 = private unnamed_addr constant [6 x i8] c"hello\00"
@.str7 = private unnamed_addr constant [5 x i8] c"test\00"
@.str8 = private unnamed_addr constant [21 x i8] c"Testing csv.parse...\00"
@.str0 = private unnamed_addr constant [27 x i8] c"Testing math.sqrt(16.0)...\00"
@.str1 = private unnamed_addr constant [6 x i8] c"%d\0A\00"
@.str2 = private unnamed_addr constant [9 x i8] c"Result: \00"
@.str9 = private unnamed_addr constant [27 x i8] c"name,age\nAlice,30\nBob,25\00"
@.str6 = private unnamed_addr constant [34 x i8] c"Testing strings.length('test')...\00"
@.str10 = private unnamed_addr constant [24 x i8] c"Parsed CSV successfully\00"
@.str11 = private unnamed_addr constant [32 x i8] c"Testing crypto.randomHex(16)...\00"
