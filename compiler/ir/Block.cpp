#include "Block.h"

IR::BasicBlock::BasicBlock(ControlFlowGraph& cfg, std::string label) :cfg(cfg), label(label), exitTrue(nullptr), exitFalse(nullptr) {}

void IR::BasicBlock::generateAsm(std::ostream& o)
{
    o << "." << label << ":" << "\n";
    for (auto&& inst : instructions) {
        inst.get()->generateAsm(o);
    }

    // TODO branch if needed
}
