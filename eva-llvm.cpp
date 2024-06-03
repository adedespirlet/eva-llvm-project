#include "./src/EvaLLVM.h"

int main(int argc, char const *argv[]){


    //[program to execute
    std::string program = R"(
        // (var VERSION 42)
        // (begin
        //     (var VERSION "Hello")
        //     (printf "Version: %s\n\n" VERSION))

        (printf "Version: %d\n\n" VERSION)
        

    )";
    //compiler instance
    EvaLLVM vm;

    vm.exec(program);
    return 0;
}