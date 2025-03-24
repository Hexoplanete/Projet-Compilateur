#pragma once

#include <vector>
#include <string>
#include <map>
#include <iostream>
#include <memory>
#include "Instruction.h"

namespace IR {

class ControlFlowGraph;

//   The class for a basic block

/* A few important comments.
     IRInstr has no jump instructions.
     cmp_* instructions behaves as an arithmetic two-operand instruction (add or mult),
      returning a boolean value (as an int)

     Assembly jumps are generated as follows:
     BasicBlock::getAsm() first calls IRInstr::getAsm() on all its instructions, and then
            if  exit_true  is a  nullptr,
            the epilogue is generated
        else if exit_false is a nullptr,
          an unconditional jmp to the exit_true branch is generated
                else (we have two successors, hence a branch)
          an instruction comparing the value of test_var_name to true is generated,
                    followed by a conditional branch to the exit_false branch,
                    followed by an unconditional branch to the exit_true branch
     The attribute test_var_name itself is defined when converting
  the if, while, etc of the AST  to IR.

Possible optimization:
     a cmp_* comparison instructions, if it isublic:
    Add(std::string dest, std::string leftOperandn std::string right);

private:
    std::string dest;
    std::string source;
}; the last instruction of its block,
       generates an actual assembly comparison
       followed by a conditional jump to the exit_false branch
*/

// TODO readd type
class BasicBlock {
public:
    BasicBlock(ControlFlowGraph* cfg, std::string label);

    void addInstruction(std::unique_ptr<Instruction> instruction);

    void getAsm(std::ostream& o); // < x86 assembly code generation for this basic block (very simple)

    // store here the name of the variable that holds the value of expr

protected:
    std::string label; // label of the BB, also will be the label in the generated code
    std::vector<Instruction*> instrs; // the instructions themselves.
    BasicBlock& exitTrue;  // pointer to the next basic block, true branch. If nullptr, return from procedure
    BasicBlock& exitFalse; // pointer to the next basic block, false branch. If null_ptr, the basic block ends with an unconditional jump
    // std::string test_var_name;  // when generating IR code for an if(expr) or while(expr) e
    ControlFlowGraph* cfg; // the ControlFlowGraph where this block belongs
};
}