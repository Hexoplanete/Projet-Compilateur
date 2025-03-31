#include "Block.h"

IR::BasicBlock::BasicBlock(ControlFlowGraph& cfg, std::string label) :cfg(cfg), label(label), exitTrue(nullptr), exitFalse(nullptr) {}