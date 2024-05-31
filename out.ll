; ModuleID = 'EvaLLVM'
source_filename = "EvaLLVM"

@0 = private unnamed_addr constant [13 x i8] c"Hello world!\00", align 1

define i32 @main() {
entry:
  ret i32 ptrtoint ([13 x i8]* @0 to i32)
}
