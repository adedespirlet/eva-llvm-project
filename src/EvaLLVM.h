#ifndef EvaLLVM_h
#define EvaLLVM_h

#include <string>
#include <llvm/IR/IRBuilder.h>
#include <llvm/IR/LLVMContext.h>
#include <llvm/IR/Module.h>
#include <llvm/Support/raw_ostream.h>
#include <llvm/Support/FileSystem.h>
#include <llvm/IR/Verifier.h>
#include <iostream>


class EvaLLVM{
    public:

    EvaLLVM(){
        moduleInit();
    }
    //$ denotes that parameter is a reference to a std::string object
    void exec(const std::string& program){

        //1 parse the program, outputs an AST
        

        //2 compile to LLVM IR from ast
        compile();

        //print generated code to the console
        module->print(llvm::outs(),nullptr);

        //3 save module IR to file
        savedModuleToFile("./out.ll");
    }

    private:

    

    void compile(){
        //
        fn= createFunction("main", llvm::FunctionType::get(builder->getInt32Ty(),false));
        //recursive compiler for the body function
        auto result =gen();

        auto i32Result= builder->CreateIntCast(result,builder->getInt32Ty(),true);

        builder->CreateRet(i32Result);

    }

    void savedModuleToFile(const std::string& fileName){
        std::error_code errorCode;
        llvm::raw_fd_ostream outLL(fileName,errorCode);
        module->print(outLL,nullptr);
    }
    //initialize everything
    void moduleInit(){
        ctx= std::make_unique<llvm::LLVMContext>();
        module= std::make_unique<llvm::Module>("EvaLLVM",*ctx);
        builder= std::make_unique<llvm::IRBuilder<>>(*ctx);
    }

    llvm::Function* fn;

    //need to initialize those to make them global and not local to the moduleInit() function
    //global LLVM context
    std::unique_ptr<llvm::LLVMContext> ctx;

    //module contains global functions

    std::unique_ptr<llvm::Module> module;


    std::unique_ptr<llvm::IRBuilder<>> builder;



    //main compile loop
    llvm::Value* gen(){
        return  builder->getInt32(42); 
    }

    llvm::Function* createFunction(const std::string& fnName, llvm::FunctionType* fnType){
        //check if funcrion protoype is alredy defined
        auto fn = module->getFunction(fnName);
        //if functionname doesnt exist create functionprototype
        if (fn ==nullptr){
            fn=createFunctionProto(fnName,fnType);
        }

        //now function is defined create function block
        createFunctionBlock(fn);
        return fn;
    }
    void createFunctionBlock(llvm::Function* fn){
        auto entry= createBB("entry",fn);
        //once we allocate new block we tell the builder to emitnthe code related to this block
        builder->SetInsertPoint(entry);
    }

    llvm::BasicBlock* createBB(std::string name, llvm::Function* fn=nullptr){
        return llvm::BasicBlock::Create(*ctx,name,fn);
    }

    llvm::Function* createFunctionProto(const std::string& fnName,llvm::FunctionType* fnType){
        auto fn = llvm::Function::Create(fnType,llvm::Function::ExternalLinkage,fnName,*module);
        verifyFunction(*fn);

        return fn;
    }


};

#endif
