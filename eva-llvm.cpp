#include "./src/EvaLLVM.h"

int main(int argc, char const *argv[]){


    //program to execute
    std::string program = R"(
        (var z 32)
        
        (var x (+ z 10))

        (printf "Is X ==42?: %d\n" (== x 42))

    
    )";
    //compiler instance
    EvaLLVM vm;

    vm.exec(program);
    return 0;
}