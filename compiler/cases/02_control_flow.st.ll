; ModuleID = 'stratos_module'
source_filename = "stratos_source"
target datalayout = "e-m:w-p270:32:32-p271:32:32-p272:64:64-i64:64-f80:128-n8:16:32:64-S128"
target triple = "x86_64-pc-windows-msvc"

declare i32 @printf(i8*, ...)

@.str_specifier = private unnamed_addr constant [4 x i8] c"%d\0A\00"

define i32 @main() {
  %age_0 = alloca i32
  store i32 20, i32* %age_0
  %t1 = load i32, i32* %age_0
  %t2 = icmp sgt i32 %t1, 60
  br i1 %t2, label %L0, label %L1

L0:
  %t3 = call i32 (i8*, ...) @printf(i8* getelementptr inbounds ([4 x i8], [4 x i8]* @.str_specifier, i32 0, i32 0), i32 %t2)
  br label %L2

L1:
  %t4 = load i32, i32* %age_0
  %t5 = icmp sgt i32 %t4, 18
  br i1 %t5, label %L3, label %L4

L3:
  %t6 = call i32 (i8*, ...) @printf(i8* getelementptr inbounds ([4 x i8], [4 x i8]* @.str_specifier, i32 0, i32 0), i32 %t5)
  br label %L5

L4:
  %t7 = call i32 (i8*, ...) @printf(i8* getelementptr inbounds ([4 x i8], [4 x i8]* @.str_specifier, i32 0, i32 0), i32 %t5)
  br label %L5

L5:
  br label %L2

L2:
  %i_8 = alloca i32
  store i32 0, i32* %i_8
  br label %L6

L6:
  %t9 = load i32, i32* %i_8
  %t10 = icmp slt i32 %t9, 10
  br i1 %t10, label %L7, label %L8

L7:
  %t11 = load i32, i32* %i_8
  %t12 = call i32 (i8*, ...) @printf(i8* getelementptr inbounds ([4 x i8], [4 x i8]* @.str_specifier, i32 0, i32 0), i32 %t11)
  br label %L6

L8:
  %userType_13 = alloca i32
  store i32 %t11, i32* %userType_13
  ret i32 0
}

