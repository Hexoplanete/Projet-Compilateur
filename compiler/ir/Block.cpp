#include "Block.h"
#include "ControlFlowGraph.h"
using namespace IR;

BasicBlock::BasicBlock(ControlFlowGraph& cfg, std::string label) :cfg(cfg), label(label), exitTrue(nullptr), exitFalse(nullptr) {}

std::string extractFunctionName(const std::string& s) {
    size_t pos = s.find('(');
    if (pos != std::string::npos) {
        return s.substr(0, pos);
    }
    return s;
}

void BasicBlock::generateAsm(std::ostream& o)
{
    o << ".globl " << extractFunctionName(label) << "\n";
    o << extractFunctionName(label) << ":" << "\n";

    if (exitTrue) addInstruction<BrTrue>(*exitTrue);
    if (exitFalse) addInstruction<Br>(*exitFalse);
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
