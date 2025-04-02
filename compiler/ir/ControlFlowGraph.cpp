#include "ControlFlowGraph.h"
#include <format>
using namespace IR;

IR::ControlFlowGraph::ControlFlowGraph() : nextBBnumber(0), memoryTop(0) {}
IR::ControlFlowGraph::~ControlFlowGraph() {}

void IR::ControlFlowGraph::generateAsm(std::ostream& o) const
{
    generateAsmPrologue(o);
    generateAsmBody(o);
    generateAsmEpilogue(o);
}

void IR::ControlFlowGraph::generateAsmPrologue(std::ostream& o) const
{
    o << ".globl main\n";
    o << "main:\n";
    o << "\t# prologue:\n";
    o << "\tpushq\t%rbp # save %rbp on the stack\n";
    o << "\tmovq\t%rsp, %rbp # define %rbp for the current function\n\n";
}

void IR::ControlFlowGraph::generateAsmBody(std::ostream& o) const
{
    o << "\t# body:\n";
    for (auto&& block : blocks) {
        block.get()->generateAsm(o);
    }
}

void IR::ControlFlowGraph::generateAsmEpilogue(std::ostream& o) const
{
    o << "\n\t# epilogue:\n";
    o << "\tpopq\t%rbp # restore %rbp from the stack\n";
    o << "\tret # return to the caller (here the shell)" << std::endl;
}

std::string IR::ControlFlowGraph::varToAsm(std::string variable) const
{
    switch (variable[0]) {
    case '?': return "%eax";
    case '!': return std::to_string(tmpIndex.at(variable)) + "(%rbp)";
    default:  return std::to_string(symbolIndex.at(variable)) + "(%rbp)";
    }
}

void IR::ControlFlowGraph::createSymbolVar(std::string name)
{
    symbolIndex[name] = reserveSpace(4);
}

std::string IR::ControlFlowGraph::createTmpVar()
{
    std::string name = "!" + std::to_string(tmpIndex.size());
    tmpIndex[name] = reserveSpace(4);
    return name;
}

BasicBlock& IR::ControlFlowGraph::getCurrentBlock() const
{
    return *blocks.at(blocks.size() - 1).get();
}

int IR::ControlFlowGraph::reserveSpace(int size)
{
    memoryTop -= size;
    return memoryTop;
}