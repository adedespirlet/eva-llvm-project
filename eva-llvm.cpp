#include "./src/EvaLLVM.h"

int main(int argc, char const *argv[]){


    //[program to execute
    std::string program = R"(

        (printf "Value: %d" 42)
        

    )";
    //compiler instance
    EvaLLVM vm;

    vm.exec(program);
    return 0;
}