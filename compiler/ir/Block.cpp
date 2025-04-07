#include "Block.h"

using namespace IR;

BasicBlock::BasicBlock(ControlFlowGraph& cfg, std::string label) :cfg(cfg), label(label), exitTrue(nullptr), exitFalse(nullptr) {}

void BasicBlock::generateAsm(std::ostream& o)
{
    o << "." << label << ":" << "\n";
    for (auto&& inst : instructions) {
        inst.get()->generateAsm(o);
    }

    if (exitTrue) addInstruction<BrTrue>(*exitTrue);
    addInstruction<Br>(*exitFalse);
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
