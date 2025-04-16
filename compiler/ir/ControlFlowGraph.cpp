#include "ControlFlowGraph.h"
using namespace IR;

ControlFlowGraph::ControlFlowGraph() : _memorySize(0), _tmpCount(0) {}
ControlFlowGraph::~ControlFlowGraph() {}

void ControlFlowGraph::generateAsm(std::ostream& o) const
{
    for (auto&& block : _blocks) {
        block.get()->generateAsm(o);
    }
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

BasicBlock& ControlFlowGraph::createAndAddBlock(std::string label) {
    addBlock(createBlock(label));
    return getCurrentBlock();
}

std::unique_ptr<BasicBlock> ControlFlowGraph::createBlock(std::string label) {
    static int _nextBlocknumber = 0; // just for naming
    if (label == "") label = ".L" + std::to_string(_nextBlocknumber++);
    return std::make_unique<BasicBlock>(*this, label);
}

void ControlFlowGraph::addBlock(std::unique_ptr<BasicBlock> block) {
    _blocks.push_back(std::move(block));
}

const Variable& ControlFlowGraph::createSymbolVar(std::string name)
{
    auto variable = std::make_shared<Variable>(name, reserveSpace(4));
    _symbols.push_back(variable);
    _contextSymbolMaps[_contextSymbolMaps.size() - 1][name] = variable.get();
    return *variable.get();
}

const Variable& ControlFlowGraph::createSymbolVar(std::string name, int address)
{
    auto variable = std::make_shared<Variable>(name, address);
    _symbols.push_back(variable);
    _contextSymbolMaps[_contextSymbolMaps.size() - 1][name] = variable.get();
    return *variable.get();
}

const Variable& ControlFlowGraph::createTmpVar()
{
    std::string name = "!" + std::to_string(_tmpCount++);
    auto variable = std::make_shared<Variable>(name, reserveSpace(4));
    _symbols.push_back(variable);
    _contextTmpMaps[_contextTmpMaps.size() - 1][name] = variable.get();
    return *variable.get();
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
    _memorySize += size;
    return -_memorySize;
}