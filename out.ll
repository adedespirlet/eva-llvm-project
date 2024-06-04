; ModuleID = 'EvaLLVM'
source_filename = "EvaLLVM"

@VERSION = global i32 42, align 4

declare i32 @printf(i8*, ...)

define i32 @main() {
entry:
  %0 = call i32 @square(i32 2)
  %1 = call i32 @sum(i32 2, i32 3)
  ret i32 0
}

define i32 @square(i32 %x) {
entry:
  %x1 = alloca i32
  store i32 %x, i32* %x1
  %x2 = load i32, i32* %x1
  %x3 = load i32, i32* %x1
  %tmpsub = mul i32 %x2, %x3
  ret i32 %tmpsub
}

define i32 @sum(i32 %a, i32 %b) {
entry:
  %a1 = alloca i32
  store i32 %a, i32* %a1
  %b2 = alloca i32
  store i32 %b, i32* %b2
  %a3 = load i32, i32* %a1
  %b4 = load i32, i32* %b2
  %tmpadd = add i32 %a3, %b4
  ret i32 %tmpadd
}
