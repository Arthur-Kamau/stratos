; ModuleID = 'stratos_module'
source_filename = "stratos_source"
target datalayout = "e-m:w-p270:32:32-p271:32:32-p272:64:64-i64:64-f80:128-n8:16:32:64-S128"
target triple = "x86_64-pc-windows-msvc"

declare i32 @printf(i8*, ...)

declare i8* @malloc(i64)

; Format specifiers for print
@.str_specifier = private unnamed_addr constant [4 x i8] c"%d\0A\00"

; Class struct type definitions
%struct.Result = type { i32, i32, i1 }
%struct.Error = type { i8* }
%struct.File = type { i8*, i32, i8* }


; Constructor for File
define %struct.File* @File() {
  %t0 = call i8* @malloc(i64 24)
  %t1 = bitcast i8* %t0 to %struct.File*
  %t2 = getelementptr inbounds %struct.File, %struct.File* %t1, i32 0, i32 0
  store i8* null, i8** %t2
  %t3 = getelementptr inbounds %struct.File, %struct.File* %t1, i32 0, i32 1
  store i32 0, i32* %t3
  %t4 = getelementptr inbounds %struct.File, %struct.File* %t1, i32 0, i32 2
  store i8* null, i8** %t4
  ret %struct.File* %t1
}


; Constructor for Result
define %struct.Result* @Result() {
  %t5 = call i8* @malloc(i64 24)
  %t6 = bitcast i8* %t5 to %struct.Result*
  %t7 = getelementptr inbounds %struct.Result, %struct.Result* %t6, i32 0, i32 0
  store i32 0, i32* %t7
  %t8 = getelementptr inbounds %struct.Result, %struct.Result* %t6, i32 0, i32 1
  store i32 0, i32* %t8
  %t9 = getelementptr inbounds %struct.Result, %struct.Result* %t6, i32 0, i32 2
  store i1 0, i1* %t9
  ret %struct.Result* %t6
}


; Method Result.ok
define i1 @Result_ok(%struct.Result* %this_ptr) {
  %t0 = getelementptr inbounds %struct.Result, %struct.Result* %this_ptr, i32 0, i32 2
  %t1 = load i1, i1* %t0
  ret i1 %t1
}


; Method Result.err
define i32 @Result_err(%struct.Result* %this_ptr) {
  %t0 = getelementptr inbounds %struct.Result, %struct.Result* %this_ptr, i32 0, i32 1
  %t1 = load i32, i32* %t0
  ret i32 %t1
  ret i32 0
}


; Method Result.unwrap
define i32 @Result_unwrap(%struct.Result* %this_ptr) {
  %t0 = getelementptr inbounds %struct.Result, %struct.Result* %this_ptr, i32 0, i32 2
  %t1 = load i1, i1* %t0
  br i1 %t1, label %L0, label %L1

L0:
  %t2 = getelementptr inbounds [30 x i8], [30 x i8]* @.str0, i64 0, i64 0
  %t3 = call i32 @panic(i8* %t2)
  br label %L2

L1:
  br label %L2

L2:
  %t4 = getelementptr inbounds %struct.Result, %struct.Result* %this_ptr, i32 0, i32 0
  %t5 = load i32, i32* %t4
  ret i32 %t5
  ret i32 0
}


; Constructor for Error
define %struct.Error* @Error(i8* %arg0) {
  %t6 = call i8* @malloc(i64 8)
  %t7 = bitcast i8* %t6 to %struct.Error*
  %msg.addr = alloca i8*
  store i8* %arg0, i8** %msg.addr
  %t8 = getelementptr inbounds %struct.Error, %struct.Error* %t7, i32 0, i32 0
  %t9 = load i8*, i8** %t8
  %t10 = load i8*, i8** %msg.addr
  %t11 = add i8* %t9, %t10
  ret %struct.Error* %t7
}


define i8* @readFile(i8* %arg0) {
  %path.addr = alloca i8*
  store i8* %arg0, i8** %path.addr
}


define i32 @readBytes(i8* %arg0) {
  %path.addr = alloca i8*
  store i8* %arg0, i8** %path.addr
  ret i32 0
}


define i1 @writeFile(i8* %arg0, i8* %arg1) {
  %path.addr = alloca i8*
  store i8* %arg0, i8** %path.addr
  %content.addr = alloca i8*
  store i8* %arg1, i8** %content.addr
}


define i1 @appendFile(i8* %arg0, i8* %arg1) {
  %path.addr = alloca i8*
  store i8* %arg0, i8** %path.addr
  %content.addr = alloca i8*
  store i8* %arg1, i8** %content.addr
}


define i1 @writeBytes(i8* %arg0, i32 %arg1) {
  %path.addr = alloca i8*
  store i8* %arg0, i8** %path.addr
  %data.addr = alloca i32
  store i32 %arg1, i32* %data.addr
}


define i32 @open(i8* %arg0, i8* %arg1) {
  %path.addr = alloca i8*
  store i8* %arg0, i8** %path.addr
  %mode.addr = alloca i8*
  store i8* %arg1, i8** %mode.addr
  ret i32 0
}


define i32 @create(i8* %arg0) {
  %path.addr = alloca i8*
  store i8* %arg0, i8** %path.addr
  ret i32 0
}


define i1 @remove(i8* %arg0) {
  %path.addr = alloca i8*
  store i8* %arg0, i8** %path.addr
}


define i1 @rename(i8* %arg0, i8* %arg1) {
  %oldPath.addr = alloca i8*
  store i8* %arg0, i8** %oldPath.addr
  %newPath.addr = alloca i8*
  store i8* %arg1, i8** %newPath.addr
}


define i1 @copy(i8* %arg0, i8* %arg1) {
  %src.addr = alloca i8*
  store i8* %arg0, i8** %src.addr
  %dst.addr = alloca i8*
  store i8* %arg1, i8** %dst.addr
}


define i1 @mkdir(i8* %arg0) {
  %path.addr = alloca i8*
  store i8* %arg0, i8** %path.addr
}


define i1 @mkdirAll(i8* %arg0) {
  %path.addr = alloca i8*
  store i8* %arg0, i8** %path.addr
}


define i1 @removeDir(i8* %arg0) {
  %path.addr = alloca i8*
  store i8* %arg0, i8** %path.addr
}


define i1 @removeDirAll(i8* %arg0) {
  %path.addr = alloca i8*
  store i8* %arg0, i8** %path.addr
}


define i32 @readDir(i8* %arg0) {
  %path.addr = alloca i8*
  store i8* %arg0, i8** %path.addr
  ret i32 0
}


define i1 @exists(i8* %arg0) {
  %path.addr = alloca i8*
  store i8* %arg0, i8** %path.addr
}


define i1 @isFile(i8* %arg0) {
  %path.addr = alloca i8*
  store i8* %arg0, i8** %path.addr
}


define i1 @isDirectory(i8* %arg0) {
  %path.addr = alloca i8*
  store i8* %arg0, i8** %path.addr
}


define i32 @fileSize(i8* %arg0) {
  %path.addr = alloca i8*
  store i8* %arg0, i8** %path.addr
  ret i32 0
}


define i32 @stat(i8* %arg0) {
  %path.addr = alloca i8*
  store i8* %arg0, i8** %path.addr
  ret i32 0
}


define i8* @pathJoin(i32 %arg0) {
  %parts.addr = alloca i32
  store i32 %arg0, i32* %parts.addr
}


define i8* @basename(i8* %arg0) {
  %path.addr = alloca i8*
  store i8* %arg0, i8** %path.addr
}


define i8* @dirname(i8* %arg0) {
  %path.addr = alloca i8*
  store i8* %arg0, i8** %path.addr
}


define i8* @extension(i8* %arg0) {
  %path.addr = alloca i8*
  store i8* %arg0, i8** %path.addr
}


define i8* @absolute(i8* %arg0) {
  %path.addr = alloca i8*
  store i8* %arg0, i8** %path.addr
}


define i32 @tempFile(i8* %arg0) {
  %pattern.addr = alloca i8*
  store i8* %arg0, i8** %pattern.addr
  ret i32 0
}


define i32 @tempDir(i8* %arg0) {
  %pattern.addr = alloca i8*
  store i8* %arg0, i8** %pattern.addr
  ret i32 0
}


define i32 @readLines(i8* %arg0) {
  %path.addr = alloca i8*
  store i8* %arg0, i8** %path.addr
  %content_0 = alloca i32
  %t1 = load i8*, i8** %path.addr
  %t2 = call i32 @readFile(i8* %t1)
  store i32 %t2, i32* %content_0
  %t3 = load i32, i32* %content_0
  %t4 = getelementptr inbounds [1 x i8], [1 x i8]* @.str1, i64 0, i64 0
  %t5 = icmp eq i32 %t3, %t4
  br i1 %t5, label %L3, label %L4

L3:
  %t6 = call i32 @Array()
  ret i32 %t6
  br label %L5

L4:
  br label %L5

L5:
  %t7 = getelementptr inbounds [2 x i8], [2 x i8]* @.str2, i64 0, i64 0
  %t8 = load i32, i32* %content_0
  ret i32 0
  ret i32 0
}


define i1 @writeLines(i8* %arg0, i32 %arg1) {
  %path.addr = alloca i8*
  store i8* %arg0, i8** %path.addr
  %lines.addr = alloca i32
  store i32 %arg1, i32* %lines.addr
  %content_0 = alloca i32
  %t1 = getelementptr inbounds [2 x i8], [2 x i8]* @.str2, i64 0, i64 0
  %t2 = load i32, i32* %lines.addr
  store i32 0, i32* %content_0
  %t3 = load i8*, i8** %path.addr
  %t4 = load i32, i32* %content_0
  %t5 = call i32 @writeFile(i8* %t3, i32 %t4)
  ret i32 %t5
}


define i1 @appendLine(i8* %arg0, i8* %arg1) {
  %path.addr = alloca i8*
  store i8* %arg0, i8** %path.addr
  %line.addr = alloca i8*
  store i8* %arg1, i8** %line.addr
  %t0 = load i8*, i8** %path.addr
  %t1 = load i8*, i8** %line.addr
  %t2 = getelementptr inbounds [2 x i8], [2 x i8]* @.str2, i64 0, i64 0
  %t3 = add nsw i8* %t1, %t2
  %t4 = call i32 @appendFile(i8* %t0, i8* %t3)
  ret i32 %t4
}


define i1 @hasExtension(i8* %arg0, i8* %arg1) {
  %path.addr = alloca i8*
  store i8* %arg0, i8** %path.addr
  %ext.addr = alloca i8*
  store i8* %arg1, i8** %ext.addr
  %t0 = load i8*, i8** %path.addr
  %t1 = call i32 @extension(i8* %t0)
  %t2 = load i8*, i8** %ext.addr
  %t3 = icmp eq i32 %t1, %t2
  ret i1 %t3
}


define i32 @listFiles(i8* %arg0, i32 %arg1) {
  %path.addr = alloca i8*
  store i8* %arg0, i8** %path.addr
  %filter.addr = alloca i32
  store i32 %arg1, i32* %filter.addr
  %files_0 = alloca i32
  %t1 = load i8*, i8** %path.addr
  %t2 = call i32 @readDir(i8* %t1)
  store i32 %t2, i32* %files_0
  %filtered_3 = alloca i32
  %t4 = call i32 @Array()
  store i32 %t4, i32* %filtered_3
  ; For loop not yet implemented in IR generation