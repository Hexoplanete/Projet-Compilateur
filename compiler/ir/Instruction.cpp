#include "Instruction.h"
#include "Block.h"

#include "ControlFlowGraph.h"

using namespace IR;

std::string Instruction::reg()
{
    return "%eax";
}

void generateAsmComp(BasicBlock& block, std::ostream& o, const std::string& lhs) {
    o << "\tcmpl\t" << Instruction::reg() << ", " <<  block.getCFG().varToAsm(lhs) << "\n";
}

void generateAsmCompFlags(std::ostream& o, const std::string& reg) {
    o << "\tandb\t$1, " << reg << "\n";
    o << "\tmovzbl\t" << reg << ", " << Instruction::reg() << "\n";
}

std::string Instruction::varToAsm(std::string var) const
{
    return block.getCFG().varToAsm(var);
}

void LdConst::generateAsm(std::ostream& o) const
{
    o << "\tmovl\t$" << std::to_string(value) << ", " << reg() << "\n";
}

void LdLoc::generateAsm(std::ostream& o) const
{
    o << "\tmovl\t" << varToAsm(loc) << ", " << reg() << "\n";
}

void Store::generateAsm(std::ostream& o) const
{
    o << "\tmovl\t" << reg() << ", " << varToAsm(loc) << "\n";
}

void Add::generateAsm(std::ostream& o) const
{
    o << "\taddl\t" << varToAsm(lhs) << "," << reg() << "\n";
}

void Sub::generateAsm(std::ostream& o) const
{
    o << "\tsubl\t" << reg() << "," << varToAsm(lhs) << "\n";
    LdLoc(block, lhs).generateAsm(o);
}

void Mul::generateAsm(std::ostream& o) const
{
    o << "\timull\t" << varToAsm(lhs) << "," << reg() << "\n";
}

void Div::generateAsm(std::ostream& o) const
{
    o << "\tmovl\t %eax, %ebx\n";
    LdLoc(block, lhs).generateAsm(o);
    o << "\tcltd\n"; // 'cltd' = 'Convert Long To Double'
    o << "\tidivl\t%ebx\n";
}

void Mod::generateAsm(std::ostream& o) const
{
    Div(block, lhs).generateAsm(o);
    o << "\tmovl\t%edx, " << reg() << "\n";
}

void Negate::generateAsm(std::ostream& o) const
{
    o << "\tnegl\t" << reg() << "\n";
}

void LogicalNot::generateAsm(std::ostream& o) const
{
    o << "\tcmpl\t$0, " << reg() << "\n";
    o << "\tsete\t%al\n";
    generateAsmCompFlags(o, "%al");
}

void BitAnd::generateAsm(std::ostream& o) const
{
    o << "\tandl\t" << varToAsm(lhs) << "," << reg() << "\n";
}

void BitXor::generateAsm(std::ostream& o) const
{
    o << "\txorl\t" << varToAsm(lhs) << "," << reg() << "\n";
}

void BitOr::generateAsm(std::ostream& o) const
{
    o << "\torl\t" << varToAsm(lhs) << "," << reg() << "\n";
}

void CompGt::generateAsm(std::ostream& o) const
{
    generateAsmComp(block, o, lhs);
    o << "\tsetg\t%al\n";
    generateAsmCompFlags(o, "%al");
}

void CompLt::generateAsm(std::ostream& o) const
{
    generateAsmComp(block, o, lhs);
    o << "\tsetl\t%al\n";
    generateAsmCompFlags(o, "%al");
}

void CompGtEq::generateAsm(std::ostream& o) const
{
    generateAsmComp(block, o, lhs);
    o << "\tsetge\t%al\n";
    generateAsmCompFlags(o, "%al");
}

void CompLtEq::generateAsm(std::ostream& o) const
{
    generateAsmComp(block, o, lhs);
    o << "\tsetle\t%al\n";
    generateAsmCompFlags(o, "%al");
}

void CompEq::generateAsm(std::ostream& o) const
{
    generateAsmComp(block, o, lhs);
    o << "\tsete\t%al\n";
    generateAsmCompFlags(o, "%al");
}
void CompNe::generateAsm(std::ostream& o) const
{
    generateAsmComp(block, o, lhs);
    o << "\tsetne\t%al\n";
    generateAsmCompFlags(o, "%al");
}

void Return::generateAsm(std::ostream& o) const
{
}

// CompGt
// CompLt
// CompGtEq
// CompLtEq
// CompEq
// CompNe