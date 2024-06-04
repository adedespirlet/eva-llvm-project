; ModuleID = 'EvaLLVM'
source_filename = "EvaLLVM"

@VERSION = global i32 42, align 4

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
  br i1 %tmpcmp, label %then, label %else6

then:                                             ; preds = %entry
  %x3 = load i32, i32* %x
  %tmpcmp4 = icmp ugt i32 %x3, 42
  br i1 %tmpcmp4, label %then5, label %else

then5:                                            ; preds = %then
  br label %ifend

else:                                             ; preds = %then
  br label %ifend

ifend:                                            ; preds = %else, %then5
  %tmpif = phi i32 [ 1, %then5 ], [ 3, %else ]
  br label %ifend7

else6:                                            ; preds = %entry
  br label %ifend7

ifend7:                                           ; preds = %else6, %ifend
  %tmpif8 = phi i32 [ %tmpif, %ifend ], [ 2, %else6 ]
  ret i32 0
}
