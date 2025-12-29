; ModuleID = 'stratos_module'
source_filename = "stratos_source"
target datalayout = "e-m:w-p270:32:32-p271:32:32-p272:64:64-i64:64-f80:128-n8:16:32:64-S128"
target triple = "x86_64-pc-windows-msvc"

declare i32 @printf(i8*, ...)

declare i8* @malloc(i64)

; Format specifiers for print
@.str_specifier = private unnamed_addr constant [4 x i8] c"%d\0A\00"

; Class struct type definitions
%struct.Node = type { i32, i32 }


; Constructor for Node
define %struct.Node* @Node(i32 %arg0) {
  %t0 = call i8* @malloc(i64 16)
  %t1 = bitcast i8* %t0 to %struct.Node*
  %v.addr = alloca i32
  store i32 %arg0, i32* %v.addr
  %t2 = getelementptr inbounds %struct.Node, %struct.Node* %t1, i32 0, i32 0
  %t3 = load i32, i32* %t2
  %t4 = load i32, i32* %v.addr
  %t5 = add i32 %t3, %t4
  ret %struct.Node* %t1
}


; Method Node.setNext
define void @Node_setNext(%struct.Node* %this_ptr, i32 %arg0) {
  %n.addr = alloca i32
  store i32 %arg0, i32* %n.addr
  %t0 = getelementptr inbounds %struct.Node, %struct.Node* %this_ptr, i32 0, i32 1
  %t1 = load i32, i32* %t0
  %t2 = load i32, i32* %n.addr
  %t3 = add i32 %t1, %t2
  ret void
}


define void @main() {
  %a_0 = alloca i32
  %t1 = call %struct.Node* @Node(i32 1)
  store i32 %t1, i32* %a_0
  %b_2 = alloca i32
  %t3 = call %struct.Node* @Node(i32 2)
  store i32 %t3, i32* %b_2
  %t4 = load i32, i32* %a_0
  %t5 = call i32 @println(i32 %t4)
  %t6 = load i32, i32* %b_2
  %t7 = call i32 @println(i32 %t6)
  %t8 = load i32, i32* %b_2
  %t9 = load i32, i32* %a_0
  %t10 = load i32, i32* %a_0
  %t11 = load i32, i32* %b_2
  %t12 = load i32, i32* %a_0
  %t13 = call i32 @println(i32 %t12)
  %t14 = load i32, i32* %b_2
  %t15 = call i32 @println(i32 %t14)
  ret void
}


; String Literals
