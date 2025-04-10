#pragma once

#include <vector>
#include <set>
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
struct Variable {
    Variable(std::string name, int offset) : name(name), offset(offset) {}
    std::string name;
    int offset;
};

class ControlFlowGraph {
public:
    ControlFlowGraph();
    ~ControlFlowGraph();

    template<typename... BArgs>
    BasicBlock& createBlock(std::string label = "") {
        static int _nextBlocknumber = 0; // just for naming
        if (label == "") label = ".L" + std::to_string(_nextBlocknumber++);
        _blocks.push_back(std::make_unique<BasicBlock>(*this, label));
        return getCurrentBlock();
    }

    // x86 code generation: could be encapsulated in a processor class in a retargetable compiler
    void generateAsm(std::ostream& o) const;

    // symbol table methods
    void pushContext();
    void popContext();
    const Variable& createSymbolVar(std::string name /*, Type t*/);
    const Variable& createSymbolVar(std::string name, int address /*, Type t*/);
    const Variable& getSymbolVar(std::string name);
    const Variable& createTmpVar(/*Type t*/);

    BasicBlock& getCurrentBlock() const;
    
    inline void resetMemoryCount() { _memoryTop = 0; _tmpCount = 0; }
protected:
    int reserveSpace(int size);
    
    std::vector<std::shared_ptr<Variable>> _symbols;
    int _memoryTop; // to allocate new symbols in the symbol table
    std::vector<std::map<std::string, Variable*>> _contextSymbolMaps; // part of the symbol table
    std::vector<std::map<std::string, Variable*>> _contextTmpMaps; // part of the temporary symbol table
    int _tmpCount; // just for naming

    std::vector<std::unique_ptr<BasicBlock>> _blocks; // all the basic blocks of this CF
    BasicBlock* _currentBB;
};

}
