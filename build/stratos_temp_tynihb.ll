; ModuleID = 'stratos_module'
source_filename = "stratos_source"
target datalayout = "e-m:w-p270:32:32-p271:32:32-p272:64:64-i64:64-f80:128-n8:16:32:64-S128"
target triple = "x86_64-pc-windows-msvc"

declare i32 @printf(i8*, ...)

declare i8* @malloc(i64)

; Format specifiers for print
@.str_specifier = private unnamed_addr constant [4 x i8] c"%d\0A\00"

define void @printUtilsHeader(i8* %arg0) {
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


define void @printUtilsSection(i32 %arg0, i8* %arg1) {
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


define void @demoUtils() {
  %t0 = getelementptr inbounds [29 x i8], [29 x i8]* @.str4, i64 0, i64 0
  %t1 = call i32 @printUtilsHeader(i8* %t0)
  %t2 = getelementptr inbounds [31 x i8], [31 x i8]* @.str5, i64 0, i64 0
  %t3 = call i32 @printUtilsSection(i32 1, i8* %t2)
  %fruits_4 = alloca i32
  %t5 = getelementptr inbounds [2 x i8], [2 x i8]* @.str6, i64 0, i64 0
  %t6 = getelementptr inbounds [36 x i8], [36 x i8]* @.str7, i64 0, i64 0
  store i32 0, i32* %fruits_4
  %t7 = getelementptr inbounds [15 x i8], [15 x i8]* @.str8, i64 0, i64 0
  %t8 = load i32, i32* %fruits_4
  %t9 = add nsw i8* %t7, 0
  %t10 = getelementptr inbounds [7 x i8], [7 x i8]* @.str9, i64 0, i64 0
  %t11 = add nsw i8* %t9, %t10
  %t12 = call i32 @println(i8* %t11)
  %t13 = getelementptr inbounds [14 x i8], [14 x i8]* @.str10, i64 0, i64 0
  %t14 = load i32, i32* %fruits_4
  %t15 = add nsw i8* %t13, 0
  %t16 = call i32 @println(i8* %t15)
  %t17 = getelementptr inbounds [12 x i8], [12 x i8]* @.str11, i64 0, i64 0
  %t18 = load i32, i32* %fruits_4
  %t19 = add nsw i8* %t17, 0
  %t20 = call i32 @println(i8* %t19)
  %t21 = getelementptr inbounds [11 x i8], [11 x i8]* @.str12, i64 0, i64 0
  %t22 = load i32, i32* %fruits_4
  %t23 = add nsw i8* %t21, 0
  %t24 = call i32 @println(i8* %t23)
  %empty_25 = alloca i32
  %t26 = getelementptr inbounds [2 x i8], [2 x i8]* @.str6, i64 0, i64 0
  %t27 = getelementptr inbounds [1 x i8], [1 x i8]* @.str13, i64 0, i64 0
  store i32 0, i32* %empty_25
  %t28 = load i32, i32* %empty_25
  %t29 = load i32, i32* %empty_25
  %t30 = add i32 %t28, 0
  %t31 = getelementptr inbounds [14 x i8], [14 x i8]* @.str14, i64 0, i64 0
  %t32 = call i32 @println(i8* %t31)
  %t33 = getelementptr inbounds [14 x i8], [14 x i8]* @.str10, i64 0, i64 0
  %t34 = load i32, i32* %empty_25
  %t35 = add nsw i8* %t33, 0
  %t36 = call i32 @println(i8* %t35)
  %t37 = getelementptr inbounds [13 x i8], [13 x i8]* @.str15, i64 0, i64 0
  %t38 = load i32, i32* %empty_25
  %t39 = add nsw i8* %t37, 0
  %t40 = getelementptr inbounds [25 x i8], [25 x i8]* @.str16, i64 0, i64 0
  %t41 = add nsw i8* %t39, %t40
  %t42 = call i32 @println(i8* %t41)
  %t43 = getelementptr inbounds [12 x i8], [12 x i8]* @.str17, i64 0, i64 0
  %t44 = load i32, i32* %empty_25
  %t45 = add nsw i8* %t43, 0
  %t46 = getelementptr inbounds [25 x i8], [25 x i8]* @.str16, i64 0, i64 0
  %t47 = add nsw i8* %t45, %t46
  %t48 = call i32 @println(i8* %t47)
  %t49 = getelementptr inbounds [25 x i8], [25 x i8]* @.str18, i64 0, i64 0
  %t50 = call i32 @printUtilsSection(i32 2, i8* %t49)
  %colors_51 = alloca i32
  %t52 = getelementptr inbounds [2 x i8], [2 x i8]* @.str6, i64 0, i64 0
  %t53 = getelementptr inbounds [29 x i8], [29 x i8]* @.str19, i64 0, i64 0
  store i32 0, i32* %colors_51
  %t54 = getelementptr inbounds [9 x i8], [9 x i8]* @.str20, i64 0, i64 0
  %t55 = getelementptr inbounds [3 x i8], [3 x i8]* @.str21, i64 0, i64 0
  %t56 = load i32, i32* %colors_51
  %t57 = add nsw i8* %t54, 0
  %t58 = call i32 @println(i8* %t57)
  %t59 = getelementptr inbounds [23 x i8], [23 x i8]* @.str22, i64 0, i64 0
  %t60 = call i32 @println(i8* %t59)
  %t61 = getelementptr inbounds [21 x i8], [21 x i8]* @.str23, i64 0, i64 0
  %t62 = getelementptr inbounds [5 x i8], [5 x i8]* @.str24, i64 0, i64 0
  %t63 = load i32, i32* %colors_51
  %t64 = add nsw i8* %t61, 0
  %t65 = call i32 @println(i8* %t64)
  %t66 = getelementptr inbounds [20 x i8], [20 x i8]* @.str25, i64 0, i64 0
  %t67 = getelementptr inbounds [5 x i8], [5 x i8]* @.str24, i64 0, i64 0
  %t68 = load i32, i32* %colors_51
  %t69 = add nsw i8* %t66, 0
  %t70 = call i32 @println(i8* %t69)
  %t71 = getelementptr inbounds [23 x i8], [23 x i8]* @.str26, i64 0, i64 0
  %t72 = getelementptr inbounds [7 x i8], [7 x i8]* @.str27, i64 0, i64 0
  %t73 = load i32, i32* %colors_51
  %t74 = add nsw i8* %t71, 0
  %t75 = call i32 @println(i8* %t74)
  %t76 = getelementptr inbounds [22 x i8], [22 x i8]* @.str28, i64 0, i64 0
  %t77 = getelementptr inbounds [7 x i8], [7 x i8]* @.str27, i64 0, i64 0
  %t78 = load i32, i32* %colors_51
  %t79 = add nsw i8* %t76, 0
  %t80 = getelementptr inbounds [27 x i8], [27 x i8]* @.str29, i64 0, i64 0
  %t81 = add nsw i8* %t79, %t80
  %t82 = call i32 @println(i8* %t81)
  %t83 = getelementptr inbounds [10 x i8], [10 x i8]* @.str30, i64 0, i64 0
  %t84 = call i32 @printUtilsSection(i32 3, i8* %t83)
  %numbers_85 = alloca i32
  %t86 = getelementptr inbounds [2 x i8], [2 x i8]* @.str6, i64 0, i64 0
  %t87 = getelementptr inbounds [10 x i8], [10 x i8]* @.str31, i64 0, i64 0
  store i32 0, i32* %numbers_85
  %reversed_88 = alloca i32
  %t89 = load i32, i32* %numbers_85
  store i32 0, i32* %reversed_88
  %t90 = getelementptr inbounds [11 x i8], [11 x i8]* @.str32, i64 0, i64 0
  %t91 = getelementptr inbounds [5 x i8], [5 x i8]* @.str33, i64 0, i64 0
  %t92 = load i32, i32* %numbers_85
  %t93 = add nsw i8* %t90, 0
  %t94 = call i32 @println(i8* %t93)
  %t95 = getelementptr inbounds [11 x i8], [11 x i8]* @.str34, i64 0, i64 0
  %t96 = getelementptr inbounds [5 x i8], [5 x i8]* @.str33, i64 0, i64 0
  %t97 = load i32, i32* %reversed_88
  %t98 = add nsw i8* %t95, 0
  %t99 = call i32 @println(i8* %t98)
  %words_100 = alloca i32
  %t101 = getelementptr inbounds [2 x i8], [2 x i8]* @.str6, i64 0, i64 0
  %t102 = getelementptr inbounds [25 x i8], [25 x i8]* @.str35, i64 0, i64 0
  store i32 0, i32* %words_100
  %t103 = getelementptr inbounds [18 x i8], [18 x i8]* @.str36, i64 0, i64 0
  %t104 = getelementptr inbounds [2 x i8], [2 x i8]* @.str37, i64 0, i64 0
  %t105 = load i32, i32* %words_100
  %t106 = add nsw i8* %t103, 0
  %t107 = call i32 @println(i8* %t106)
  %t108 = getelementptr inbounds [17 x i8], [17 x i8]* @.str38, i64 0, i64 0
  %t109 = getelementptr inbounds [2 x i8], [2 x i8]* @.str37, i64 0, i64 0
  %t110 = load i32, i32* %words_100
  %t111 = add nsw i8* %t108, 0
  %t112 = call i32 @println(i8* %t111)
  %t113 = getelementptr inbounds [30 x i8], [30 x i8]* @.str39, i64 0, i64 0
  %t114 = call i32 @printUtilsSection(i32 4, i8* %t113)
  %items_115 = alloca i32
  %t116 = getelementptr inbounds [2 x i8], [2 x i8]* @.str6, i64 0, i64 0
  %t117 = getelementptr inbounds [35 x i8], [35 x i8]* @.str40, i64 0, i64 0
  store i32 0, i32* %items_115
  %t118 = getelementptr inbounds [23 x i8], [23 x i8]* @.str41, i64 0, i64 0
  %t119 = getelementptr inbounds [2 x i8], [2 x i8]* @.str6, i64 0, i64 0
  %t120 = load i32, i32* %items_115
  %t121 = add nsw i8* %t118, 0
  %t122 = call i32 @println(i8* %t121)
  %t123 = getelementptr inbounds [18 x i8], [18 x i8]* @.str42, i64 0, i64 0
  %t124 = getelementptr inbounds [2 x i8], [2 x i8]* @.str37, i64 0, i64 0
  %t125 = load i32, i32* %items_115
  %t126 = add nsw i8* %t123, 0
  %t127 = call i32 @println(i8* %t126)
  %t128 = getelementptr inbounds [17 x i8], [17 x i8]* @.str43, i64 0, i64 0
  %t129 = getelementptr inbounds [4 x i8], [4 x i8]* @.str44, i64 0, i64 0
  %t130 = load i32, i32* %items_115
  %t131 = add nsw i8* %t128, 0
  %t132 = call i32 @println(i8* %t131)
  %t133 = getelementptr inbounds [18 x i8], [18 x i8]* @.str45, i64 0, i64 0
  %t134 = getelementptr inbounds [5 x i8], [5 x i8]* @.str33, i64 0, i64 0
  %t135 = load i32, i32* %items_115
  %t136 = add nsw i8* %t133, 0
  %t137 = call i32 @println(i8* %t136)
  %t138 = getelementptr inbounds [20 x i8], [20 x i8]* @.str46, i64 0, i64 0
  %t139 = getelementptr inbounds [2 x i8], [2 x i8]* @.str0, i64 0, i64 0
  %t140 = load i32, i32* %items_115
  %t141 = add nsw i8* %t138, 0
  %t142 = call i32 @println(i8* %t141)
  %t143 = getelementptr inbounds [8 x i8], [8 x i8]* @.str47, i64 0, i64 0
  %t144 = call i32 @printUtilsSection(i32 5, i8* %t143)
  %data_145 = alloca i32
  %t146 = getelementptr inbounds [2 x i8], [2 x i8]* @.str6, i64 0, i64 0
  %t147 = getelementptr inbounds [10 x i8], [10 x i8]* @.str48, i64 0, i64 0
  store i32 0, i32* %data_145
  %t148 = getelementptr inbounds [14 x i8], [14 x i8]* @.str49, i64 0, i64 0
  %t149 = call i32 @println(i8* %t148)
  %t150 = getelementptr inbounds [9 x i8], [9 x i8]* @.str50, i64 0, i64 0
  %t151 = load i32, i32* %data_145
  %t152 = add nsw i8* %t150, 0
  %t153 = call i32 @println(i8* %t152)
  %t154 = getelementptr inbounds [10 x i8], [10 x i8]* @.str51, i64 0, i64 0
  %t155 = load i32, i32* %data_145
  %t156 = add nsw i8* %t154, 0
  %t157 = call i32 @println(i8* %t156)
  %cleared_158 = alloca i32
  %t159 = load i32, i32* %data_145
  store i32 0, i32* %cleared_158
  %t160 = getelementptr inbounds [14 x i8], [14 x i8]* @.str52, i64 0, i64 0
  %t161 = call i32 @println(i8* %t160)
  %t162 = getelementptr inbounds [9 x i8], [9 x i8]* @.str50, i64 0, i64 0
  %t163 = load i32, i32* %cleared_158
  %t164 = add nsw i8* %t162, 0
  %t165 = call i32 @println(i8* %t164)
  %t166 = getelementptr inbounds [12 x i8], [12 x i8]* @.str53, i64 0, i64 0
  %t167 = load i32, i32* %cleared_158
  %t168 = add nsw i8* %t166, 0
  %t169 = call i32 @println(i8* %t168)
  %t170 = getelementptr inbounds [16 x i8], [16 x i8]* @.str54, i64 0, i64 0
  %t171 = call i32 @printUtilsSection(i32 6, i8* %t170)
  %text_172 = alloca i32
  %t173 = getelementptr inbounds [2 x i8], [2 x i8]* @.str6, i64 0, i64 0
  %t174 = getelementptr inbounds [26 x i8], [26 x i8]* @.str55, i64 0, i64 0
  store i32 0, i32* %text_172
  %processed_175 = alloca i32
  %t176 = load i32, i32* %text_172
  store i32 0, i32* %processed_175
  %result_177 = alloca i32
  %t178 = getelementptr inbounds [2 x i8], [2 x i8]* @.str37, i64 0, i64 0
  %t179 = load i32, i32* %processed_175
  store i32 0, i32* %result_177
  %t180 = getelementptr inbounds [11 x i8], [11 x i8]* @.str32, i64 0, i64 0
  %t181 = getelementptr inbounds [2 x i8], [2 x i8]* @.str37, i64 0, i64 0
  %t182 = load i32, i32* %text_172
  %t183 = add nsw i8* %t180, 0
  %t184 = call i32 @println(i8* %t183)
  %t185 = getelementptr inbounds [36 x i8], [36 x i8]* @.str56, i64 0, i64 0
  %t186 = call i32 @println(i8* %t185)
  %t187 = getelementptr inbounds [9 x i8], [9 x i8]* @.str57, i64 0, i64 0
  %t188 = load i32, i32* %result_177
  %t189 = add nsw i8* %t187, %t188
  %t190 = call i32 @println(i8* %t189)
  %t191 = getelementptr inbounds [34 x i8], [34 x i8]* @.str58, i64 0, i64 0
  %t192 = call i32 @printUtilsSection(i32 7, i8* %t191)
  %logLines_193 = alloca i32
  %t194 = getelementptr inbounds [2 x i8], [2 x i8]* @.str6, i64 0, i64 0
  %t195 = getelementptr inbounds [41 x i8], [41 x i8]* @.str59, i64 0, i64 0
  store i32 0, i32* %logLines_193
  %t196 = getelementptr inbounds [20 x i8], [20 x i8]* @.str60, i64 0, i64 0
  %t197 = load i32, i32* %logLines_193
  %t198 = add nsw i8* %t196, 0
  %t199 = call i32 @println(i8* %t198)
  %t200 = getelementptr inbounds [18 x i8], [18 x i8]* @.str61, i64 0, i64 0
  %t201 = load i32, i32* %logLines_193
  %t202 = add nsw i8* %t200, 0
  %t203 = call i32 @println(i8* %t202)
  %t204 = getelementptr inbounds [17 x i8], [17 x i8]* @.str62, i64 0, i64 0
  %t205 = load i32, i32* %logLines_193
  %t206 = add nsw i8* %t204, 0
  %t207 = call i32 @println(i8* %t206)
  %t208 = getelementptr inbounds [17 x i8], [17 x i8]* @.str63, i64 0, i64 0
  %t209 = call i32 @println(i8* %t208)
  %t210 = getelementptr inbounds [19 x i8], [19 x i8]* @.str64, i64 0, i64 0
  %t211 = getelementptr inbounds [6 x i8], [6 x i8]* @.str65, i64 0, i64 0
  %t212 = load i32, i32* %logLines_193
  %t213 = add nsw i8* %t210, 0
  %t214 = call i32 @println(i8* %t213)
  %t215 = getelementptr inbounds [25 x i8], [25 x i8]* @.str66, i64 0, i64 0
  %t216 = getelementptr inbounds [6 x i8], [6 x i8]* @.str65, i64 0, i64 0
  %t217 = load i32, i32* %logLines_193
  %t218 = add nsw i8* %t215, 0
  %t219 = call i32 @println(i8* %t218)
  %t220 = getelementptr inbounds [19 x i8], [19 x i8]* @.str67, i64 0, i64 0
  %t221 = getelementptr inbounds [6 x i8], [6 x i8]* @.str68, i64 0, i64 0
  %t222 = load i32, i32* %logLines_193
  %t223 = add nsw i8* %t220, 0
  %t224 = call i32 @println(i8* %t223)
  %t225 = getelementptr inbounds [18 x i8], [18 x i8]* @.str69, i64 0, i64 0
  %t226 = getelementptr inbounds [5 x i8], [5 x i8]* @.str33, i64 0, i64 0
  %t227 = load i32, i32* %logLines_193
  %t228 = add nsw i8* %t225, 0
  %t229 = call i32 @println(i8* %t228)
  %t230 = getelementptr inbounds [37 x i8], [37 x i8]* @.str70, i64 0, i64 0
  %t231 = call i32 @printUtilsSection(i32 8, i8* %t230)
  %path_232 = alloca i32
  %t233 = getelementptr inbounds [2 x i8], [2 x i8]* @.str71, i64 0, i64 0
  %t234 = getelementptr inbounds [23 x i8], [23 x i8]* @.str72, i64 0, i64 0
  store i32 0, i32* %path_232
  %t235 = getelementptr inbounds [16 x i8], [16 x i8]* @.str73, i64 0, i64 0
  %t236 = load i32, i32* %path_232
  %t237 = add nsw i8* %t235, 0
  %t238 = call i32 @println(i8* %t237)
  %t239 = getelementptr inbounds [19 x i8], [19 x i8]* @.str74, i64 0, i64 0
  %t240 = load i32, i32* %path_232
  %t241 = add nsw i8* %t239, 0
  %t242 = getelementptr inbounds [24 x i8], [24 x i8]* @.str75, i64 0, i64 0
  %t243 = add nsw i8* %t241, %t242
  %t244 = call i32 @println(i8* %t243)
  %t245 = getelementptr inbounds [28 x i8], [28 x i8]* @.str76, i64 0, i64 0
  %t246 = load i32, i32* %path_232
  %t247 = add nsw i8* %t245, 0
  %t248 = call i32 @println(i8* %t247)
  %t249 = getelementptr inbounds [21 x i8], [21 x i8]* @.str77, i64 0, i64 0
  %t250 = getelementptr inbounds [6 x i8], [6 x i8]* @.str78, i64 0, i64 0
  %t251 = load i32, i32* %path_232
  %t252 = add nsw i8* %t249, 0
  %t253 = call i32 @println(i8* %t252)
  %reversePath_254 = alloca i32
  %t255 = load i32, i32* %path_232
  store i32 0, i32* %reversePath_254
  %t256 = getelementptr inbounds [16 x i8], [16 x i8]* @.str79, i64 0, i64 0
  %t257 = getelementptr inbounds [2 x i8], [2 x i8]* @.str71, i64 0, i64 0
  %t258 = load i32, i32* %reversePath_254
  %t259 = add nsw i8* %t256, 0
  %t260 = call i32 @println(i8* %t259)
  %t261 = getelementptr inbounds [24 x i8], [24 x i8]* @.str80, i64 0, i64 0
  %t262 = call i32 @printUtilsHeader(i8* %t261)
  %t263 = getelementptr inbounds [41 x i8], [41 x i8]* @.str81, i64 0, i64 0
  %t264 = call i32 @println(i8* %t263)
  %t265 = getelementptr inbounds [32 x i8], [32 x i8]* @.str82, i64 0, i64 0
  %t266 = call i32 @println(i8* %t265)
  %t267 = getelementptr inbounds [30 x i8], [30 x i8]* @.str83, i64 0, i64 0
  %t268 = call i32 @println(i8* %t267)
  %t269 = getelementptr inbounds [33 x i8], [33 x i8]* @.str84, i64 0, i64 0
  %t270 = call i32 @println(i8* %t269)
  %t271 = getelementptr inbounds [44 x i8], [44 x i8]* @.str85, i64 0, i64 0
  %t272 = call i32 @println(i8* %t271)
  %t273 = getelementptr inbounds [59 x i8], [59 x i8]* @.str86, i64 0, i64 0
  %t274 = call i32 @println(i8* %t273)
  %t275 = getelementptr inbounds [36 x i8], [36 x i8]* @.str87, i64 0, i64 0
  %t276 = call i32 @println(i8* %t275)
  %t277 = getelementptr inbounds [48 x i8], [48 x i8]* @.str88, i64 0, i64 0
  %t278 = call i32 @println(i8* %t277)
  %t279 = getelementptr inbounds [2 x i8], [2 x i8]* @.str0, i64 0, i64 0
  %t280 = getelementptr inbounds [2 x i8], [2 x i8]* @.str1, i64 0, i64 0
  %t281 = add nsw i8* %t279, 0
  %t282 = call i32 @println(i8* %t281)
  %t283 = getelementptr inbounds [44 x i8], [44 x i8]* @.str89, i64 0, i64 0
  %t284 = call i32 @println(i8* %t283)
  %t285 = getelementptr inbounds [2 x i8], [2 x i8]* @.str1, i64 0, i64 0
  %t286 = call i32 @println(i32 0)
  ret void
}


define i32 @main() {
  ret i32 0
}


; String Literals
@.str89 = private unnamed_addr constant [44 x i8] c"All array utility functions work perfectly!\00"
@.str88 = private unnamed_addr constant [48 x i8] c"✓ join(separator) - Join elements into string\00"
@.str84 = private unnamed_addr constant [33 x i8] c"✓ clear() - Return empty array\00"
@.str83 = private unnamed_addr constant [30 x i8] c"✓ last() - Get last element\00"
@.str81 = private unnamed_addr constant [41 x i8] c"
✓ isEmpty() - Check if array is empty\00"
@.str80 = private unnamed_addr constant [24 x i8] c"Array Utilities Summary\00"
@.str78 = private unnamed_addr constant [6 x i8] c"local\00"
@.str77 = private unnamed_addr constant [21 x i8] c"  Contains 'local': \00"
@.str87 = private unnamed_addr constant [36 x i8] c"✓ reverse() - Reverse array order\00"
@.str70 = private unnamed_addr constant [37 x i8] c"Practical Example: Path Manipulation\00"
@.str69 = private unnamed_addr constant [18 x i8] c"
All log levels: \00"
@.str68 = private unnamed_addr constant [6 x i8] c"FATAL\00"
@.str67 = private unnamed_addr constant [19 x i8] c"  Contains FATAL: \00"
@.str66 = private unnamed_addr constant [25 x i8] c"  First ERROR at index: \00"
@.str65 = private unnamed_addr constant [6 x i8] c"ERROR\00"
@.str64 = private unnamed_addr constant [19 x i8] c"  Contains ERROR: \00"
@.str63 = private unnamed_addr constant [17 x i8] c"
Searching logs:\00"
@.str82 = private unnamed_addr constant [32 x i8] c"✓ first() - Get first element\00"
@.str72 = private unnamed_addr constant [23 x i8] c"/usr/local/bin/stratos\00"
@.str62 = private unnamed_addr constant [17 x i8] c"Last log level: \00"
@.str61 = private unnamed_addr constant [18 x i8] c"First log level: \00"
@.str60 = private unnamed_addr constant [20 x i8] c"Total log entries: \00"
@.str59 = private unnamed_addr constant [41 x i8] c"INFO,WARNING,ERROR,INFO,DEBUG,ERROR,INFO\00"
@.str28 = private unnamed_addr constant [22 x i8] c"  indexOf('orange'): \00"
@.str27 = private unnamed_addr constant [7 x i8] c"orange\00"
@.str21 = private unnamed_addr constant [3 x i8] c", \00"
@.str18 = private unnamed_addr constant [25 x i8] c"contains() and indexOf()\00"
@.str25 = private unnamed_addr constant [20 x i8] c"  indexOf('blue'): \00"
@.str3 = private unnamed_addr constant [2 x i8] c"-\00"
@.str24 = private unnamed_addr constant [5 x i8] c"blue\00"
@.str32 = private unnamed_addr constant [11 x i8] c"Original: \00"
@.str17 = private unnamed_addr constant [12 x i8] c"  last(): '\00"
@.str56 = private unnamed_addr constant [36 x i8] c"Process: reverse -> join with space\00"
@.str85 = private unnamed_addr constant [44 x i8] c"✓ contains(value) - Check if value exists\00"
@.str16 = private unnamed_addr constant [25 x i8] c"' (returns empty string)\00"
@.str15 = private unnamed_addr constant [13 x i8] c"  first(): '\00"
@.str19 = private unnamed_addr constant [29 x i8] c"red,green,blue,yellow,purple\00"
@.str14 = private unnamed_addr constant [14 x i8] c"
Empty array:\00"
@.str13 = private unnamed_addr constant [1 x i8] c"\00"
@.str11 = private unnamed_addr constant [12 x i8] c"  first(): \00"
@.str0 = private unnamed_addr constant [2 x i8] c"
\00"
@.str2 = private unnamed_addr constant [3 x i8] c". \00"
@.str1 = private unnamed_addr constant [2 x i8] c"=\00"
@.str35 = private unnamed_addr constant [25 x i8] c"Hello,World,from,Stratos\00"
@.str12 = private unnamed_addr constant [11 x i8] c"  last(): \00"
@.str71 = private unnamed_addr constant [2 x i8] c"/\00"
@.str5 = private unnamed_addr constant [31 x i8] c"isEmpty(), first(), and last()\00"
@.str7 = private unnamed_addr constant [36 x i8] c"apple,banana,cherry,date,elderberry\00"
@.str4 = private unnamed_addr constant [29 x i8] c"Array Utility Functions Demo\00"
@.str9 = private unnamed_addr constant [7 x i8] c" items\00"
@.str33 = private unnamed_addr constant [5 x i8] c" -> \00"
@.str6 = private unnamed_addr constant [2 x i8] c",\00"
@.str8 = private unnamed_addr constant [15 x i8] c"Fruits array: \00"
@.str23 = private unnamed_addr constant [21 x i8] c"  contains('blue'): \00"
@.str26 = private unnamed_addr constant [23 x i8] c"  contains('orange'): \00"
@.str20 = private unnamed_addr constant [9 x i8] c"Colors: \00"
@.str73 = private unnamed_addr constant [16 x i8] c"Path segments: \00"
@.str10 = private unnamed_addr constant [14 x i8] c"  isEmpty(): \00"
@.str29 = private unnamed_addr constant [27 x i8] c" (returns -1 if not found)\00"
@.str30 = private unnamed_addr constant [10 x i8] c"reverse()\00"
@.str31 = private unnamed_addr constant [10 x i8] c"1,2,3,4,5\00"
@.str52 = private unnamed_addr constant [14 x i8] c"
After clear:\00"
@.str34 = private unnamed_addr constant [11 x i8] c"Reversed: \00"
@.str86 = private unnamed_addr constant [59 x i8] c"✓ indexOf(value) - Find index of value (-1 if not found)\00"
@.str36 = private unnamed_addr constant [18 x i8] c"
Original words: \00"
@.str48 = private unnamed_addr constant [10 x i8] c"a,b,c,d,e\00"
@.str37 = private unnamed_addr constant [2 x i8] c" \00"
@.str54 = private unnamed_addr constant [16 x i8] c"Method Chaining\00"
@.str38 = private unnamed_addr constant [17 x i8] c"Reversed words: \00"
@.str41 = private unnamed_addr constant [23 x i8] c"Default join (comma): \00"
@.str40 = private unnamed_addr constant [35 x i8] c"laptop,phone,tablet,mouse,keyboard\00"
@.str42 = private unnamed_addr constant [18 x i8] c"Space separated: \00"
@.str79 = private unnamed_addr constant [16 x i8] c"
Reverse path: \00"
@.str43 = private unnamed_addr constant [17 x i8] c"Pipe separated: \00"
@.str58 = private unnamed_addr constant [34 x i8] c"Practical Example: Log Processing\00"
@.str76 = private unnamed_addr constant [28 x i8] c"  Last segment (filename): \00"
@.str44 = private unnamed_addr constant [4 x i8] c" | \00"
@.str45 = private unnamed_addr constant [18 x i8] c"Arrow separated: \00"
@.str47 = private unnamed_addr constant [8 x i8] c"clear()\00"
@.str49 = private unnamed_addr constant [14 x i8] c"Before clear:\00"
@.str74 = private unnamed_addr constant [19 x i8] c"  First segment: '\00"
@.str50 = private unnamed_addr constant [9 x i8] c"  Size: \00"
@.str75 = private unnamed_addr constant [24 x i8] c"' (empty for leading /)\00"
@.str39 = private unnamed_addr constant [30 x i8] c"join() with custom separators\00"
@.str22 = private unnamed_addr constant [23 x i8] c"
Searching for values:\00"
@.str53 = private unnamed_addr constant [12 x i8] c"  isEmpty: \00"
@.str46 = private unnamed_addr constant [20 x i8] c"Newline separated:
\00"
@.str51 = private unnamed_addr constant [10 x i8] c"  First: \00"
@.str57 = private unnamed_addr constant [9 x i8] c"Result: \00"
@.str55 = private unnamed_addr constant [26 x i8] c"the,quick,brown,fox,jumps\00"
