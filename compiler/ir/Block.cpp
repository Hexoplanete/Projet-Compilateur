#include "Block.h"
#include "ControlFlowGraph.h"
using namespace IR;

BasicBlock::BasicBlock(ControlFlowGraph& cfg, std::string label) :cfg(cfg), label(label), exitTrue(nullptr), exitFalse(nullptr) {}

void BasicBlock::generateAsm(std::ostream& o)
{
    if (exitTrue) addInstruction<BrTrue>(*exitTrue);
    if (exitFalse) addInstruction<Br>(*exitFalse);
    
    o << label << ":" << "\n";
    for (auto&& inst : instructions) {
        inst.get()->generateAsm(o);
    }
}
void IR::BasicBlock::setExit(BasicBlock& block)
{
    exitTrue = nullptr;
    exitFalse = &block;
}

void IR::BasicBlock::setExitTrue(BasicBlock& block)
{
    exitTrue = &block;
}

void IR::BasicBlock::setExitFalse(BasicBlock& block)
{
    exitFalse = &block;
}
