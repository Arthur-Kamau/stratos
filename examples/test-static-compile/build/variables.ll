; ModuleID = 'stratos_module'
source_filename = "stratos_source"
target datalayout = "e-m:e-p270:32:32-p271:32:32-p272:64:64-i64:64-f80:128-n8:16:32:64-S128"
target triple = "x86_64-pc-linux-gnu"

declare i32 @printf(i8*, ...)

declare i8* @malloc(i64)

; Format specifiers for print
@.str_int = private unnamed_addr constant [4 x i8] c"%d\0A\00"
@.str_float = private unnamed_addr constant [4 x i8] c"%f\0A\00"
@.str_string = private unnamed_addr constant [4 x i8] c"%s\0A\00"

define void @main() {
  %t0 = getelementptr inbounds [40 x i8], [40 x i8]* @.str0, i64 0, i64 0
  %t1 = call i32 (i8*, ...) @printf(i8* getelementptr inbounds ([4 x i8], [4 x i8]* @.str_string, i32 0, i32 0), i8* %t0)
  %t2 = getelementptr inbounds [27 x i8], [27 x i8]* @.str1, i64 0, i64 0
  %t3 = call i32 (i8*, ...) @printf(i8* getelementptr inbounds ([4 x i8], [4 x i8]* @.str_string, i32 0, i32 0), i8* %t2)
  ret void
}


; String Literals
@.str1 = private unnamed_addr constant [27 x i8] c"Testing optimization flags\00"
@.str0 = private unnamed_addr constant [40 x i8] c"Hello from statically compiled Stratos!\00"
