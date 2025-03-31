#include "ControlFlowGraph.h"

using namespace IR;

IR::ControlFlowGraph::ControlFlowGraph(): nextBBnumber(0), memoryTop(0){}
IR::ControlFlowGraph::~ControlFlowGraph(){}

void IR::ControlFlowGraph::addToSymbolTable(std::string name)
{
    symbolIndex[name] = reserveSpace(4);    
}

std::string IR::ControlFlowGraph::createTmpvar()
{
    std::string name = "@tmp" + std::to_string(tmpIndex.size());
    tmpIndex[name] = reserveSpace(4);
    return name;
}

int IR::ControlFlowGraph::getVarIndex(std::string name)
{
    return (symbolIndex.find("f") != symbolIndex.end()) ? symbolIndex.at(name) : tmpIndex.at(name);
}

BasicBlock& IR::ControlFlowGraph::getCurrentBlock()
{
    return *blocks.at(blocks.size() - 1).get();
}

int IR::ControlFlowGraph::reserveSpace(int size)
{
    memoryTop -= size;
    return memoryTop;
}