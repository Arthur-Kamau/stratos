; ModuleID = 'stratos_module'
source_filename = "stratos_source"
target datalayout = "e-m:w-p270:32:32-p271:32:32-p272:64:64-i64:64-f80:128-n8:16:32:64-S128"
target triple = "x86_64-pc-windows-msvc"

declare i32 @printf(i8*, ...)

declare i8* @malloc(i64)

; Format specifiers for print
@.str_specifier = private unnamed_addr constant [4 x i8] c"%d\0A\00"

; Class struct type definitions
%struct.Square = type { i8 }
%struct.Circle = type { double }
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


; Constructor for Square
define %struct.Square* @Square() {
  %t6 = call i8* @malloc(i64 0)
  %t7 = bitcast i8* %t6 to %struct.Square*
  ret %struct.Square* %t7
}


; Method Square.perimeter
define double @Square_perimeter(%struct.Square* %this_ptr) {
  %t0 = fmul double 4.0, %this_ptr
  ret double %t0
  ret double 0.0
}


; Constructor for Circle
define %struct.Circle* @Circle() {
  %t1 = call i8* @malloc(i64 8)
  %t2 = bitcast i8* %t1 to %struct.Circle*
  %t3 = getelementptr inbounds %struct.Circle, %struct.Circle* %t2, i32 0, i32 0
  store double 0.0, double* %t3
  ret %struct.Circle* %t2
}


; Method Circle.area
define double @Circle_area(%struct.Circle* %this_ptr) {
  %t0 = getelementptr inbounds %struct.Circle, %struct.Circle* %this_ptr, i32 0, i32 0
  %t1 = load double, double* %t0
  %t2 = fmul double 3.14159, %t1
  %t3 = getelementptr inbounds %struct.Circle, %struct.Circle* %this_ptr, i32 0, i32 0
  %t4 = load double, double* %t3
  %t5 = fmul double %t2, %t4
  ret double %t5
  ret double 0.0
}


; Method Circle.perimeter
define double @Circle_perimeter(%struct.Circle* %this_ptr) {
  %t0 = fmul double 2.0, 3.14159
  %t1 = getelementptr inbounds %struct.Circle, %struct.Circle* %this_ptr, i32 0, i32 0
  %t2 = load double, double* %t1
  %t3 = fmul double %t0, %t2
  ret double %t3
  ret double 0.0
}


define i32 @main() {
  %rect_4 = alloca i32
  %t5 = call %struct.Rectangle* @Rectangle(double 10.0, double 5.0)
  store i32 %t5, i32* %rect_4
  %square_6 = alloca i32
  %t7 = call %struct.Square* @Square(double 7.0)
  store i32 %t7, i32* %square_6
  %circle_8 = alloca i32
  %t9 = call %struct.Circle* @Circle(double 3.0)
  store i32 %t9, i32* %circle_8
  %t10 = load i32, i32* %rect_4
  %t12 = call i32 (i8*, ...) @printf(i8* getelementptr inbounds ([4 x i8], [4 x i8]* @.str_specifier, i32 0, i32 0), i32 0)
  %t13 = call i32 (i8*, ...) @printf(i8* getelementptr inbounds ([4 x i8], [4 x i8]* @.str1, i32 0, i32 0), i32 %t12)
  %t14 = load i32, i32* %square_6
  %t16 = call i32 (i8*, ...) @printf(i8* getelementptr inbounds ([4 x i8], [4 x i8]* @.str_specifier, i32 0, i32 0), i32 0)
  %t17 = call i32 (i8*, ...) @printf(i8* getelementptr inbounds ([4 x i8], [4 x i8]* @.str1, i32 0, i32 0), i32 %t16)
  %t18 = load i32, i32* %circle_8
  %t20 = call i32 (i8*, ...) @printf(i8* getelementptr inbounds ([4 x i8], [4 x i8]* @.str_specifier, i32 0, i32 0), i32 0)
  %t21 = call i32 (i8*, ...) @printf(i8* getelementptr inbounds ([4 x i8], [4 x i8]* @.str1, i32 0, i32 0), i32 %t20)
  ret i32 0
}


; String Literals
@.str1 = private unnamed_addr constant [6 x i8] c"%d\0A\00"
@.str0 = private unnamed_addr constant [3 x i8] c"%d\00"
