; ModuleID = 'EvaLLVM'
source_filename = "EvaLLVM"

@VERSION = global i32 42, align 4
@0 = private unnamed_addr constant [17 x i8] c"Is X ==42?: %d\\n\00", align 1

declare i32 @printf(i8*, ...)

define i32 @main() {
entry:
  %z = alloca i32
  store i32 32, i32* %z
  %z1 = load i32, i32* %z
  %tmpadd = add i32 %z1, 10
  %x = alloca i32
  store i32 %tmpadd, i32* %x
  %x2 = load i32, i32* %x
  %tmpcmp = icmp eq i32 %x2, 42
  %0 = call i32 (i8*, ...) @printf(i8* getelementptr inbounds ([17 x i8], [17 x i8]* @0, i32 0, i32 0), i1 %tmpcmp)
  ret i32 0
}
