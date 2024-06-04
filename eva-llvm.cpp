#include "./src/EvaLLVM.h"

int main(int argc, char const *argv[]){


    //program to execute
    std::string program = R"(
        (def square (x) (* x x))

        (square 2)
        (def sum ((a number) (b number)) -> number (+ a b))
        (sum 2 3) 
      

       
    )";
    //compiler instance
    EvaLLVM vm;

    vm.exec(program);
    return 0;
}