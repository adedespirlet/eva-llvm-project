#include "./src/EvaLLVM.h"

int main(int argc, char const *argv[]){


    //program to execute
    std::string program = R"(
        (var z 32)
        
        (var x (+ z 10))

        (if (== x 42)
            (if (> x 42)
            1
            3)
        2)

    )";
    //compiler instance
    EvaLLVM vm;

    vm.exec(program);
    return 0;
}