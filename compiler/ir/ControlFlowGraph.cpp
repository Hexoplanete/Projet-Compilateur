#include "ControlFlowGraph.h"

void IR::ControlFlowGraph::addBlock(std::unique_ptr<BasicBlock> block)
{
    blocks.push_back(std::move(block));
}