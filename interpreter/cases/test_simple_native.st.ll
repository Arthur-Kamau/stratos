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
  %result_2 = alloca i32
  %t3 = call double @__native_math_sqrt(double 16.0)
  store i32 %t3, i32* %result_2
  %t4 = getelementptr inbounds [21 x i8], [21 x i8]* @.str2, i64 0, i64 0
  call void @__native_log_info(i8* %t4)
  %t5 = call i32 (i8*, ...) @printf(i8* getelementptr inbounds ([4 x i8], [4 x i8]* @.str1, i32 0, i32 0), void )
  ret void
}


define i32 @main() {
  ret i32 0
}


; String Literals
@.str2 = private unnamed_addr constant [21 x i8] c"Result should be 4.0\00"
@.str1 = private unnamed_addr constant [6 x i8] c"%d\0A\00"
@.str0 = private unnamed_addr constant [28 x i8] c"Testing native math.sqrt...\00"
