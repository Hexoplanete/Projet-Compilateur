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
    // The instructions themselves - feel free to subclass instead
    enum Operation {

        // Stack variables
        LdConst,
        Copy,

        // TODO
        // Heap variables
        // MemRead, MemWrite,

        // TODO
        // Functions
        // Call,

        // Arithmetic operators
        Add, Sub, Mul, Div, Mod,
        Negate,

        // TODO
        // Comparison operators
        // CmpEq, CmpLt, CmpLe

        // Control instructions
        Return,
    };



    //  Actual code generation
    void getAsm(std::ostream& o); // < x86 assembly code generation for this IR instruction

    virtual ~Instruction();

protected:
    //   constructor
    Instruction(BasicBlock* bb, Operation op /*, Type t*/);

    // variables
    BasicBlock* bb; // < The BB this instruction belongs to, which provides a pointer to the CFG this instruction belongs to
    Operation op;
    // Type t;
};

class LdConst : public Instruction {
public:
    LdConst(std::string dest, int constValue);

private:
    std::string dest;
    int value;
};

class Copy : public Instruction {
public:
    Copy(std::string dest, std::string source);

private:
    std::string dest;
    std::string source;
};


class Add : public Instruction {
public:
    Add(std::string dest, std::string leftOperand, std::string rightOperand);

private:
    std::string leftOperand;
    std::string rightOperand;
};

class Sub : public Instruction {
public:
    Sub(std::string dest, std::string leftOperand, std::string rightOperand);

private:
    std::string leftOperand;
    std::string rightOperand;
};

class Mul : public Instruction {
public:
    Mul(std::string dest, std::string leftOperand, std::string rightOperand);

private:
    std::string leftOperand;
    std::string rightOperand;
};

class Div : public Instruction {
public:
    Div(std::string dest, std::string leftOperand, std::string rightOperand);

private:
    std::string leftOperand;
    std::string rightOperand;
};


class Mod : public Instruction {
public:
    Mod(std::string dest, std::string leftOperand, std::string rightOperand);

private:
    std::string leftOperand;
    std::string rightOperand;
};

class Negate : public Instruction {
public:
    Negate(std::string dest, std::string operand);

private:
    std::string dest;
    std::string operand;
};

class Return : public Instruction {
    public:
        Return(std::string operand);
    
    private:
        std::string operand;
    };
}
