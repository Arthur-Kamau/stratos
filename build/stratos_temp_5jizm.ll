; ModuleID = 'stratos_module'
source_filename = "stratos_source"
target datalayout = "e-m:w-p270:32:32-p271:32:32-p272:64:64-i64:64-f80:128-n8:16:32:64-S128"
target triple = "x86_64-pc-windows-msvc"

declare i32 @printf(i8*, ...)

declare i8* @malloc(i64)

; Format specifiers for print
@.str_specifier = private unnamed_addr constant [4 x i8] c"%d\0A\00"

define void @printHeader(i8* %arg0) {
  %title.addr = alloca i8*
  store i8* %arg0, i8** %title.addr
  %t0 = getelementptr inbounds [2 x i8], [2 x i8]* @.str0, i64 0, i64 0
  %t1 = getelementptr inbounds [2 x i8], [2 x i8]* @.str1, i64 0, i64 0
  %t2 = add nsw i8* %t0, 0
  %t3 = call i32 @println(i8* %t2)
  %t4 = load i8*, i8** %title.addr
  %t5 = call i32 @println(i8* %t4)
  %t6 = getelementptr inbounds [2 x i8], [2 x i8]* @.str1, i64 0, i64 0
  %t7 = call i32 @println(i32 0)
  ret void
}


define void @printSection(i32 %arg0, i8* %arg1) {
  %num.addr = alloca i32
  store i32 %arg0, i32* %num.addr
  %title.addr = alloca i8*
  store i8* %arg1, i8** %title.addr
  %t0 = getelementptr inbounds [2 x i8], [2 x i8]* @.str0, i64 0, i64 0
  %t1 = load i32, i32* %num.addr
  %t2 = add nsw i8* %t0, %t1
  %t3 = getelementptr inbounds [3 x i8], [3 x i8]* @.str2, i64 0, i64 0
  %t4 = add nsw i8* %t2, %t3
  %t5 = load i8*, i8** %title.addr
  %t6 = add nsw i8* %t4, %t5
  %t7 = call i32 @println(i8* %t6)
  %t8 = getelementptr inbounds [2 x i8], [2 x i8]* @.str3, i64 0, i64 0
  %t9 = call i32 @println(i32 0)
  ret void
}


define void @main() {
  %t0 = getelementptr inbounds [30 x i8], [30 x i8]* @.str4, i64 0, i64 0
  %t1 = call i32 @printHeader(i8* %t0)
  %t2 = getelementptr inbounds [34 x i8], [34 x i8]* @.str5, i64 0, i64 0
  %t3 = call i32 @printSection(i32 1, i8* %t2)
  %text_4 = alloca i32
  %t5 = getelementptr inbounds [36 x i8], [36 x i8]* @.str6, i64 0, i64 0
  store i32 %t5, i32* %text_4
  %fruits_6 = alloca i32
  %t7 = getelementptr inbounds [2 x i8], [2 x i8]* @.str7, i64 0, i64 0
  %t8 = load i32, i32* %text_4
  store i32 0, i32* %fruits_6
  %t9 = getelementptr inbounds [16 x i8], [16 x i8]* @.str8, i64 0, i64 0
  %t10 = load i32, i32* %text_4
  %t11 = add nsw i8* %t9, %t10
  %t12 = call i32 @println(i8* %t11)
  %t13 = getelementptr inbounds [26 x i8], [26 x i8]* @.str9, i64 0, i64 0
  %t14 = call i32 @println(i8* %t13)
  %t15 = getelementptr inbounds [15 x i8], [15 x i8]* @.str10, i64 0, i64 0
  %t16 = load i32, i32* %fruits_6
  %t17 = add nsw i8* %t15, 0
  %t18 = call i32 @println(i8* %t17)
  %t19 = getelementptr inbounds [14 x i8], [14 x i8]* @.str11, i64 0, i64 0
  %t20 = add nsw i8* %t19, %idx_placeholder
  %t21 = call i32 @println(i8* %t20)
  %t22 = getelementptr inbounds [13 x i8], [13 x i8]* @.str12, i64 0, i64 0
  %t23 = add nsw i8* %t22, %idx_placeholder
  %t24 = call i32 @println(i8* %t23)
  %t25 = getelementptr inbounds [26 x i8], [26 x i8]* @.str13, i64 0, i64 0
  %t26 = call i32 @printSection(i32 2, i8* %t25)
  %colors_27 = alloca i32
  %t28 = getelementptr inbounds [2 x i8], [2 x i8]* @.str7, i64 0, i64 0
  %t29 = getelementptr inbounds [29 x i8], [29 x i8]* @.str14, i64 0, i64 0
  store i32 0, i32* %colors_27
  %t30 = getelementptr inbounds [18 x i8], [18 x i8]* @.str15, i64 0, i64 0
  %t31 = load i32, i32* %colors_27
  %t32 = add nsw i8* %t30, 0
  %t33 = getelementptr inbounds [10 x i8], [10 x i8]* @.str16, i64 0, i64 0
  %t34 = add nsw i8* %t32, %t33
  %t35 = call i32 @println(i8* %t34)
  %t36 = getelementptr inbounds [1 x i8], [1 x i8]* @.str17, i64 0, i64 0
  %t37 = call i32 @println(i8* %t36)
  %t38 = getelementptr inbounds [24 x i8], [24 x i8]* @.str18, i64 0, i64 0
  %t39 = call i32 @println(i8* %t38)
  %t40 = getelementptr inbounds [9 x i8], [9 x i8]* @.str19, i64 0, i64 0
  %t41 = add nsw i8* %t40, %idx_placeholder
  %t42 = call i32 @println(i8* %t41)
  %t43 = getelementptr inbounds [9 x i8], [9 x i8]* @.str20, i64 0, i64 0
  %t44 = add nsw i8* %t43, %idx_placeholder
  %t45 = call i32 @println(i8* %t44)
  %t46 = getelementptr inbounds [9 x i8], [9 x i8]* @.str21, i64 0, i64 0
  %t47 = add nsw i8* %t46, %idx_placeholder
  %t48 = call i32 @println(i8* %t47)
  %t49 = getelementptr inbounds [9 x i8], [9 x i8]* @.str22, i64 0, i64 0
  %t50 = add nsw i8* %t49, %idx_placeholder
  %t51 = call i32 @println(i8* %t50)
  %t52 = getelementptr inbounds [9 x i8], [9 x i8]* @.str23, i64 0, i64 0
  %t53 = add nsw i8* %t52, %idx_placeholder
  %t54 = call i32 @println(i8* %t53)
  %t55 = getelementptr inbounds [27 x i8], [27 x i8]* @.str24, i64 0, i64 0
  %t56 = call i32 @printSection(i32 3, i8* %t55)
  %sentence_57 = alloca i32
  %t58 = getelementptr inbounds [26 x i8], [26 x i8]* @.str25, i64 0, i64 0
  store i32 %t58, i32* %sentence_57
  %words_59 = alloca i32
  %t60 = getelementptr inbounds [2 x i8], [2 x i8]* @.str26, i64 0, i64 0
  %t61 = load i32, i32* %sentence_57
  store i32 0, i32* %words_59
  %t62 = getelementptr inbounds [11 x i8], [11 x i8]* @.str27, i64 0, i64 0
  %t63 = load i32, i32* %sentence_57
  %t64 = add nsw i8* %t62, %t63
  %t65 = call i32 @println(i8* %t64)
  %t66 = getelementptr inbounds [14 x i8], [14 x i8]* @.str28, i64 0, i64 0
  %t67 = load i32, i32* %words_59
  %t68 = add nsw i8* %t66, 0
  %t69 = call i32 @println(i8* %t68)
  %t70 = getelementptr inbounds [13 x i8], [13 x i8]* @.str29, i64 0, i64 0
  %t71 = add nsw i8* %t70, %idx_placeholder
  %t72 = call i32 @println(i8* %t71)
  %t73 = getelementptr inbounds [12 x i8], [12 x i8]* @.str30, i64 0, i64 0
  %t74 = add nsw i8* %t73, %idx_placeholder
  %t75 = call i32 @println(i8* %t74)
  %multiline_76 = alloca i32
  %t77 = getelementptr inbounds [21 x i8], [21 x i8]* @.str31, i64 0, i64 0
  store i32 %t77, i32* %multiline_76
  %lines_78 = alloca i32
  %t79 = getelementptr inbounds [2 x i8], [2 x i8]* @.str0, i64 0, i64 0
  %t80 = load i32, i32* %multiline_76
  store i32 0, i32* %lines_78
  %t81 = getelementptr inbounds [21 x i8], [21 x i8]* @.str32, i64 0, i64 0
  %t82 = load i32, i32* %lines_78
  %t83 = add nsw i8* %t81, 0
  %t84 = getelementptr inbounds [7 x i8], [7 x i8]* @.str33, i64 0, i64 0
  %t85 = add nsw i8* %t83, %t84
  %t86 = call i32 @println(i8* %t85)
  %t87 = getelementptr inbounds [13 x i8], [13 x i8]* @.str34, i64 0, i64 0
  %t88 = add nsw i8* %t87, %idx_placeholder
  %t89 = call i32 @println(i8* %t88)
  %data_90 = alloca i32
  %t91 = getelementptr inbounds [22 x i8], [22 x i8]* @.str35, i64 0, i64 0
  store i32 %t91, i32* %data_90
  %fields_92 = alloca i32
  %t93 = getelementptr inbounds [2 x i8], [2 x i8]* @.str36, i64 0, i64 0
  %t94 = load i32, i32* %data_90
  store i32 0, i32* %fields_92
  %t95 = getelementptr inbounds [15 x i8], [15 x i8]* @.str37, i64 0, i64 0
  %t96 = load i32, i32* %fields_92
  %t97 = add nsw i8* %t95, 0
  %t98 = call i32 @println(i8* %t97)
  %t99 = getelementptr inbounds [10 x i8], [10 x i8]* @.str38, i64 0, i64 0
  %t100 = add nsw i8* %t99, %idx_placeholder
  %t101 = call i32 @println(i8* %t100)
  %t102 = getelementptr inbounds [10 x i8], [10 x i8]* @.str39, i64 0, i64 0
  %t103 = add nsw i8* %t102, %idx_placeholder
  %t104 = call i32 @println(i8* %t103)
  %t105 = getelementptr inbounds [38 x i8], [38 x i8]* @.str40, i64 0, i64 0
  %t106 = call i32 @printSection(i32 4, i8* %t105)
  %single_107 = alloca i32
  %t108 = getelementptr inbounds [2 x i8], [2 x i8]* @.str7, i64 0, i64 0
  %t109 = getelementptr inbounds [14 x i8], [14 x i8]* @.str41, i64 0, i64 0
  store i32 0, i32* %single_107
  %t110 = getelementptr inbounds [34 x i8], [34 x i8]* @.str42, i64 0, i64 0
  %t111 = call i32 @println(i8* %t110)
  %t112 = getelementptr inbounds [11 x i8], [11 x i8]* @.str43, i64 0, i64 0
  %t113 = load i32, i32* %single_107
  %t114 = add nsw i8* %t112, 0
  %t115 = call i32 @println(i8* %t114)
  %t116 = getelementptr inbounds [9 x i8], [9 x i8]* @.str44, i64 0, i64 0
  %t117 = add nsw i8* %t116, %idx_placeholder
  %t118 = call i32 @println(i8* %t117)
  %withEmpty_119 = alloca i32
  %t120 = getelementptr inbounds [2 x i8], [2 x i8]* @.str7, i64 0, i64 0
  %t121 = getelementptr inbounds [5 x i8], [5 x i8]* @.str45, i64 0, i64 0
  store i32 0, i32* %withEmpty_119
  %t122 = getelementptr inbounds [35 x i8], [35 x i8]* @.str46, i64 0, i64 0
  %t123 = call i32 @println(i8* %t122)
  %t124 = getelementptr inbounds [11 x i8], [11 x i8]* @.str43, i64 0, i64 0
  %t125 = load i32, i32* %withEmpty_119
  %t126 = add nsw i8* %t124, 0
  %t127 = call i32 @println(i8* %t126)
  %t128 = getelementptr inbounds [9 x i8], [9 x i8]* @.str47, i64 0, i64 0
  %t129 = add nsw i8* %t128, %idx_placeholder
  %t130 = getelementptr inbounds [2 x i8], [2 x i8]* @.str48, i64 0, i64 0
  %t131 = add nsw i8* %t129, %t130
  %t132 = call i32 @println(i8* %t131)
  %t133 = getelementptr inbounds [9 x i8], [9 x i8]* @.str49, i64 0, i64 0
  %t134 = add nsw i8* %t133, %idx_placeholder
  %t135 = getelementptr inbounds [10 x i8], [10 x i8]* @.str50, i64 0, i64 0
  %t136 = add nsw i8* %t134, %t135
  %t137 = call i32 @println(i8* %t136)
  %t138 = getelementptr inbounds [9 x i8], [9 x i8]* @.str51, i64 0, i64 0
  %t139 = add nsw i8* %t138, %idx_placeholder
  %t140 = getelementptr inbounds [2 x i8], [2 x i8]* @.str48, i64 0, i64 0
  %t141 = add nsw i8* %t139, %t140
  %t142 = call i32 @println(i8* %t141)
  %t143 = getelementptr inbounds [20 x i8], [20 x i8]* @.str52, i64 0, i64 0
  %t144 = call i32 @printSection(i32 5, i8* %t143)
  %small_145 = alloca i32
  %t146 = getelementptr inbounds [2 x i8], [2 x i8]* @.str7, i64 0, i64 0
  %t147 = getelementptr inbounds [4 x i8], [4 x i8]* @.str53, i64 0, i64 0
  store i32 0, i32* %small_145
  %medium_148 = alloca i32
  %t149 = getelementptr inbounds [2 x i8], [2 x i8]* @.str7, i64 0, i64 0
  %t150 = getelementptr inbounds [10 x i8], [10 x i8]* @.str54, i64 0, i64 0
  store i32 0, i32* %medium_148
  %large_151 = alloca i32
  %t152 = getelementptr inbounds [2 x i8], [2 x i8]* @.str7, i64 0, i64 0
  %t153 = getelementptr inbounds [20 x i8], [20 x i8]* @.str55, i64 0, i64 0
  store i32 0, i32* %large_151
  %t154 = getelementptr inbounds [21 x i8], [21 x i8]* @.str56, i64 0, i64 0
  %t155 = load i32, i32* %small_145
  %t156 = add nsw i8* %t154, 0
  %t157 = call i32 @println(i8* %t156)
  %t158 = getelementptr inbounds [22 x i8], [22 x i8]* @.str57, i64 0, i64 0
  %t159 = load i32, i32* %medium_148
  %t160 = add nsw i8* %t158, 0
  %t161 = call i32 @println(i8* %t160)
  %t162 = getelementptr inbounds [21 x i8], [21 x i8]* @.str58, i64 0, i64 0
  %t163 = load i32, i32* %large_151
  %t164 = add nsw i8* %t162, 0
  %t165 = call i32 @println(i8* %t164)
  %t166 = getelementptr inbounds [38 x i8], [38 x i8]* @.str59, i64 0, i64 0
  %t167 = call i32 @printSection(i32 6, i8* %t166)
  %csvHeader_168 = alloca i32
  %t169 = getelementptr inbounds [25 x i8], [25 x i8]* @.str60, i64 0, i64 0
  store i32 %t169, i32* %csvHeader_168
  %csvRow1_170 = alloca i32
  %t171 = getelementptr inbounds [22 x i8], [22 x i8]* @.str61, i64 0, i64 0
  store i32 %t171, i32* %csvRow1_170
  %csvRow2_172 = alloca i32
  %t173 = getelementptr inbounds [19 x i8], [19 x i8]* @.str62, i64 0, i64 0
  store i32 %t173, i32* %csvRow2_172
  %csvRow3_174 = alloca i32
  %t175 = getelementptr inbounds [22 x i8], [22 x i8]* @.str63, i64 0, i64 0
  store i32 %t175, i32* %csvRow3_174
  %headers_176 = alloca i32
  %t177 = getelementptr inbounds [2 x i8], [2 x i8]* @.str7, i64 0, i64 0
  %t178 = load i32, i32* %csvHeader_168
  store i32 0, i32* %headers_176
  %row1_179 = alloca i32
  %t180 = getelementptr inbounds [2 x i8], [2 x i8]* @.str7, i64 0, i64 0
  %t181 = load i32, i32* %csvRow1_170
  store i32 0, i32* %row1_179
  %row2_182 = alloca i32
  %t183 = getelementptr inbounds [2 x i8], [2 x i8]* @.str7, i64 0, i64 0
  %t184 = load i32, i32* %csvRow2_172
  store i32 0, i32* %row2_182
  %row3_185 = alloca i32
  %t186 = getelementptr inbounds [2 x i8], [2 x i8]* @.str7, i64 0, i64 0
  %t187 = load i32, i32* %csvRow3_174
  store i32 0, i32* %row3_185
  %t188 = getelementptr inbounds [21 x i8], [21 x i8]* @.str64, i64 0, i64 0
  %t189 = call i32 @println(i8* %t188)
  %t190 = getelementptr inbounds [1 x i8], [1 x i8]* @.str17, i64 0, i64 0
  %t191 = call i32 @println(i8* %t190)
  %t192 = getelementptr inbounds [10 x i8], [10 x i8]* @.str65, i64 0, i64 0
  %t193 = load i32, i32* %headers_176
  %t194 = add nsw i8* %t192, 0
  %t195 = getelementptr inbounds [11 x i8], [11 x i8]* @.str66, i64 0, i64 0
  %t196 = add nsw i8* %t194, %t195
  %t197 = call i32 @println(i8* %t196)
  %t198 = getelementptr inbounds [3 x i8], [3 x i8]* @.str67, i64 0, i64 0
  %t199 = add nsw i8* %t198, %idx_placeholder
  %t200 = getelementptr inbounds [4 x i8], [4 x i8]* @.str68, i64 0, i64 0
  %t201 = add nsw i8* %t199, %t200
  %t202 = add nsw i8* %t201, %idx_placeholder
  %t203 = getelementptr inbounds [4 x i8], [4 x i8]* @.str68, i64 0, i64 0
  %t204 = add nsw i8* %t202, %t203
  %t205 = add nsw i8* %t204, %idx_placeholder
  %t206 = getelementptr inbounds [4 x i8], [4 x i8]* @.str68, i64 0, i64 0
  %t207 = add nsw i8* %t205, %t206
  %t208 = add nsw i8* %t207, %idx_placeholder
  %t209 = call i32 @println(i8* %t208)
  %t210 = getelementptr inbounds [1 x i8], [1 x i8]* @.str17, i64 0, i64 0
  %t211 = call i32 @println(i8* %t210)
  %t212 = getelementptr inbounds [7 x i8], [7 x i8]* @.str69, i64 0, i64 0
  %t213 = call i32 @println(i8* %t212)
  %t214 = getelementptr inbounds [3 x i8], [3 x i8]* @.str67, i64 0, i64 0
  %t215 = add nsw i8* %t214, %idx_placeholder
  %t216 = getelementptr inbounds [4 x i8], [4 x i8]* @.str68, i64 0, i64 0
  %t217 = add nsw i8* %t215, %t216
  %t218 = add nsw i8* %t217, %idx_placeholder
  %t219 = getelementptr inbounds [4 x i8], [4 x i8]* @.str68, i64 0, i64 0
  %t220 = add nsw i8* %t218, %t219
  %t221 = add nsw i8* %t220, %idx_placeholder
  %t222 = getelementptr inbounds [4 x i8], [4 x i8]* @.str68, i64 0, i64 0
  %t223 = add nsw i8* %t221, %t222
  %t224 = add nsw i8* %t223, %idx_placeholder
  %t225 = call i32 @println(i8* %t224)
  %t226 = getelementptr inbounds [7 x i8], [7 x i8]* @.str70, i64 0, i64 0
  %t227 = call i32 @println(i8* %t226)
  %t228 = getelementptr inbounds [3 x i8], [3 x i8]* @.str67, i64 0, i64 0
  %t229 = add nsw i8* %t228, %idx_placeholder
  %t230 = getelementptr inbounds [4 x i8], [4 x i8]* @.str68, i64 0, i64 0
  %t231 = add nsw i8* %t229, %t230
  %t232 = add nsw i8* %t231, %idx_placeholder
  %t233 = getelementptr inbounds [4 x i8], [4 x i8]* @.str68, i64 0, i64 0
  %t234 = add nsw i8* %t232, %t233
  %t235 = add nsw i8* %t234, %idx_placeholder
  %t236 = getelementptr inbounds [4 x i8], [4 x i8]* @.str68, i64 0, i64 0
  %t237 = add nsw i8* %t235, %t236
  %t238 = add nsw i8* %t237, %idx_placeholder
  %t239 = call i32 @println(i8* %t238)
  %t240 = getelementptr inbounds [7 x i8], [7 x i8]* @.str71, i64 0, i64 0
  %t241 = call i32 @println(i8* %t240)
  %t242 = getelementptr inbounds [3 x i8], [3 x i8]* @.str67, i64 0, i64 0
  %t243 = add nsw i8* %t242, %idx_placeholder
  %t244 = getelementptr inbounds [4 x i8], [4 x i8]* @.str68, i64 0, i64 0
  %t245 = add nsw i8* %t243, %t244
  %t246 = add nsw i8* %t245, %idx_placeholder
  %t247 = getelementptr inbounds [4 x i8], [4 x i8]* @.str68, i64 0, i64 0
  %t248 = add nsw i8* %t246, %t247
  %t249 = add nsw i8* %t248, %idx_placeholder
  %t250 = getelementptr inbounds [4 x i8], [4 x i8]* @.str68, i64 0, i64 0
  %t251 = add nsw i8* %t249, %t250
  %t252 = add nsw i8* %t251, %idx_placeholder
  %t253 = call i32 @println(i8* %t252)
  %t254 = getelementptr inbounds [35 x i8], [35 x i8]* @.str72, i64 0, i64 0
  %t255 = call i32 @printSection(i32 7, i8* %t254)
  %testData_256 = alloca i32
  %t257 = getelementptr inbounds [57 x i8], [57 x i8]* @.str73, i64 0, i64 0
  store i32 %t257, i32* %testData_256
  %t258 = getelementptr inbounds [9 x i8], [9 x i8]* @.str74, i64 0, i64 0
  %t259 = load i32, i32* %testData_256
  %t260 = call i32 @__native_io_writeFile(i8* %t258, i32 %t259)
  %t261 = getelementptr inbounds [33 x i8], [33 x i8]* @.str75, i64 0, i64 0
  %t262 = call i32 @println(i8* %t261)
  %fileContent_263 = alloca i32
  %t264 = getelementptr inbounds [9 x i8], [9 x i8]* @.str74, i64 0, i64 0
  %t265 = call i8* @__native_io_readFile(i8* %t264)
  store i32 %t265, i32* %fileContent_263
  %days_266 = alloca i32
  %t267 = getelementptr inbounds [2 x i8], [2 x i8]* @.str0, i64 0, i64 0
  %t268 = load i32, i32* %fileContent_263
  store i32 0, i32* %days_266
  %t269 = getelementptr inbounds [19 x i8], [19 x i8]* @.str76, i64 0, i64 0
  %t270 = load i32, i32* %days_266
  %t271 = add nsw i8* %t269, 0
  %t272 = call i32 @println(i8* %t271)
  %t273 = getelementptr inbounds [12 x i8], [12 x i8]* @.str77, i64 0, i64 0
  %t274 = add nsw i8* %t273, %idx_placeholder
  %t275 = call i32 @println(i8* %t274)
  %t276 = getelementptr inbounds [10 x i8], [10 x i8]* @.str78, i64 0, i64 0
  %t277 = add nsw i8* %t276, %idx_placeholder
  %t278 = call i32 @println(i8* %t277)
  %t279 = getelementptr inbounds [17 x i8], [17 x i8]* @.str79, i64 0, i64 0
  %t280 = add nsw i8* %t279, %idx_placeholder
  %t281 = call i32 @println(i8* %t280)
  %t282 = getelementptr inbounds [9 x i8], [9 x i8]* @.str74, i64 0, i64 0
  %t283 = call i32 @__native_io_remove(i8* %t282)
  %t284 = getelementptr inbounds [24 x i8], [24 x i8]* @.str80, i64 0, i64 0
  %t285 = call i32 @println(i8* %t284)
  %t286 = getelementptr inbounds [23 x i8], [23 x i8]* @.str81, i64 0, i64 0
  %t287 = call i32 @printSection(i32 8, i8* %t286)
  %path_288 = alloca i32
  %t289 = getelementptr inbounds [29 x i8], [29 x i8]* @.str82, i64 0, i64 0
  store i32 %t289, i32* %path_288
  %directories_290 = alloca i32
  %t291 = getelementptr inbounds [2 x i8], [2 x i8]* @.str83, i64 0, i64 0
  %t292 = load i32, i32* %path_288
  store i32 0, i32* %directories_290
  %t293 = getelementptr inbounds [14 x i8], [14 x i8]* @.str84, i64 0, i64 0
  %t294 = call i32 @println(i8* %t293)
  %t295 = getelementptr inbounds [22 x i8], [22 x i8]* @.str85, i64 0, i64 0
  %t296 = load i32, i32* %directories_290
  %t297 = add nsw i8* %t295, 0
  %t298 = call i32 @println(i8* %t297)
  %t299 = getelementptr inbounds [20 x i8], [20 x i8]* @.str86, i64 0, i64 0
  %t300 = add nsw i8* %t299, %idx_placeholder
  %t301 = call i32 @println(i8* %t300)
  %tags_302 = alloca i32
  %t303 = getelementptr inbounds [2 x i8], [2 x i8]* @.str7, i64 0, i64 0
  %t304 = getelementptr inbounds [41 x i8], [41 x i8]* @.str87, i64 0, i64 0
  store i32 0, i32* %tags_302
  %t305 = getelementptr inbounds [18 x i8], [18 x i8]* @.str88, i64 0, i64 0
  %t306 = load i32, i32* %tags_302
  %t307 = add nsw i8* %t305, 0
  %t308 = getelementptr inbounds [6 x i8], [6 x i8]* @.str89, i64 0, i64 0
  %t309 = add nsw i8* %t307, %t308
  %t310 = call i32 @println(i8* %t309)
  %t311 = getelementptr inbounds [16 x i8], [16 x i8]* @.str90, i64 0, i64 0
  %t312 = add nsw i8* %t311, %idx_placeholder
  %t313 = call i32 @println(i8* %t312)
  %t314 = getelementptr inbounds [13 x i8], [13 x i8]* @.str91, i64 0, i64 0
  %t315 = add nsw i8* %t314, %idx_placeholder
  %t316 = call i32 @println(i8* %t315)
  %version_317 = alloca i32
  %t318 = getelementptr inbounds [2 x i8], [2 x i8]* @.str92, i64 0, i64 0
  %t319 = getelementptr inbounds [6 x i8], [6 x i8]* @.str93, i64 0, i64 0
  store i32 0, i32* %version_317
  %t320 = getelementptr inbounds [18 x i8], [18 x i8]* @.str94, i64 0, i64 0
  %t321 = call i32 @println(i8* %t320)
  %t322 = getelementptr inbounds [10 x i8], [10 x i8]* @.str95, i64 0, i64 0
  %t323 = add nsw i8* %t322, %idx_placeholder
  %t324 = call i32 @println(i8* %t323)
  %t325 = getelementptr inbounds [10 x i8], [10 x i8]* @.str96, i64 0, i64 0
  %t326 = add nsw i8* %t325, %idx_placeholder
  %t327 = call i32 @println(i8* %t326)
  %t328 = getelementptr inbounds [10 x i8], [10 x i8]* @.str97, i64 0, i64 0
  %t329 = add nsw i8* %t328, %idx_placeholder
  %t330 = call i32 @println(i8* %t329)
  %t331 = getelementptr inbounds [25 x i8], [25 x i8]* @.str98, i64 0, i64 0
  %t332 = call i32 @printHeader(i8* %t331)
  %t333 = getelementptr inbounds [51 x i8], [51 x i8]* @.str99, i64 0, i64 0
  %t334 = call i32 @println(i8* %t333)
  %t335 = getelementptr inbounds [44 x i8], [44 x i8]* @.str100, i64 0, i64 0
  %t336 = call i32 @println(i8* %t335)
  %t337 = getelementptr inbounds [46 x i8], [46 x i8]* @.str101, i64 0, i64 0
  %t338 = call i32 @println(i8* %t337)
  %t339 = getelementptr inbounds [52 x i8], [52 x i8]* @.str102, i64 0, i64 0
  %t340 = call i32 @println(i8* %t339)
  %t341 = getelementptr inbounds [60 x i8], [60 x i8]* @.str103, i64 0, i64 0
  %t342 = call i32 @println(i8* %t341)
  %t343 = getelementptr inbounds [2 x i8], [2 x i8]* @.str0, i64 0, i64 0
  %t344 = getelementptr inbounds [2 x i8], [2 x i8]* @.str1, i64 0, i64 0
  %t345 = add nsw i8* %t343, 0
  %t346 = call i32 @println(i8* %t345)
  %t347 = getelementptr inbounds [45 x i8], [45 x i8]* @.str104, i64 0, i64 0
  %t348 = call i32 @println(i8* %t347)
  %t349 = getelementptr inbounds [2 x i8], [2 x i8]* @.str1, i64 0, i64 0
  %t350 = call i32 @println(i32 0)
  ret void
}


define i32 @main() {
  ret i32 0
}


; String Literals
@.str104 = private unnamed_addr constant [45 x i8] c"All array operations completed successfully!\00"
@.str103 = private unnamed_addr constant [60 x i8] c"✓ Use Cases: CSV parsing, file processing, data splitting\00"
@.str102 = private unnamed_addr constant [52 x i8] c"✓ Array Types: Currently supporting string arrays\00"
@.str99 = private unnamed_addr constant [51 x i8] c"
✓ Array Creation: Using string.split(delimiter)\00"
@.str97 = private unnamed_addr constant [10 x i8] c"  Patch: \00"
@.str92 = private unnamed_addr constant [2 x i8] c".\00"
@.str91 = private unnamed_addr constant [13 x i8] c"  Last tag: \00"
@.str85 = private unnamed_addr constant [22 x i8] c"  Total directories: \00"
@.str83 = private unnamed_addr constant [2 x i8] c":\00"
@.str82 = private unnamed_addr constant [29 x i8] c"/usr/local/bin:/usr/bin:/bin\00"
@.str79 = private unnamed_addr constant [17 x i8] c"Weekend starts: \00"
@.str77 = private unnamed_addr constant [12 x i8] c"First day: \00"
@.str76 = private unnamed_addr constant [19 x i8] c"Days in the file: \00"
@.str75 = private unnamed_addr constant [33 x i8] c"✓ Created days.txt with 7 days\00"
@.str98 = private unnamed_addr constant [25 x i8] c"Array Operations Summary\00"
@.str84 = private unnamed_addr constant [14 x i8] c"PATH parsing:\00"
@.str74 = private unnamed_addr constant [9 x i8] c"days.txt\00"
@.str73 = private unnamed_addr constant [57 x i8] c"Monday
Tuesday
Wednesday
Thursday
Friday
Saturday
Sunday\00"
@.str94 = private unnamed_addr constant [18 x i8] c"
Version parsing:\00"
@.str80 = private unnamed_addr constant [24 x i8] c"✓ Cleaned up days.txt\00"
@.str72 = private unnamed_addr constant [35 x i8] c"Processing File Contents as Arrays\00"
@.str71 = private unnamed_addr constant [7 x i8] c"Row 3:\00"
@.str65 = private unnamed_addr constant [10 x i8] c"Headers (\00"
@.str90 = private unnamed_addr constant [16 x i8] c"  Primary tag: \00"
@.str78 = private unnamed_addr constant [10 x i8] c"Midweek: \00"
@.str64 = private unnamed_addr constant [21 x i8] c"CSV Data Processing:\00"
@.str62 = private unnamed_addr constant [19 x i8] c"Bob,25,LA,Designer\00"
@.str61 = private unnamed_addr constant [22 x i8] c"Alice,30,NYC,Engineer\00"
@.str60 = private unnamed_addr constant [25 x i8] c"Name,Age,City,Occupation\00"
@.str59 = private unnamed_addr constant [38 x i8] c"Practical Example: CSV Row Processing\00"
@.str81 = private unnamed_addr constant [23 x i8] c"Common Array Use Cases\00"
@.str23 = private unnamed_addr constant [9 x i8] c"  [4] = \00"
@.str45 = private unnamed_addr constant [5 x i8] c"a,,b\00"
@.str21 = private unnamed_addr constant [9 x i8] c"  [2] = \00"
@.str56 = private unnamed_addr constant [21 x i8] c"Small array length: \00"
@.str28 = private unnamed_addr constant [14 x i8] c"Words count: \00"
@.str20 = private unnamed_addr constant [9 x i8] c"  [1] = \00"
@.str39 = private unnamed_addr constant [10 x i8] c"Field 1: \00"
@.str57 = private unnamed_addr constant [22 x i8] c"Medium array length: \00"
@.str19 = private unnamed_addr constant [9 x i8] c"  [0] = \00"
@.str96 = private unnamed_addr constant [10 x i8] c"  Minor: \00"
@.str12 = private unnamed_addr constant [13 x i8] c"Last fruit: \00"
@.str15 = private unnamed_addr constant [18 x i8] c"Colors array has \00"
@.str13 = private unnamed_addr constant [26 x i8] c"Array Indexing and Access\00"
@.str46 = private unnamed_addr constant [35 x i8] c"
Array with empty elements (a,,b):\00"
@.str0 = private unnamed_addr constant [2 x i8] c"
\00"
@.str16 = private unnamed_addr constant [10 x i8] c" elements\00"
@.str25 = private unnamed_addr constant [26 x i8] c"The quick brown fox jumps\00"
@.str18 = private unnamed_addr constant [24 x i8] c"Accessing each element:\00"
@.str44 = private unnamed_addr constant [9 x i8] c"  Item: \00"
@.str5 = private unnamed_addr constant [34 x i8] c"Creating Arrays from String Split\00"
@.str4 = private unnamed_addr constant [30 x i8] c"Stratos Array Operations Demo\00"
@.str24 = private unnamed_addr constant [27 x i8] c"Different Split Delimiters\00"
@.str100 = private unnamed_addr constant [44 x i8] c"✓ Array Access: Using array[index] syntax\00"
@.str6 = private unnamed_addr constant [36 x i8] c"apple,banana,cherry,date,elderberry\00"
@.str53 = private unnamed_addr constant [4 x i8] c"a,b\00"
@.str87 = private unnamed_addr constant [41 x i8] c"stratos,programming,arrays,tutorial,demo\00"
@.str2 = private unnamed_addr constant [3 x i8] c". \00"
@.str89 = private unnamed_addr constant [6 x i8] c" tags\00"
@.str27 = private unnamed_addr constant [11 x i8] c"Sentence: \00"
@.str1 = private unnamed_addr constant [2 x i8] c"=\00"
@.str26 = private unnamed_addr constant [2 x i8] c" \00"
@.str7 = private unnamed_addr constant [2 x i8] c",\00"
@.str70 = private unnamed_addr constant [7 x i8] c"Row 2:\00"
@.str9 = private unnamed_addr constant [26 x i8] c"Split by comma into array\00"
@.str8 = private unnamed_addr constant [16 x i8] c"Original text: \00"
@.str17 = private unnamed_addr constant [1 x i8] c"\00"
@.str14 = private unnamed_addr constant [29 x i8] c"red,green,blue,yellow,purple\00"
@.str42 = private unnamed_addr constant [34 x i8] c"Single item (no delimiter found):\00"
@.str33 = private unnamed_addr constant [7 x i8] c" lines\00"
@.str11 = private unnamed_addr constant [14 x i8] c"First fruit: \00"
@.str29 = private unnamed_addr constant [13 x i8] c"First word: \00"
@.str93 = private unnamed_addr constant [6 x i8] c"1.2.3\00"
@.str68 = private unnamed_addr constant [4 x i8] c" | \00"
@.str10 = private unnamed_addr constant [15 x i8] c"Array length: \00"
@.str30 = private unnamed_addr constant [12 x i8] c"Last word: \00"
@.str31 = private unnamed_addr constant [21 x i8] c"Line 1
Line 2
Line 3\00"
@.str101 = private unnamed_addr constant [46 x i8] c"✓ Array Length: Using array.length() method\00"
@.str40 = private unnamed_addr constant [38 x i8] c"Edge Cases: Empty and Single Elements\00"
@.str69 = private unnamed_addr constant [7 x i8] c"Row 1:\00"
@.str35 = private unnamed_addr constant [22 x i8] c"name|age|city|country\00"
@.str36 = private unnamed_addr constant [2 x i8] c"|\00"
@.str32 = private unnamed_addr constant [21 x i8] c"
Multiline text has \00"
@.str37 = private unnamed_addr constant [15 x i8] c"
Data fields: \00"
@.str38 = private unnamed_addr constant [10 x i8] c"Field 0: \00"
@.str66 = private unnamed_addr constant [11 x i8] c" columns):\00"
@.str41 = private unnamed_addr constant [14 x i8] c"only-one-item\00"
@.str22 = private unnamed_addr constant [9 x i8] c"  [3] = \00"
@.str49 = private unnamed_addr constant [9 x i8] c"  [1]: '\00"
@.str43 = private unnamed_addr constant [11 x i8] c"  Length: \00"
@.str67 = private unnamed_addr constant [3 x i8] c"  \00"
@.str34 = private unnamed_addr constant [13 x i8] c"First line: \00"
@.str47 = private unnamed_addr constant [9 x i8] c"  [0]: '\00"
@.str63 = private unnamed_addr constant [22 x i8] c"Charlie,35,SF,Manager\00"
@.str48 = private unnamed_addr constant [2 x i8] c"'\00"
@.str3 = private unnamed_addr constant [2 x i8] c"-\00"
@.str50 = private unnamed_addr constant [10 x i8] c"' (empty)\00"
@.str88 = private unnamed_addr constant [18 x i8] c"
Blog post tags: \00"
@.str51 = private unnamed_addr constant [9 x i8] c"  [2]: '\00"
@.str86 = private unnamed_addr constant [20 x i8] c"  First directory: \00"
@.str52 = private unnamed_addr constant [20 x i8] c"Array Length Method\00"
@.str95 = private unnamed_addr constant [10 x i8] c"  Major: \00"
@.str54 = private unnamed_addr constant [10 x i8] c"1,2,3,4,5\00"
@.str55 = private unnamed_addr constant [20 x i8] c"a,b,c,d,e,f,g,h,i,j\00"
@.str58 = private unnamed_addr constant [21 x i8] c"Large array length: \00"
