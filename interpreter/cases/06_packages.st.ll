; ModuleID = 'stratos_module'
source_filename = "stratos_source"
target datalayout = "e-m:w-p270:32:32-p271:32:32-p272:64:64-i64:64-f80:128-n8:16:32:64-S128"
target triple = "x86_64-pc-windows-msvc"

declare i32 @printf(i8*, ...)

declare i8* @malloc(i64)

; Format specifiers for print
@.str_specifier = private unnamed_addr constant [4 x i8] c"%d\0A\00"

define void @main() {
  %t0 = getelementptr inbounds [20 x i8], [20 x i8]* @.str0, i64 0, i64 0
  %t2 = call i32 (i8*, ...) @printf(i8* getelementptr inbounds ([4 x i8], [4 x i8]* @.str_specifier, i32 0, i32 0), i8* %t0)
  %t3 = call i32 (i8*, ...) @printf(i8* getelementptr inbounds ([4 x i8], [4 x i8]* @.str2, i32 0, i32 0), i32 %t2)
  ret void
}


; String Literals
@.str2 = private unnamed_addr constant [6 x i8] c"%d\0A\00"
@.str1 = private unnamed_addr constant [3 x i8] c"%s\00"
@.str0 = private unnamed_addr constant [20 x i8] c"Package system test\00"
