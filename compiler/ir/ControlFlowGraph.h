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
`
 */
class ControlFlowGraph {
public:
    ControlFlowGraph();
    ~ControlFlowGraph();

    template<typename... BArgs>
    BasicBlock& createBlock(std::string name) {
        blocks.push_back(std::make_unique<BasicBlock>(*this, name));
        return getCurrentBlock();
    }

    // x86 code generation: could be encapsulated in a processor class in a retargetable compiler
    void getAsm(std::ostream& o);
    std::string IRRegToAsm(std::string reg); // helper method: inputs a IR reg or input variable, returns e.g. "-24(%rbp)" for the proper value of 24
    void getAsmPrologue(std::ostream& o);
    void getAsmEpilogue(std::ostream& o);

    // symbol table methods
    void addToSymbolTable(std::string name /*, Type t*/);
    std::string createTmpvar(/*Type t*/);
    int getVarIndex(std::string name);
    // Type getVarType(std::string name);

    BasicBlock& getCurrentBlock();
    // basic block management
    // std::string new_BB_name();
    
protected:
    int reserveSpace(int size);
    // std::map<std::string, Type> SymbolType; // part of the symbol table
    std::map<std::string, int> symbolIndex; // part of the symbol table
    std::map<std::string, int> tmpIndex; // part of the temporary symbol table
    int memoryTop; // to allocate new symbols in the symbol table


    std::vector<std::unique_ptr<BasicBlock>> blocks; // all the basic blocks of this CF
    int nextBBnumber; // just for naming
    BasicBlock* current_bb;
};

}
