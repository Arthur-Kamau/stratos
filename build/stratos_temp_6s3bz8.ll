; ModuleID = 'stratos_module'
source_filename = "stratos_source"
target datalayout = "e-m:w-p270:32:32-p271:32:32-p272:64:64-i64:64-f80:128-n8:16:32:64-S128"
target triple = "x86_64-pc-windows-msvc"

declare i32 @printf(i8*, ...)

declare i8* @malloc(i64)

; Format specifiers for print
@.str_specifier = private unnamed_addr constant [4 x i8] c"%d\0A\00"

define void @main() {
  %t0 = getelementptr inbounds [33 x i8], [33 x i8]* @.str0, i64 0, i64 0
  %t1 = call i32 @println(i8* %t0)
  %d1_2 = alloca i32
  %t3 = fptosi double 3.14 to i32
  store i32 %t3, i32* %d1_2
  %t4 = getelementptr inbounds [14 x i8], [14 x i8]* @.str1, i64 0, i64 0
  %t5 = load i32, i32* %d1_2
  %t6 = add nsw i8* %t4, %t5
  %t7 = call i32 @println(i8* %t6)
  %d2_8 = alloca i32
  %t9 = fptosi double 3.99 to i32
  store i32 %t9, i32* %d2_8
  %t10 = getelementptr inbounds [14 x i8], [14 x i8]* @.str2, i64 0, i64 0
  %t11 = load i32, i32* %d2_8
  %t12 = add nsw i8* %t10, %t11
  %t13 = call i32 @println(i8* %t12)
  %d3_14 = alloca i32
  %t15 = fptosi double 2.5 to i32
  store i32 %t15, i32* %d3_14
  %t16 = getelementptr inbounds [14 x i8], [14 x i8]* @.str3, i64 0, i64 0
  %t17 = load i32, i32* %d3_14
  %t18 = add nsw i8* %t16, %t17
  %t19 = call i32 @println(i8* %t18)
  %i1_20 = alloca i32
  %t21 = sitofp i32 10 to double
  store i32 %t21, i32* %i1_20
  %t22 = getelementptr inbounds [15 x i8], [15 x i8]* @.str4, i64 0, i64 0
  %t23 = load i32, i32* %i1_20
  %t24 = add nsw i8* %t22, %t23
  %t25 = call i32 @println(i8* %t24)
  %i2_26 = alloca i32
  %t27 = sitofp i32 5 to double
  store i32 %t27, i32* %i2_26
  %t28 = getelementptr inbounds [15 x i8], [15 x i8]* @.str5, i64 0, i64 0
  %t29 = load i32, i32* %i2_26
  %t30 = add nsw i8* %t28, %t29
  %t31 = call i32 @println(i8* %t30)
  %b1_32 = alloca i32
  store i32 1, i32* %b1_32
  %t33 = getelementptr inbounds [14 x i8], [14 x i8]* @.str6, i64 0, i64 0
  %t34 = load i32, i32* %b1_32
  %t35 = add nsw i8* %t33, %t34
  %t36 = call i32 @println(i8* %t35)
  %b2_37 = alloca i32
  store i32 0, i32* %b2_37
  %t38 = getelementptr inbounds [15 x i8], [15 x i8]* @.str7, i64 0, i64 0
  %t39 = load i32, i32* %b2_37
  %t40 = add nsw i8* %t38, %t39
  %t41 = call i32 @println(i8* %t40)
  %intTrue_42 = alloca i32
  store i32 1, i32* %intTrue_42
  %t43 = getelementptr inbounds [12 x i8], [12 x i8]* @.str8, i64 0, i64 0
  %t44 = load i32, i32* %intTrue_42
  %t45 = add nsw i8* %t43, %t44
  %t46 = call i32 @println(i8* %t45)
  %intFalse_47 = alloca i32
  store i32 0, i32* %intFalse_47
  %t48 = getelementptr inbounds [12 x i8], [12 x i8]* @.str9, i64 0, i64 0
  %t49 = load i32, i32* %intFalse_47
  %t50 = add nsw i8* %t48, %t49
  %t51 = call i32 @println(i8* %t50)
  %intAny_52 = alloca i32
  store i32 123, i32* %intAny_52
  %t53 = getelementptr inbounds [14 x i8], [14 x i8]* @.str10, i64 0, i64 0
  %t54 = load i32, i32* %intAny_52
  %t55 = add nsw i8* %t53, %t54
  %t56 = call i32 @println(i8* %t55)
  %doubleTrue_57 = alloca i32
  store i32 0.5, i32* %doubleTrue_57
  %t58 = getelementptr inbounds [14 x i8], [14 x i8]* @.str11, i64 0, i64 0
  %t59 = load i32, i32* %doubleTrue_57
  %t60 = add nsw i8* %t58, %t59
  %t61 = call i32 @println(i8* %t60)
  %doubleFalse_62 = alloca i32
  store i32 0.0, i32* %doubleFalse_62
  %t63 = getelementptr inbounds [14 x i8], [14 x i8]* @.str12, i64 0, i64 0
  %t64 = load i32, i32* %doubleFalse_62
  %t65 = add nsw i8* %t63, %t64
  %t66 = call i32 @println(i8* %t65)
  %doubleAny_67 = alloca i32
  store i32 10.0, i32* %doubleAny_67
  %t68 = getelementptr inbounds [16 x i8], [16 x i8]* @.str13, i64 0, i64 0
  %t69 = load i32, i32* %doubleAny_67
  %t70 = add nsw i8* %t68, %t69
  %t71 = call i32 @println(i8* %t70)
  %s1_72 = alloca i32
  %t73 = getelementptr inbounds [4 x i8], [4 x i8]* @.str14, i64 0, i64 0
  store i32 %t73, i32* %s1_72
  %t74 = getelementptr inbounds [15 x i8], [15 x i8]* @.str15, i64 0, i64 0
  %t75 = load i32, i32* %s1_72
  %t76 = add nsw i8* %t74, %t75
  %t77 = call i32 @println(i8* %t76)
  %s2_78 = alloca i32
  %t79 = getelementptr inbounds [5 x i8], [5 x i8]* @.str16, i64 0, i64 0
  store i32 %t79, i32* %s2_78
  %t80 = getelementptr inbounds [19 x i8], [19 x i8]* @.str17, i64 0, i64 0
  %t81 = load i32, i32* %s2_78
  %t82 = add nsw i8* %t80, %t81
  %t83 = call i32 @println(i8* %t82)
  %iToStr_84 = alloca i32
  store i32 456, i32* %iToStr_84
  %t85 = getelementptr inbounds [17 x i8], [17 x i8]* @.str18, i64 0, i64 0
  %t86 = load i32, i32* %iToStr_84
  %t87 = add nsw i8* %t85, %t86
  %t88 = getelementptr inbounds [2 x i8], [2 x i8]* @.str19, i64 0, i64 0
  %t89 = add nsw i8* %t87, %t88
  %t90 = call i32 @println(i8* %t89)
  %dToStr_91 = alloca i32
  store i32 7.89, i32* %dToStr_91
  %t92 = getelementptr inbounds [18 x i8], [18 x i8]* @.str20, i64 0, i64 0
  %t93 = load i32, i32* %dToStr_91
  %t94 = add nsw i8* %t92, %t93
  %t95 = getelementptr inbounds [2 x i8], [2 x i8]* @.str19, i64 0, i64 0
  %t96 = add nsw i8* %t94, %t95
  %t97 = call i32 @println(i8* %t96)
  %bToStr_98 = alloca i32
  store i32 1, i32* %bToStr_98
  %t99 = getelementptr inbounds [18 x i8], [18 x i8]* @.str21, i64 0, i64 0
  %t100 = load i32, i32* %bToStr_98
  %t101 = add nsw i8* %t99, %t100
  %t102 = getelementptr inbounds [2 x i8], [2 x i8]* @.str19, i64 0, i64 0
  %t103 = add nsw i8* %t101, %t102
  %t104 = call i32 @println(i8* %t103)
  %cToInt_105 = alloca i32
  store i32 %t104, i32* %cToInt_105
  %t106 = getelementptr inbounds [13 x i8], [13 x i8]* @.str22, i64 0, i64 0
  %t107 = load i32, i32* %cToInt_105
  %t108 = add nsw i8* %t106, %t107
  %t109 = call i32 @println(i8* %t108)
  %intToC_110 = alloca i32
  store i32 97, i32* %intToC_110
  %t111 = getelementptr inbounds [14 x i8], [14 x i8]* @.str23, i64 0, i64 0
  %t112 = load i32, i32* %intToC_110
  %t113 = add nsw i8* %t111, %t112
  %t114 = getelementptr inbounds [2 x i8], [2 x i8]* @.str24, i64 0, i64 0
  %t115 = add nsw i8* %t113, %t114
  %t116 = call i32 @println(i8* %t115)
  %strToC_117 = alloca i32
  %t118 = getelementptr inbounds [2 x i8], [2 x i8]* @.str25, i64 0, i64 0
  store i32 %t118, i32* %strToC_117
  %t119 = getelementptr inbounds [15 x i8], [15 x i8]* @.str26, i64 0, i64 0
  %t120 = load i32, i32* %strToC_117
  %t121 = add nsw i8* %t119, %t120
  %t122 = getelementptr inbounds [2 x i8], [2 x i8]* @.str24, i64 0, i64 0
  %t123 = add nsw i8* %t121, %t122
  %t124 = call i32 @println(i8* %t123)
  %t125 = getelementptr inbounds [28 x i8], [28 x i8]* @.str27, i64 0, i64 0
  %t126 = call i32 @println(i8* %t125)
  %unsafeStr_127 = alloca i32
  %t128 = getelementptr inbounds [6 x i8], [6 x i8]* @.str28, i64 0, i64 0
  store i32 %t128, i32* %unsafeStr_127
  %safeInt_129 = alloca i32
  %t130 = load i32, i32* %unsafeStr_127
  store i32 %t130, i32* %safeInt_129
  %t131 = getelementptr inbounds [36 x i8], [36 x i8]* @.str29, i64 0, i64 0
  %t132 = load i32, i32* %safeInt_129
  %t133 = add nsw i8* %t131, %t132
  %t134 = getelementptr inbounds [25 x i8], [25 x i8]* @.str30, i64 0, i64 0
  %t135 = add nsw i8* %t133, %t134
  %t136 = call i32 @println(i8* %t135)
  %safeValid_137 = alloca i32
  %t138 = getelementptr inbounds [4 x i8], [4 x i8]* @.str14, i64 0, i64 0
  store i32 %t138, i32* %safeValid_137
  %t139 = getelementptr inbounds [33 x i8], [33 x i8]* @.str31, i64 0, i64 0
  %t140 = load i32, i32* %safeValid_137
  %t141 = add nsw i8* %t139, %t140
  %t142 = call i32 @println(i8* %t141)
  %t143 = getelementptr inbounds [32 x i8], [32 x i8]* @.str32, i64 0, i64 0
  %t144 = call i32 @println(i8* %t143)
  ret void
}


; String Literals
@.str32 = private unnamed_addr constant [32 x i8] c"
--- Casting Tests Complete ---\00"
@.str30 = private unnamed_addr constant [25 x i8] c"' (should be empty/void)\00"
@.str29 = private unnamed_addr constant [36 x i8] c"Safe cast 'hello' as? int result: '\00"
@.str9 = private unnamed_addr constant [12 x i8] c"0 as bool: \00"
@.str13 = private unnamed_addr constant [16 x i8] c"-10.0 as bool: \00"
@.str28 = private unnamed_addr constant [6 x i8] c"hello\00"
@.str11 = private unnamed_addr constant [14 x i8] c"0.5 as bool: \00"
@.str7 = private unnamed_addr constant [15 x i8] c"false as int: \00"
@.str18 = private unnamed_addr constant [17 x i8] c"456 as string: "\00"
@.str19 = private unnamed_addr constant [2 x i8] c""\00"
@.str6 = private unnamed_addr constant [14 x i8] c"true as int: \00"
@.str2 = private unnamed_addr constant [14 x i8] c"3.99 as int: \00"
@.str15 = private unnamed_addr constant [15 x i8] c""123" as int: \00"
@.str5 = private unnamed_addr constant [15 x i8] c"-5 as double: \00"
@.str24 = private unnamed_addr constant [2 x i8] c"'\00"
@.str10 = private unnamed_addr constant [14 x i8] c"123 as bool: \00"
@.str4 = private unnamed_addr constant [15 x i8] c"10 as double: \00"
@.str8 = private unnamed_addr constant [12 x i8] c"1 as bool: \00"
@.str12 = private unnamed_addr constant [14 x i8] c"0.0 as bool: \00"
@.str17 = private unnamed_addr constant [19 x i8] c""3.14" as double: \00"
@.str1 = private unnamed_addr constant [14 x i8] c"3.14 as int: \00"
@.str0 = private unnamed_addr constant [33 x i8] c"--- Stratos Casting Examples ---\00"
@.str3 = private unnamed_addr constant [14 x i8] c"-2.5 as int: \00"
@.str16 = private unnamed_addr constant [5 x i8] c"3.14\00"
@.str20 = private unnamed_addr constant [18 x i8] c"7.89 as string: "\00"
@.str14 = private unnamed_addr constant [4 x i8] c"123\00"
@.str21 = private unnamed_addr constant [18 x i8] c"true as string: "\00"
@.str26 = private unnamed_addr constant [15 x i8] c""Z" as char: '\00"
@.str22 = private unnamed_addr constant [13 x i8] c"'A' as int: \00"
@.str31 = private unnamed_addr constant [33 x i8] c"Safe cast '123' as? int result: \00"
@.str23 = private unnamed_addr constant [14 x i8] c"97 as char: '\00"
@.str25 = private unnamed_addr constant [2 x i8] c"Z\00"
@.str27 = private unnamed_addr constant [28 x i8] c"
--- Safe Cast Examples ---\00"
