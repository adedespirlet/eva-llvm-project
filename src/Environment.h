#ifndef Environment_h
#define Environment_h
#include <string>
#include <map>
#include <memory>

#include "llvm/IR/Value.h"
#include "./Logger.h"

class Environment : public std::enable_shared_from_this<Environment>{

    public:
    //main component of the environment is the record the actual storage
    Environment(std::map<std::string,llvm::Value*> record,std::shared_ptr<Environment> parent) : record_(record), parent_(parent){}

    llvm::Value* define(const std::string& name , llvm::Value* value){
        record_[name]=value;
        return value;

    }
    //lookup for a variable int the environemnt chain
    llvm::Value* lookup(const std::string& name){
        return resolve(name)->record_[name];
    }

    private:
    std::map<std::string,llvm::Value*> record_;
    std::shared_ptr<Environment> parent_;

    

    std::shared_ptr<Environment> resolve(const std::string& name){
        if (record_.count(name)!=0){
            return shared_from_this();

        }
        //if current environment doesnt contain the nsmr, maybe the parent scope does

        if (parent_ ==nullptr){
            //in very top environment
            //if parent environment is NuLL and we didnt find the variable we exit the program
            DIE << "Variable \"" << name << "\" is not defined.";
        }
        return parent_->resolve(name);
    }
    //resolve() will traveese the environment chain

    //since multiple environemnt can share the same parent environment , we use shared pointers
};

#endif