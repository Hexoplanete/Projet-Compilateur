#include "Instruction.h"
#include "Block.h"

#include "ControlFlowGraph.h"

std::string IR::Instruction::reg() const
{
    return "%eax";
}

std::string IR::Instruction::varToAsm(std::string var) const
{
    return block.getCFG().varToAsm(var);
}

void IR::LdConst::generateAsm(std::ostream& o) const
{
    o << "\tmovl\t$" << std::to_string(value) << ", " << reg() << "\n";
}

void IR::LdLoc::generateAsm(std::ostream& o) const
{
    o << "\tmovl\t" << varToAsm(loc) << ", " << reg() << "\n";
}

void IR::Store::generateAsm(std::ostream& o) const
{
    o << "\tmovl\t"<< reg() << ", " << varToAsm(loc) << "\n";
}

void IR::Add::generateAsm(std::ostream& o) const
{
    o << "\taddl\t" << varToAsm(lhs) << "," << reg() << "\n";
}

void IR::Sub::generateAsm(std::ostream& o) const
{
    o << "\tsubl\t" << reg() << "," << varToAsm(lhs) << "\n";
    LdLoc(block, lhs).generateAsm(o);
}

void IR::Mul::generateAsm(std::ostream& o) const
{
    o << "\timull\t" << varToAsm(lhs) << "," << reg() << "\n";
}

void IR::Div::generateAsm(std::ostream& o) const
{
    o << "\tmovl\t %eax, %ebx\n";
    LdLoc(block, lhs).generateAsm(o);
    o << "\tcltd\n"; // 'cltd' = 'Convert Long To Double'
    o << "\tidivl\t%ebx\n";
}

void IR::Mod::generateAsm(std::ostream& o) const
{
    o << "\tmovl\t %eax, %ebx\n";
    LdLoc(block, lhs).generateAsm(o);
    o << "\tcltd\n"; // 'cltd' = 'Convert Long To Double'
    o << "\tidivl\t%ebx\n";
    o << "\tmovl\t%edx, %eax\n";
}

void IR::Negate::generateAsm(std::ostream& o) const
{
    o << "\tnegl\t%eax\n";
}

void IR::Return::generateAsm(std::ostream& o) const
{
}
