#include "Instruction.h"
#include "Block.h"

#include "ControlFlowGraph.h"

using namespace IR;

std::string reg() {
    return "%eax";
}

std::string regByte() {
    return "%al";
}

void generateAsmComp(const Instruction& inst, std::ostream& o, const Variable& lhs) {
    o << "\tcmpl\t" << reg() << ", " << inst.varToAsm(lhs) << "\n";
}

void generateAsmCompFlags(std::ostream& o, const std::string& var) {
    o << "\tandb\t$1, " << var << "\n";
    o << "\tmovzbl\t" << var << ", " << reg() << "\n";
}

std::string Instruction::varToAsm(const Variable& variable) const
{
    return std::to_string(variable.offset) + "(%rbp)";
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
    o << "\tmovl\t" << reg() << ", %ebx\n";
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
    generateAsmComp(*this, o, lhs);
    o << "\tsetg\t%al\n";
    generateAsmCompFlags(o, "%al");
}

void CompLt::generateAsm(std::ostream& o) const
{
    generateAsmComp(*this, o, lhs);
    o << "\tsetl\t%al\n";
    generateAsmCompFlags(o, "%al");
}

void CompGtEq::generateAsm(std::ostream& o) const
{
    generateAsmComp(*this, o, lhs);
    o << "\tsetge\t%al\n";
    generateAsmCompFlags(o, "%al");
}

void CompLtEq::generateAsm(std::ostream& o) const
{
    generateAsmComp(*this, o, lhs);
    o << "\tsetle\t%al\n";
    generateAsmCompFlags(o, "%al");
}

void CompEq::generateAsm(std::ostream& o) const
{
    generateAsmComp(*this, o, lhs);
    o << "\tsete\t%al\n";
    generateAsmCompFlags(o, "%al");
}
void CompNe::generateAsm(std::ostream& o) const
{
    generateAsmComp(*this, o, lhs);
    o << "\tsetne\t%al\n";
    generateAsmCompFlags(o, "%al");
}

void Return::generateAsm(std::ostream& o) const
{
}

void IR::Br::generateAsm(std::ostream& o) const
{
    o << "\tjmp\t" << target.getLabel() << "\n";
}

void IR::BrTrue::generateAsm(std::ostream& o) const
{
    o << "\tcmpl\t$0, " << reg() << "\n";
    o << "\tjne\t" << target.getLabel() << "\n";
}

void IR::CastBool::generateAsm(std::ostream& o) const
{
    o << "\tcmpl\t$0, " << reg() << "\n";
    o << "\tsetne\t" << regByte() << "\n";
    o << "\tandl\t$1, " << reg() << "\n";
}
