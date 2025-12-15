; ModuleID = 'stratos_module'
source_filename = "stratos_source"
target datalayout = "e-m:w-p270:32:32-p271:32:32-p272:64:64-i64:64-f80:128-n8:16:32:64-S128"
target triple = "x86_64-pc-windows-msvc"

declare i32 @printf(i8*, ...)


define i32 @main() {
  %pi_0 = alloca double
  store double 3.14159, double* %pi_0
  %counter_1 = alloca i32
  store i32 0, i32* %counter_1
  %message_2 = alloca i32
  %t3 = getelementptr inbounds [15 x i8], [15 x i8]* @.str0, i64 0, i64 0
  store i32 %t3, i32* %message_2
  %isRunning_4 = alloca i32
  store i32 1, i32* %isRunning_4
  %t5 = load i32, i32* %counter_1
  %t6 = load i32, i32* %counter_1
  %t7 = add nsw i32 %t6, 1
  %t8 = add i32 %t5, %t7
  %t9 = call i32 (i8*, ...) @printf(i8* getelementptr inbounds ([4 x i8], [4 x i8]* @.str_specifier, i32 0, i32 0), i32 %t8)
  %t10 = load i32, i32* %counter_1
  %t11 = add i32 %t10, 5
  %t12 = call i32 (i8*, ...) @printf(i8* getelementptr inbounds ([4 x i8], [4 x i8]* @.str_specifier, i32 0, i32 0), i32 %t11)
  %temp_13 = alloca i32
  store i32 100, i32* %temp_13
  %result_14 = alloca i32
  %t15 = load i32, i32* %temp_13
  %t16 = mul nsw i32 %t15, 2
  store i32 %t16, i32* %result_14
  ret i32 0
}


; String Literals
@.str0 = private unnamed_addr constant [15 x i8] c"Hello, Stratos\00"
