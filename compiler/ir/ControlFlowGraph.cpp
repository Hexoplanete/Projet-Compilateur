#include "ControlFlowGraph.h"
#include <format>
using namespace IR;

ControlFlowGraph::ControlFlowGraph() : _memoryTop(0), _tmpCount(0) {}
ControlFlowGraph::~ControlFlowGraph() {}

void ControlFlowGraph::generateAsm(std::ostream& o) const
{
    generateAsmPrologue(o);
    generateAsmBody(o);
    generateAsmEpilogue(o);
}

void ControlFlowGraph::generateAsmPrologue(std::ostream& o) const
{
    o << ".globl main\n";
    o << "main:\n";
    o << "\t# prologue:\n";
    o << "\tpushq\t%rbp # save %rbp on the stack\n";
    o << "\tmovq\t%rsp, %rbp # define %rbp for the current function\n\n";
}

void ControlFlowGraph::generateAsmBody(std::ostream& o) const
{
    o << "\t# body:\n";
    for (auto&& block : _blocks) {
        block.get()->generateAsm(o);
    }
}

void ControlFlowGraph::generateAsmEpilogue(std::ostream& o) const
{
    o << "\n\t# epilogue:\n";
    o << "\tpopq\t%rbp # restore %rbp from the stack\n";
    o << "\tret # return to the caller (here the shell)" << std::endl;
}

void ControlFlowGraph::pushContext()
{
    _contextSymbolMaps.push_back(std::map<std::string, Variable*>());
    _contextTmpMaps.push_back(std::map<std::string, Variable*>());
}

void ControlFlowGraph::popContext()
{
    _contextSymbolMaps.pop_back();
    _contextTmpMaps.pop_back();
}

const Variable& ControlFlowGraph::createSymbolVar(std::string name)
{
    Variable& variable = _symbols.emplace_back(name, reserveSpace(4));
    _contextSymbolMaps[_contextSymbolMaps.size() - 1][name] = &variable;
    std::cout << std::to_string(_memoryTop) << std::endl;
    return variable;
}

const Variable& ControlFlowGraph::createTmpVar()
{
    std::string name = "!" + std::to_string(_tmpCount++);
    Variable& variable = _symbols.emplace_back(name, reserveSpace(4));
    _contextTmpMaps[_contextTmpMaps.size() - 1][name] = &variable;
    std::cout << std::to_string(_memoryTop) << std::endl;
    return variable;
}

const Variable& IR::ControlFlowGraph::getSymbolVar(std::string varname)
{
    for (int i = _contextSymbolMaps.size() - 1; i >= 0; i--) {
        const auto& tmpMap = _contextTmpMaps.at(i);
        const auto& symbolMap = _contextSymbolMaps[i];
        switch (varname[0]) {
        case '!': {}
                if (tmpMap.find(varname) != tmpMap.end()) return *tmpMap.at(varname);
                break;
        default:
            if (symbolMap.find(varname) != symbolMap.end()) return *symbolMap.at(varname);
            break;
        }
    }
    std::cerr << "error: Undefined variable (something went very wrong)." << std::endl;
    exit(1);
}

BasicBlock& ControlFlowGraph::getCurrentBlock() const
{
    return *_blocks.at(_blocks.size() - 1).get();
}

int ControlFlowGraph::reserveSpace(int size)
{
    _memoryTop -= size;
    return _memoryTop;
}