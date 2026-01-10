; ModuleID = 'stratos_module'
source_filename = "stratos_source"
target datalayout = "e-m:w-p270:32:32-p271:32:32-p272:64:64-i64:64-f80:128-n8:16:32:64-S128"
target triple = "x86_64-pc-windows-msvc"

declare i32 @printf(i8*, ...)

declare i8* @malloc(i64)

; Format specifiers for print
@.str_specifier = private unnamed_addr constant [4 x i8] c"%d\0A\00"

define void @main() {
  %t0 = getelementptr inbounds [39 x i8], [39 x i8]* @.str0, i64 0, i64 0
  %t1 = call i32 @println(i8* %t0)
  %t2 = getelementptr inbounds [1 x i8], [1 x i8]* @.str1, i64 0, i64 0
  %t3 = call i32 @println(i8* %t2)
  %t4 = getelementptr inbounds [22 x i8], [22 x i8]* @.str2, i64 0, i64 0
  %t6 = call i32 (i8*, ...) @printf(i8* getelementptr inbounds ([4 x i8], [4 x i8]* @.str_specifier, i32 0, i32 0), i8* %t4)
  %t7 = getelementptr inbounds [7 x i8], [7 x i8]* @.str4, i64 0, i64 0
  %t9 = call i32 (i8*, ...) @printf(i8* getelementptr inbounds ([4 x i8], [4 x i8]* @.str_specifier, i32 0, i32 0), i8* %t7)
  %t10 = getelementptr inbounds [7 x i8], [7 x i8]* @.str5, i64 0, i64 0
  %t12 = call i32 (i8*, ...) @printf(i8* getelementptr inbounds ([4 x i8], [4 x i8]* @.str_specifier, i32 0, i32 0), i8* %t10)
  %t13 = getelementptr inbounds [6 x i8], [6 x i8]* @.str6, i64 0, i64 0
  %t14 = call i32 @println(i8* %t13)
  %t15 = getelementptr inbounds [1 x i8], [1 x i8]* @.str1, i64 0, i64 0
  %t16 = call i32 @println(i8* %t15)
  %t17 = getelementptr inbounds [24 x i8], [24 x i8]* @.str7, i64 0, i64 0
  %t18 = call i32 @println(i8* %t17)
  %name_19 = alloca i32
  %t20 = getelementptr inbounds [6 x i8], [6 x i8]* @.str8, i64 0, i64 0
  store i32 %t20, i32* %name_19
  %age_21 = alloca i32
  store i32 25, i32* %age_21
  %score_22 = alloca i32
  store i32 95.5, i32* %score_22
  %t23 = getelementptr inbounds [13 x i8], [13 x i8]* @.str9, i64 0, i64 0
  %t24 = load i32, i32* %name_19
  %t25 = call i32 @printf(i8* %t23, i32 %t24)
  %t26 = getelementptr inbounds [9 x i8], [9 x i8]* @.str10, i64 0, i64 0
  %t27 = load i32, i32* %age_21
  %t28 = call i32 @printf(i8* %t26, i32 %t27)
  %t29 = getelementptr inbounds [11 x i8], [11 x i8]* @.str11, i64 0, i64 0
  %t30 = load i32, i32* %score_22
  %t31 = call i32 @printf(i8* %t29, i32 %t30)
  %t32 = getelementptr inbounds [48 x i8], [48 x i8]* @.str12, i64 0, i64 0
  %t33 = load i32, i32* %name_19
  %t34 = load i32, i32* %age_21
  %t35 = load i32, i32* %score_22
  %t36 = call i32 @printf(i8* %t32, i32 %t33, i32 %t34, i32 %t35)
  %t37 = getelementptr inbounds [1 x i8], [1 x i8]* @.str1, i64 0, i64 0
  %t38 = call i32 @println(i8* %t37)
  %t39 = getelementptr inbounds [25 x i8], [25 x i8]* @.str13, i64 0, i64 0
  %t40 = call i32 @println(i8* %t39)
  %a_41 = alloca i32
  store i32 10, i32* %a_41
  %b_42 = alloca i32
  store i32 5, i32* %b_42
  %t43 = getelementptr inbounds [14 x i8], [14 x i8]* @.str14, i64 0, i64 0
  %t44 = load i32, i32* %a_41
  %t45 = load i32, i32* %b_42
  %t46 = load i32, i32* %a_41
  %t47 = load i32, i32* %b_42
  %t48 = add nsw i32 %t46, %t47
  %t49 = call i32 @printf(i8* %t43, i32 %t44, i32 %t45, i32 %t48)
  %t50 = getelementptr inbounds [14 x i8], [14 x i8]* @.str15, i64 0, i64 0
  %t51 = load i32, i32* %a_41
  %t52 = load i32, i32* %b_42
  %t53 = load i32, i32* %a_41
  %t54 = load i32, i32* %b_42
  %t55 = sub nsw i32 %t53, %t54
  %t56 = call i32 @printf(i8* %t50, i32 %t51, i32 %t52, i32 %t55)
  %t57 = getelementptr inbounds [14 x i8], [14 x i8]* @.str16, i64 0, i64 0
  %t58 = load i32, i32* %a_41
  %t59 = load i32, i32* %b_42
  %t60 = load i32, i32* %a_41
  %t61 = load i32, i32* %b_42
  %t62 = mul nsw i32 %t60, %t61
  %t63 = call i32 @printf(i8* %t57, i32 %t58, i32 %t59, i32 %t62)
  %t64 = getelementptr inbounds [1 x i8], [1 x i8]* @.str1, i64 0, i64 0
  %t65 = call i32 @println(i8* %t64)
  %t66 = getelementptr inbounds [24 x i8], [24 x i8]* @.str17, i64 0, i64 0
  %t67 = call i32 @println(i8* %t66)
  %t68 = getelementptr inbounds [13 x i8], [13 x i8]* @.str18, i64 0, i64 0
  %t69 = call i32 @printf(i8* %t68, i1 1)
  %t70 = getelementptr inbounds [13 x i8], [13 x i8]* @.str18, i64 0, i64 0
  %t71 = call i32 @printf(i8* %t70, i1 0)
  %t72 = getelementptr inbounds [15 x i8], [15 x i8]* @.str19, i64 0, i64 0
  %t73 = call i32 @printf(i8* %t72, i8* %t72)
  %t74 = getelementptr inbounds [12 x i8], [12 x i8]* @.str20, i64 0, i64 0
  %t75 = getelementptr inbounds [16 x i8], [16 x i8]* @.str21, i64 0, i64 0
  %t76 = call i32 @printf(i8* %t74, i8* %t75)
  %t77 = getelementptr inbounds [1 x i8], [1 x i8]* @.str1, i64 0, i64 0
  %t78 = call i32 @println(i8* %t77)
  %t79 = getelementptr inbounds [26 x i8], [26 x i8]* @.str22, i64 0, i64 0
  %t80 = call i32 @println(i8* %t79)
  %t81 = getelementptr inbounds [11 x i8], [11 x i8]* @.str23, i64 0, i64 0
  %t83 = call i32 (i8*, ...) @printf(i8* getelementptr inbounds ([4 x i8], [4 x i8]* @.str_specifier, i32 0, i32 0), i8* %t81)
  %i_84 = alloca i32
  store i32 0, i32* %i_84
  br label %L0

L0:
  %t85 = load i32, i32* %i_84
  %t86 = icmp slt i32 %t85, 10
  br i1 %t86, label %L1, label %L2

L1:
  %t87 = getelementptr inbounds [2 x i8], [2 x i8]* @.str24, i64 0, i64 0
  %t89 = call i32 (i8*, ...) @printf(i8* getelementptr inbounds ([4 x i8], [4 x i8]* @.str_specifier, i32 0, i32 0), i8* %t87)
  %t90 = load i32, i32* %i_84
  %t91 = load i32, i32* %i_84
  %t92 = add nsw i32 %t91, 1
  %t93 = add i32 %t90, %t92
  br label %L0

L2:
  %t94 = getelementptr inbounds [8 x i8], [8 x i8]* @.str25, i64 0, i64 0
  %t95 = call i32 @println(i8* %t94)
  ret void
}


; String Literals
@.str24 = private unnamed_addr constant [2 x i8] c"#\00"
@.str22 = private unnamed_addr constant [26 x i8] c"=== Progress Bar Demo ===\00"
@.str20 = private unnamed_addr constant [12 x i8] c"String: {}
\00"
@.str17 = private unnamed_addr constant [24 x i8] c"=== Different Types ===\00"
@.str14 = private unnamed_addr constant [14 x i8] c"{} + {} = {}
\00"
@.str13 = private unnamed_addr constant [25 x i8] c"=== Math with printf ===\00"
@.str1 = private unnamed_addr constant [1 x i8] c"\00"
@.str2 = private unnamed_addr constant [22 x i8] c"Building a sentence: \00"
@.str25 = private unnamed_addr constant [8 x i8] c"] Done!\00"
@.str18 = private unnamed_addr constant [13 x i8] c"Boolean: {}
\00"
@.str3 = private unnamed_addr constant [3 x i8] c"%s\00"
@.str11 = private unnamed_addr constant [11 x i8] c"Score: {}
\00"
@.str19 = private unnamed_addr constant [15 x i8] c"Character: {}
\00"
@.str15 = private unnamed_addr constant [14 x i8] c"{} - {} = {}
\00"
@.str4 = private unnamed_addr constant [7 x i8] c"word1 \00"
@.str8 = private unnamed_addr constant [6 x i8] c"Alice\00"
@.str16 = private unnamed_addr constant [14 x i8] c"{} * {} = {}
\00"
@.str12 = private unnamed_addr constant [48 x i8] c"Summary: {} is {} years old with a score of {}
\00"
@.str23 = private unnamed_addr constant [11 x i8] c"Loading: [\00"
@.str0 = private unnamed_addr constant [39 x i8] c"=== Stratos Prelude Functions Demo ===\00"
@.str6 = private unnamed_addr constant [6 x i8] c"word3\00"
@.str21 = private unnamed_addr constant [16 x i8] c"Hello, Stratos!\00"
@.str5 = private unnamed_addr constant [7 x i8] c"word2 \00"
@.str9 = private unnamed_addr constant [13 x i8] c"Student: {}
\00"
@.str7 = private unnamed_addr constant [24 x i8] c"=== printf Examples ===\00"
@.str10 = private unnamed_addr constant [9 x i8] c"Age: {}
\00"
