#include "./src/EvaLLVM.h"

int main(int argc, char const *argv[]){


    //program to execute
    std::string program = R"(
        (var x 42)
        (begin
            (var (x string) "Hello")
            (printf "X: %s\n\n" x))

        (printf "X: %d\n\n" x)
        (set x 100)
        (printf "X:%d\n\n" x)
        

    )";
    //compiler instance
    EvaLLVM vm;

    vm.exec(program);
    return 0;
}