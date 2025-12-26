; ModuleID = 'stratos_module'
source_filename = "stratos_source"
target datalayout = "e-m:w-p270:32:32-p271:32:32-p272:64:64-i64:64-f80:128-n8:16:32:64-S128"
target triple = "x86_64-pc-windows-msvc"

declare i32 @printf(i8*, ...)

declare i8* @malloc(i64)

; Format specifiers for print
@.str_specifier = private unnamed_addr constant [4 x i8] c"%d\0A\00"

; Class struct type definitions
%struct.Rectangle = type { double, double }
%struct.Shape = type { i8 }


; Constructor for Shape
define %struct.Shape* @Shape() {
  %t0 = call i8* @malloc(i64 0)
  %t1 = bitcast i8* %t0 to %struct.Shape*
  ret %struct.Shape* %t1
}


; Constructor for Rectangle
define %struct.Rectangle* @Rectangle() {
  %t2 = call i8* @malloc(i64 16)
  %t3 = bitcast i8* %t2 to %struct.Rectangle*
  %t4 = getelementptr inbounds %struct.Rectangle, %struct.Rectangle* %t3, i32 0, i32 0
  store double 0.0, double* %t4
  %t5 = getelementptr inbounds %struct.Rectangle, %struct.Rectangle* %t3, i32 0, i32 1
  store double 0.0, double* %t5
  ret %struct.Rectangle* %t3
}


; Method Rectangle.area
define double @Rectangle_area(%struct.Rectangle* %this_ptr) {
  %t0 = getelementptr inbounds %struct.Rectangle, %struct.Rectangle* %this_ptr, i32 0, i32 0
  %t1 = load double, double* %t0
  %t2 = getelementptr inbounds %struct.Rectangle, %struct.Rectangle* %this_ptr, i32 0, i32 1
  %t3 = load double, double* %t2
  %t4 = fmul double %t1, %t3
  ret double %t4
  ret double 0.0
}


; Method Rectangle.perimeter
define double @Rectangle_perimeter(%struct.Rectangle* %this_ptr) {
  %t0 = getelementptr inbounds %struct.Rectangle, %struct.Rectangle* %this_ptr, i32 0, i32 0
  %t1 = load double, double* %t0
  %t2 = getelementptr inbounds %struct.Rectangle, %struct.Rectangle* %this_ptr, i32 0, i32 1
  %t3 = load double, double* %t2
  %t4 = fadd double %t1, %t3
  %t5 = fmul double 2.0, %t4
  ret double %t5
  ret double 0.0
}


; Method Rectangle.getWidth
define double @Rectangle_getWidth(%struct.Rectangle* %this_ptr) {
  %t0 = getelementptr inbounds %struct.Rectangle, %struct.Rectangle* %this_ptr, i32 0, i32 0
  %t1 = load double, double* %t0
  ret double %t1
  ret double 0.0
}


define void @main() {
  %rect_0 = alloca i32
  %t1 = call %struct.Rectangle* @Rectangle(double 10.0, double 5.0)
  store i32 %t1, i32* %rect_0
  %t2 = load i32, i32* %rect_0
  %t4 = call i32 (i8*, ...) @printf(i8* getelementptr inbounds ([4 x i8], [4 x i8]* @.str_specifier, i32 0, i32 0), i32 0)
  %t5 = call i32 (i8*, ...) @printf(i8* getelementptr inbounds ([4 x i8], [4 x i8]* @.str1, i32 0, i32 0), i32 %t4)
  %t6 = load i32, i32* %rect_0
  %t8 = call i32 (i8*, ...) @printf(i8* getelementptr inbounds ([4 x i8], [4 x i8]* @.str_specifier, i32 0, i32 0), i32 0)
  %t9 = call i32 (i8*, ...) @printf(i8* getelementptr inbounds ([4 x i8], [4 x i8]* @.str1, i32 0, i32 0), i32 %t8)
  %t10 = load i32, i32* %rect_0
  %t12 = call i32 (i8*, ...) @printf(i8* getelementptr inbounds ([4 x i8], [4 x i8]* @.str_specifier, i32 0, i32 0), i32 0)
  %t13 = call i32 (i8*, ...) @printf(i8* getelementptr inbounds ([4 x i8], [4 x i8]* @.str1, i32 0, i32 0), i32 %t12)
  ret void
}


; String Literals
@.str1 = private unnamed_addr constant [6 x i8] c"%d\0A\00"
@.str0 = private unnamed_addr constant [3 x i8] c"%d\00"
