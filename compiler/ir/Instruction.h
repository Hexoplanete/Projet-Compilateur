#pragma once

#include <vector>
#include <string>
#include <iostream>
// Declarations from the parser -- replace with your own
// #include "type.h"
// #include "symbole.h"

namespace IR {

class BasicBlock;

// TODO readd Type
// The class for one 3-address instruction
class Instruction {

public:
    // // The instructions themselves - feel free to subclass instead
    // enum Operation {

    //     // Stack variables
    //     LdConst,
    //     Copy,

    //     // TODO
    //     // Heap variables
    //     // MemRead, MemWrite,

    //     // TODO
    //     // Functions
    //     // Call,

    //     // Arithmetic operators
    //     Add, Sub, Mul, Div, Mod,
    //     Negate,

    //     /j/ TODO
    //     // Comparison operators
    //     // CmpEq, CmpLt, CmpLe

    //     // Control instructions
    //     Return,
    // };



    //  Actual code generation
    void getAsm(std::ostream& o); // T86 assembly code generation for this IR instruction

    virtual ~Instruction() {}

protected:
    //   constructor
    Instruction(BasicBlock& block /*, Operation op, Type t*/) : block(block)/*, op(op)*/ {}

    // variables
    BasicBlock& block; // The BB this instruction belongs to, which provides a pointer to the CFG this instruction belongs to
    // Operation op;
    // Type t;
};

class LdConst : public Instruction {
public:
    LdConst(BasicBlock& block, std::string dest, int constValue) : Instruction(block), dest(dest), value(value) {}

private:
    std::string dest;
    int value;
};

class Copy : public Instruction {
public:
    Copy(BasicBlock& block, std::string dest, std::string source) : Instruction(block), dest(dest), source(source) {}

private:
    std::string dest;
    std::string source;
};


class Add : public Instruction {
public:
    Add(BasicBlock& block, std::string dest, std::string leftOperand, std::string rightOperand) : Instruction(block), dest(dest), leftOperand(leftOperand), rightOperand(rightOperand) {}

private:
    std::string dest;
    std::string leftOperand;
    std::string rightOperand;
};

class Sub : public Instruction {
public:
    Sub(BasicBlock& block, std::string dest, std::string leftOperand, std::string rightOperand) : Instruction(block), dest(dest), leftOperand(leftOperand), rightOperand(rightOperand) {}

private:
    std::string dest;
    std::string leftOperand;
    std::string rightOperand;
};

class Mul : public Instruction {
public:
    Mul(BasicBlock& block, std::string dest, std::string leftOperand, std::string rightOperand) : Instruction(block), dest(dest), leftOperand(leftOperand), rightOperand(rightOperand) {}

private:
    std::string dest;
    std::string leftOperand;
    std::string rightOperand;
};

class Div : public Instruction {
public:
    Div(BasicBlock& block, std::string dest, std::string leftOperand, std::string rightOperand) : Instruction(block), dest(dest), leftOperand(leftOperand), rightOperand(rightOperand) {}

private:
    std::string dest;
    std::string leftOperand;
    std::string rightOperand;
};


class Mod : public Instruction {
public:
    Mod(BasicBlock& block, std::string dest, std::string leftOperand, std::string rightOperand) : Instruction(block), dest(dest), leftOperand(leftOperand), rightOperand(rightOperand) {}

private:
    std::string dest;
    std::string leftOperand;
    std::string rightOperand;
};

class Negate : public Instruction {
public:
    Negate(BasicBlock& block, std::string dest, std::string operand) : Instruction(block), dest(dest), operand(operand) {}

private:
    std::string dest;
    std::string operand;
};

class Return : public Instruction {
public:
    Return(BasicBlock& block, std::string operand) : Instruction(block), operand(operand) {}

private:
    std::string operand;
};
}
