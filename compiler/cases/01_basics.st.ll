; ModuleID = 'stratos_module'
source_filename = "stratos_source"
target datalayout = "e-m:w-p270:32:32-p271:32:32-p272:64:64-i64:64-f80:128-n8:16:32:64-S128"
target triple = "x86_64-pc-windows-msvc"

declare i32 @printf(i8*, ...)

@.str_specifier = private unnamed_addr constant [4 x i8] c"%d\0A\00"

define i32 @main() {
  %pi_0 = alloca i32
  store i32 3.14159, i32* %pi_0
  %counter_1 = alloca i32
  store i32 0, i32* %counter_1
  %message_2 = alloca i32
  store i32 0, i32* %message_2
  %isRunning_3 = alloca i32
  store i32 1, i32* %isRunning_3
  %t4 = load i32, i32* %counter_1
  %t5 = load i32, i32* %counter_1
  %t6 = add nsw i32 %t5, 1
  %t7 = add i32 %t4, %t6
  %t8 = call i32 (i8*, ...) @printf(i8* getelementptr inbounds ([4 x i8], [4 x i8]* @.str_specifier, i32 0, i32 0), i32 %t7)
  %t9 = load i32, i32* %counter_1
  %t10 = add i32 %t9, 5
  %t11 = call i32 (i8*, ...) @printf(i8* getelementptr inbounds ([4 x i8], [4 x i8]* @.str_specifier, i32 0, i32 0), i32 %t10)
  %temp_12 = alloca i32
  store i32 100, i32* %temp_12
  %result_13 = alloca i32
  %t14 = load i32, i32* %temp_12
  %t15 = mul nsw i32 %t14, 2
  store i32 %t15, i32* %result_13
  ret i32 0
}

