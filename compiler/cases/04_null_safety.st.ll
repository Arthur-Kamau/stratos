; ModuleID = 'stratos_module'
source_filename = "stratos_source"
target datalayout = "e-m:w-p270:32:32-p271:32:32-p272:64:64-i64:64-f80:128-n8:16:32:64-S128"
target triple = "x86_64-pc-windows-msvc"

declare i32 @printf(i8*, ...)

@.str_specifier = private unnamed_addr constant [4 x i8] c"%d\0A\00"

define i32 @main() {
  %maybeNumber_0 = alloca i32
  store i32 42, i32* %maybeNumber_0
  %empty_1 = alloca i32
  store i32 42, i32* %empty_1
  ret i32 0
}

