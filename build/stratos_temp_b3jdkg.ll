; ModuleID = 'stratos_module'
source_filename = "stratos_source"
target datalayout = "e-m:w-p270:32:32-p271:32:32-p272:64:64-i64:64-f80:128-n8:16:32:64-S128"
target triple = "x86_64-pc-windows-msvc"

declare i32 @printf(i8*, ...)

declare i8* @malloc(i64)

; Format specifiers for print
@.str_specifier = private unnamed_addr constant [4 x i8] c"%d\0A\00"

define void @hello() {
  %t0 = getelementptr inbounds [12 x i8], [12 x i8]* @.str0, i64 0, i64 0
  %t1 = call i32 @println(i8* %t0)
  ret void
}


define void @nn() {
  ret void
}


define void @printName(i32 %arg0) {
  %name.addr = alloca i32
  store i32 %arg0, i32* %name.addr
  %t0 = getelementptr inbounds [6 x i8], [6 x i8]* @.str1, i64 0, i64 0
  %t1 = call i32 @println(i8* %t0)
  %t2 = load i32, i32* %name.addr
  %t3 = call i32 @println(i32 %t2)
  ret void
}


define void @printNumber(i32 %arg0) {
  %num.addr = alloca i32
  store i32 %arg0, i32* %num.addr
  %t0 = getelementptr inbounds [11 x i8], [11 x i8]* @.str2, i64 0, i64 0
  %t1 = call i32 @println(i8* %t0)
  %t2 = load i32, i32* %num.addr
  %t3 = call i32 @println(i32 %t2)
  ret void
}


define void @printNameAndIsMale(i32 %arg0, i1 %arg1) {
  %name.addr = alloca i32
  store i32 %arg0, i32* %name.addr
  %isMale.addr = alloca i1
  store i1 %arg1, i1* %isMale.addr
  %t0 = getelementptr inbounds [6 x i8], [6 x i8]* @.str3, i64 0, i64 0
  %t1 = call i32 @println(i8* %t0)
  %t2 = load i32, i32* %name.addr
  %t3 = call i32 @println(i32 %t2)
  %t4 = getelementptr inbounds [13 x i8], [13 x i8]* @.str4, i64 0, i64 0
  %t5 = call i32 @println(i8* %t4)
  %t6 = load i1, i1* %isMale.addr
  %t7 = call i32 @println(i1 %t6)
  ret void
}


define i32 @returnAge(i32 %arg0) {
  %name.addr = alloca i32
  store i32 %arg0, i32* %name.addr
  ret i32 26
  ret i32 0
}


define i32 @printHelloNameAndReturnHeight(i32 %arg0) {
  %name.addr = alloca i32
  store i32 %arg0, i32* %name.addr
  %t0 = getelementptr inbounds [6 x i8], [6 x i8]* @.str1, i64 0, i64 0
  %t1 = call i32 @println(i8* %t0)
  %t2 = load i32, i32* %name.addr
  %t3 = call i32 @println(i32 %t2)
  ret i32 170
  ret i32 0
}


define void @jumbo(i8* %arg0) {
  %p.addr = alloca i8*
  store i8* %arg0, i8** %p.addr
  ret void
}


define void @main() {
  %t0 = getelementptr inbounds [26 x i8], [26 x i8]* @.str5, i64 0, i64 0
  %t1 = call i32 @println(i8* %t0)
  %t2 = call i32 @hello()
  %t3 = getelementptr inbounds [7 x i8], [7 x i8]* @.str6, i64 0, i64 0
  %t4 = call i32 @printName(i8* %t3)
  %t5 = call i32 @printNumber(i32 42)
  %t6 = getelementptr inbounds [7 x i8], [7 x i8]* @.str6, i64 0, i64 0
  %t7 = call i32 @printNameAndIsMale(i8* %t6, i1 1)
  %me_8 = alloca i32
  %t9 = getelementptr inbounds [7 x i8], [7 x i8]* @.str6, i64 0, i64 0
  %t10 = call i32 @returnAge(i8* %t9)
  store i32 %t10, i32* %me_8
  %t11 = getelementptr inbounds [8 x i8], [8 x i8]* @.str7, i64 0, i64 0
  %t12 = call i32 @println(i8* %t11)
  %t13 = load i32, i32* %me_8
  %t14 = call i32 @println(i32 %t13)
  %h_15 = alloca i32
  %t16 = getelementptr inbounds [7 x i8], [7 x i8]* @.str6, i64 0, i64 0
  %t17 = call i32 @printHelloNameAndReturnHeight(i8* %t16)
  store i32 %t17, i32* %h_15
  %t18 = getelementptr inbounds [11 x i8], [11 x i8]* @.str8, i64 0, i64 0
  %t19 = call i32 @println(i8* %t18)
  %t20 = load i32, i32* %h_15
  %t21 = call i32 @println(i32 %t20)
  ret void
}


; String Literals
@.str8 = private unnamed_addr constant [11 x i8] c"my height:\00"
@.str7 = private unnamed_addr constant [8 x i8] c"my age:\00"
@.str5 = private unnamed_addr constant [26 x i8] c"--- Function Examples ---\00"
@.str4 = private unnamed_addr constant [13 x i8] c"are you male\00"
@.str3 = private unnamed_addr constant [6 x i8] c"Hello\00"
@.str1 = private unnamed_addr constant [6 x i8] c"hello\00"
@.str6 = private unnamed_addr constant [7 x i8] c"Arthur\00"
@.str2 = private unnamed_addr constant [11 x i8] c"Number is:\00"
@.str0 = private unnamed_addr constant [12 x i8] c"hello world\00"
