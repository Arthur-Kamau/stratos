; ModuleID = 'stratos_module'
source_filename = "stratos_source"
target datalayout = "e-m:w-p270:32:32-p271:32:32-p272:64:64-i64:64-f80:128-n8:16:32:64-S128"
target triple = "x86_64-pc-windows-msvc"

declare i32 @printf(i8*, ...)


define i32 @main() {
  %age_0 = alloca i32
  store i32 20, i32* %age_0
  %status_1 = alloca i32
  %t2 = load i32, i32* %age_0
  %t3 = add i32 %t2, 18
  %t4 = getelementptr inbounds [6 x i8], [6 x i8]* @.str0, i64 0, i64 0
  %t5 = getelementptr inbounds [6 x i8], [6 x i8]* @.str1, i64 0, i64 0
  %t6 = icmp ne i32 %t3, 0
  %t7 = select i1 %t6, i8* %t4, i8* %t5
  store i32 %t7, i32* %status_1
  %t8 = load i32, i32* %age_0
  %t9 = icmp sgt i32 %t8, 60
  br i1 %t9, label %L0, label %L1

L0:
  %t10 = getelementptr inbounds [7 x i8], [7 x i8]* @.str2, i64 0, i64 0
  %t12 = call i32 (i8*, ...) @printf(i8* getelementptr inbounds ([4 x i8], [4 x i8]* @.str_specifier, i32 0, i32 0), i8* %t10)
  %t13 = call i32 (i8*, ...) @printf(i8* getelementptr inbounds ([4 x i8], [4 x i8]* @.str_specifier, i32 0, i32 0), i32 %t12)
  br label %L2

L1:
  %t14 = load i32, i32* %age_0
  %t15 = icmp sgt i32 %t14, 18
  br i1 %t15, label %L3, label %L4

L3:
  %t16 = getelementptr inbounds [6 x i8], [6 x i8]* @.str0, i64 0, i64 0
  %t18 = call i32 (i8*, ...) @printf(i8* getelementptr inbounds ([4 x i8], [4 x i8]* @.str_specifier, i32 0, i32 0), i8* %t16)
  %t19 = call i32 (i8*, ...) @printf(i8* getelementptr inbounds ([4 x i8], [4 x i8]* @.str_specifier, i32 0, i32 0), i32 %t18)
  br label %L5

L4:
  %t20 = getelementptr inbounds [6 x i8], [6 x i8]* @.str4, i64 0, i64 0
  %t22 = call i32 (i8*, ...) @printf(i8* getelementptr inbounds ([4 x i8], [4 x i8]* @.str_specifier, i32 0, i32 0), i8* %t20)
  %t23 = call i32 (i8*, ...) @printf(i8* getelementptr inbounds ([4 x i8], [4 x i8]* @.str_specifier, i32 0, i32 0), i32 %t22)
  br label %L5

L5:
  br label %L2

L2:
  %i_24 = alloca i32
  store i32 0, i32* %i_24
  br label %L6

L6:
  %t25 = load i32, i32* %i_24
  %t26 = icmp slt i32 %t25, 10
  br i1 %t26, label %L7, label %L8

L7:
  %t27 = load i32, i32* %i_24
  %t29 = call i32 (i8*, ...) @printf(i8* getelementptr inbounds ([4 x i8], [4 x i8]* @.str_specifier, i32 0, i32 0), i32 %t27)
  %t30 = call i32 (i8*, ...) @printf(i8* getelementptr inbounds ([4 x i8], [4 x i8]* @.str_specifier, i32 0, i32 0), i32 %t29)
  br label %L6

L8:
  %userType_31 = alloca i32
  %t32 = getelementptr inbounds [6 x i8], [6 x i8]* @.str6, i64 0, i64 0
  store i32 %t32, i32* %userType_31
  ret i32 0
}


; String Literals
@.str6 = private unnamed_addr constant [6 x i8] c"admin\00"
@.str5 = private unnamed_addr constant [3 x i8] c"%d\00"
@.str4 = private unnamed_addr constant [6 x i8] c"Child\00"
@.str3 = private unnamed_addr constant [3 x i8] c"%s\00"
@.str1 = private unnamed_addr constant [6 x i8] c"Minor\00"
@.str2 = private unnamed_addr constant [7 x i8] c"Senior\00"
@.str0 = private unnamed_addr constant [6 x i8] c"Adult\00"
