; ModuleID = 'EvaLLVM'
source_filename = "EvaLLVM"

@VERSION = global i32 42, align 4
@0 = private unnamed_addr constant [3 x i8] c"%d\00", align 1

declare i32 @printf(i8*, ...)

define i32 @main() {
entry:
  %x = alloca i32
  store i32 10, i32* %x
  br label %cond

cond:                                             ; preds = %body, %entry
  %x1 = load i32, i32* %x
  %tmpcmp = icmp ugt i32 %x1, 0
  br i1 %tmpcmp, label %body, label %loopend

body:                                             ; preds = %cond
  %x2 = load i32, i32* %x
  %tmpsub = sub i32 %x2, 1
  store i32 %tmpsub, i32* %x
  %x3 = load i32, i32* %x
  %0 = call i32 (i8*, ...) @printf(i8* getelementptr inbounds ([3 x i8], [3 x i8]* @0, i32 0, i32 0), i32 %x3)
  br label %cond

loopend:                                          ; preds = %cond
  ret i32 0
}
