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
#include "./parser/EvaParser.h"
#include "./Environment.h"


using syntax::EvaParser;
using Env= std::shared_ptr<Environment>;
#define GEN_BINARY_OP(Op,varName)        \
    do{                                  \
        auto op1=gen(exp.list[1],env);   \
        auto op2= gen(exp.list[2],env);   \
        return builder->Op(op1,op2,varName);  \
    }while (false)

class EvaLLVM{
    public:

    EvaLLVM(): parser(std::make_unique<EvaParser>()){
        moduleInit();
        setupExternFunctions();
        setupGlobalEnvironment();
    }
    //$ denotes that parameter is a reference to a std::string object
    void exec(const std::string& program){

        //1 parse the program, outputs an AST
        auto ast = parser->parse("(begin"+program+")");

        //2 compile to LLVM IR from ast
        compile(ast);
        std::cout << "\n";
        //print generated code to the console
        module->print(llvm::outs(),nullptr);

    
        //3 save module IR to file
        savedModuleToFile("./out.ll");
    }

    private:

    

    void compile(const Exp& ast){
        //
        //start generation of code in globalenvironment
        fn= createFunction("main", llvm::FunctionType::get(builder->getInt32Ty(),false),GlobalEnv);
        //recursive compiler for the body function
        
        //auto i32Result= builder->CreateIntCast(result,builder->getInt32Ty(),true);
        //createGlobalVar("VERSION",builder->getInt32(42));
        auto result =gen(ast,GlobalEnv);
        //builder->CreateRet(i32Result);
        builder->CreateRet(builder->getInt32(0));

    }

    std::string extractVarName(const Exp& exp){
        return exp.type == ExpType::LIST? exp.list[0].string :exp.string;
    }

    llvm::Type* extractVarType(const Exp& exp){
        return exp.type == ExpType::LIST? getTypeFromString(exp.list[1].string) :builder->getInt32Ty();
    }

    llvm::Type* getTypeFromString(const std::string& type_){
        if (type_ =="number"){
            return builder->getInt32Ty();
        }

        else if (type_=="string"){
            return builder->getInt8Ty()->getPointerTo();
        }

    return builder->getInt32Ty();
    }

    //allocates a local variable on the stack
    llvm::Value* allocVar(const std::string& name,llvm::Type* type_,Env env){
        varsBuilder->SetInsertPoint(&fn->getEntryBlock());
        //create alloca instruction in IR that allocates memory on the stack for local variable
        auto varAlloc = varsBuilder->CreateAlloca(type_,0,name.c_str());

        env->define(name,varAlloc);
        return varAlloc;

    }
    //create a global variable
    llvm::GlobalVariable* createGlobalVar(const std::string& name, llvm::Constant* init){
        module->getOrInsertGlobal(name,init->getType());
        auto variable =module->getNamedGlobal(name);
        variable->setAlignment(llvm::MaybeAlign(4));
        variable->setInitializer(init);
        return variable;
    }
    //define external functions
    void setupExternFunctions(){

        //getInt8Ty() metho dproved by llvmcontext class to create an 8bit int type
        auto bytePtrTy = builder-> getInt8Ty()->getPointerTo();
        //int printf(const char* format, ...)
        module->getOrInsertFunction("printf",llvm::FunctionType::get(/*return type*/builder->getInt32Ty(),/*arguments*/bytePtrTy,/*vararg*/ true));
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
        varsBuilder= std::make_unique<llvm::IRBuilder<>>(*ctx);
    }

    //second builder will points to the begining of the function where we ll be emitting the variables, it will reuse the same module etc just
    void setupGlobalEnvironment(){
        //Create a map of initial global variables.
        std::map<std::string,llvm::Value*> globalObject{
            {"VERSION",builder->getInt32(42)},
        };
        //Create an empty map for the global record
        std::map<std::string,llvm::Value*> globalRec{};
        //Populate the global record map by creating global variables in IR
        for (auto& entry: globalObject){
            globalRec[entry.first]= createGlobalVar(entry.first, (llvm::Constant*)entry.second);
        }
        //parent link is set to nullpntr
        GlobalEnv =std::make_shared<Environment>(globalRec,nullptr);
    }
    llvm::Function* fn; 

    std::unique_ptr<EvaParser> parser;

    //need to initialize those to make them global and not local to the moduleInit() function
    //global LLVM context
    std::unique_ptr<llvm::LLVMContext> ctx;

    //module contains global functions

    std::unique_ptr<llvm::Module> module;


    std::unique_ptr<llvm::IRBuilder<>> builder;

    std::unique_ptr<llvm::IRBuilder<>> varsBuilder;

    std::shared_ptr<Environment> GlobalEnv;



    //main compile loop
    llvm::Value* gen(const Exp&  exp, Env env){
        //return  builder->getInt32(42); 
        // auto str = builder->CreateGlobalStringPtr("Hello world!") ;
        // auto printfFn = module->getFunction("printf");
        // //args:
        // std::vector <llvm::Value*> args{str};
        // //create call instruction to printf fct
        // return builder->CreateCall(printfFn,args);


        //need to handle different expression types

        switch(exp.type){
            case ExpType::NUMBER:
                printf("number");
                //creates a 32bit int constant in the IR
                 return builder->getInt32(exp.number);

            case ExpType::STRING:
                 return builder->CreateGlobalStringPtr(exp.string);
            //variables,operators
            case ExpType::SYMBOL:
                //boolean check
                if (exp.string == "true" || exp.string== "false"){
                    return builder->getInt1(exp.string =="true"? true:false);
                }else{
                    //variables
                    auto varName= exp.string;
                    auto value= env->lookup(varName);
                    //1.local variables
                    if (auto localVar = llvm::dyn_cast<llvm::AllocaInst>(value)) {
                        return builder->CreateLoad(localVar->getAllocatedType(),localVar,varName.c_str());
                    }
                    
                    //2. Check if ther variable name is a global variables, globalVar should be non NULL
                    else if (auto globalVar = llvm::dyn_cast<llvm::GlobalVariable>(value)){
                        //if we determine that this value is a global variable we need to load this value onto the stack by taking pointer tot the globalVar
                        return builder->CreateLoad(globalVar->getInitializer()->getType(),globalVar,varName.c_str());
                    }
                }

                return builder->getInt32(0);

            case ExpType::LIST:
                
                auto tag=exp.list[0]; 
                if (tag.type == ExpType::SYMBOL){
                    auto op= tag.string;

                    //binary math operations
                    if(op =="+"){
                        GEN_BINARY_OP(CreateAdd,"tmpadd");
                    }

                    else if(op == "-"){
                        GEN_BINARY_OP(CreateSub,"tmpsub");
                    }
                    else if(op == "*"){
                        GEN_BINARY_OP(CreateMul,"tmpsub");
                    }
                    else if(op == "/"){
                        GEN_BINARY_OP(CreateSDiv,"tmpdiv");
                    }

                    else if(op == ">"){
                        GEN_BINARY_OP(CreateICmpUGT,"tmpcmp");
                    }
                    else if(op == "<"){
                        GEN_BINARY_OP(CreateICmpULT,"tmpcmp");
                    }
                    else if(op == "=="){
                        GEN_BINARY_OP(CreateICmpEQ,"tmpcmp");
                    }
                    ///....////
                    
                    //variable declaration
                    else if (op =="var"){
                        printf("debug");

                        auto varNameDecl =exp.list[1];
                        
                        //extract name and type
                        auto varName= extractVarName(varNameDecl);
                        auto varTy=extractVarType(varNameDecl);

                        auto init= gen(exp.list[2],env);

                        auto varBinding= allocVar(varName,varTy,env);
                        
                        return builder->CreateStore(init,varBinding);

                        
                        //return createGlobalVar(varName,(llvm::Constant*)init)->getInitializer();
                    }
                    else if (op=="set"){
                        auto value= gen(exp.list[2],env); //generates llvm i32 cnstant
                        auto varName=exp.list[1].string;
                        //lookup vairbale in environment which is either a AllocaInst or a GlobalVariable inst, it returns pointer to memory loc of var
                        auto varBinding=env->lookup(varName);
                        return builder->CreateStore(value,varBinding);

                    }
                    else if(op=="begin"){

                        auto blockEnv =std::make_shared<Environment>(std::map<std::string,llvm::Value*>{},env);
                        llvm::Value* blockRes;
                        //go through list of expressions and egenerate expression code
                        for (auto i=1;i<exp.list.size();i++){
                            blockRes=gen(exp.list[i],blockEnv);

                        }
                        //last expression related is the result of the block
                        return blockRes;
                        
                    }
                   
                    else if (op == "printf"){
                        auto printfFn = module->getFunction("printf");
                         std::vector<llvm::Value*> args{};
                         for (auto i =1; i<exp.list.size();i++){
                            args.push_back(gen(exp.list[i],env));
                         }
                         return builder->CreateCall(printfFn,args);
                    }
                }

        return builder->getInt32(0);
        
        }

    }

    llvm::Function* createFunction(const std::string& fnName, llvm::FunctionType* fnType, Env env){
        //check if funcrion protoype is alredy defined
        auto fn = module->getFunction(fnName);
        //if functionname doesnt exist create functionprototype
        if (fn ==nullptr){
            fn=createFunctionProto(fnName,fnType,env);
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

    llvm::Function* createFunctionProto(const std::string& fnName,llvm::FunctionType* fnType, Env env){
        auto fn = llvm::Function::Create(fnType,llvm::Function::ExternalLinkage,fnName,*module);
        verifyFunction(*fn);
        env->define(fnName,fn);

        return fn;
    }


};

#endif
