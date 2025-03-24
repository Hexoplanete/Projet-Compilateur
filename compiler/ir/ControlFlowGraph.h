#pragma once

#include <vector>
#include <string>
#include <iostream>
#include <initializer_list>
#include "Block.h"
#include <memory>
namespace IR {

/* A few important comments:
     The entry block is the one with the same label as the AST function name.
       (it could be the first of bbs, or it could be defined by an attribute value)
     The exit block is the one with both exit pointers equal to nullptr.
     (again it could be identified in a more explicit way)

 */
class ControlFlowGraph {
public:
    ControlFlowGraph();
    ~ControlFlowGraph();

    void addBlock(std::unique_ptr<BasicBlock> block);

    // x86 code generation: could be encapsulated in a processor class in a retargetable compiler
    void getAsm(std::ostream& o);
    std::string IRRegToAsm(std::string reg); // helper method: inputs a IR reg or input variable, returns e.g. "-24(%rbp)" for the proper value of 24
    void getAsmPrologue(std::ostream& o);
    void getAsmEpilogue(std::ostream& o);

    // symbol table methods
    void addToSymbolTable(std::string name /*, Type t*/);
    std::string createNewTempvar(/*Type t*/);
    int getVarIndex(std::string name);
    // Type getVarType(std::string name);

    BasicBlock& getCurrentBlock();
    // basic block management
    // std::string new_BB_name();
    
protected:
    // std::map<std::string, Type> SymbolType; // part of the symbol table
    std::map<std::string, int> SymbolIndex; // part of the symbol table
    int nextFreeSymbolIndex; // to allocate new symbols in the symbol table
    int nextBBnumber; // just for naming
    BasicBlock* current_bb;

    std::vector<std::unique_ptr<BasicBlock>> blocks; // all the basic blocks of this CF
};

}
