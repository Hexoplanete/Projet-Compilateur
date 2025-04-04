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
    //  Actual code generation
    virtual void generateAsm(std::ostream& o) const = 0;
    static std::string reg();

    std::string varToAsm(std::string var) const;

    virtual ~Instruction() {}

protected:
    //   constructor
    Instruction(BasicBlock& block /*, Type t*/) : block(block) {}

    // variables
    BasicBlock& block; // The BB this instruction belongs to, which provides a pointer to the CFG this instruction belongs to
    // Type t;
};

class LdConst : public Instruction {
public:
    LdConst(BasicBlock& block, int constValue) : Instruction(block), value(constValue) {}
    void generateAsm(std::ostream& o) const override;

private:
    int value;
};

class LdLoc : public Instruction {
public:
    LdLoc(BasicBlock& block, std::string loc) : Instruction(block), loc(loc) {}
    void generateAsm(std::ostream& o) const override;

private:
    std::string loc;
};

class Store : public Instruction {
public:
    Store(BasicBlock& block, std::string loc) : Instruction(block), loc(loc) {}
    void generateAsm(std::ostream& o) const override;

private:
    std::string loc;
};

class Add : public Instruction {
public:
    Add(BasicBlock& block, std::string lhs) : Instruction(block), lhs(lhs) {}
    void generateAsm(std::ostream& o) const override;

private:
    std::string lhs;
};

class Sub : public Instruction {
public:
    Sub(BasicBlock& block, std::string lhs) : Instruction(block), lhs(lhs) {}
    void generateAsm(std::ostream& o) const override;

private:
    std::string lhs;
};


class Mul : public Instruction {
public:
    Mul(BasicBlock& block, std::string lhs) : Instruction(block), lhs(lhs) {}
    void generateAsm(std::ostream& o) const override;

private:
    std::string lhs;
};

class Div : public Instruction {
public:
    Div(BasicBlock& block, std::string lhs) : Instruction(block), lhs(lhs) {}
    void generateAsm(std::ostream& o) const override;

private:
    std::string lhs;
};

class Mod : public Instruction {
public:
    Mod(BasicBlock& block, std::string lhs) : Instruction(block), lhs(lhs) {}
    void generateAsm(std::ostream& o) const override;

private:
    std::string lhs;
};

class Negate : public Instruction {
public:
    Negate(BasicBlock& block) : Instruction(block) {}
    void generateAsm(std::ostream& o) const override;
};

class LogicalNot : public Instruction {
public:
    LogicalNot(BasicBlock& block) : Instruction(block) {}
    void generateAsm(std::ostream& o) const override;
};

class BitAnd : public Instruction {
public:
    BitAnd(BasicBlock& block, std::string lhs) : Instruction(block), lhs(lhs) {}
    void generateAsm(std::ostream& o) const override;

private:
    std::string lhs;
};

class BitXor : public Instruction {
public:
    BitXor(BasicBlock& block, std::string lhs) : Instruction(block), lhs(lhs) {}
    void generateAsm(std::ostream& o) const override;

private:
    std::string lhs;
};

class BitOr : public Instruction {
public:
    BitOr(BasicBlock& block, std::string lhs) : Instruction(block), lhs(lhs) {}
    void generateAsm(std::ostream& o) const override;

private:
    std::string lhs;
};

class CompGt : public Instruction {
public:
    CompGt(BasicBlock& block, std::string lhs) : Instruction(block), lhs(lhs) {}
    void generateAsm(std::ostream& o) const override;

private:
    std::string lhs;
};

class CompLt : public Instruction {
public:
    CompLt(BasicBlock& block, std::string lhs) : Instruction(block), lhs(lhs) {}
    void generateAsm(std::ostream& o) const override;

private:
    std::string lhs;
};

class CompGtEq : public Instruction {
public:
    CompGtEq(BasicBlock& block, std::string lhs) : Instruction(block), lhs(lhs) {}
    void generateAsm(std::ostream& o) const override;

private:
    std::string lhs;
};

class CompLtEq : public Instruction {
public:
    CompLtEq(BasicBlock& block, std::string lhs) : Instruction(block), lhs(lhs) {}
    void generateAsm(std::ostream& o) const override;

private:
    std::string lhs;
};

class CompEq : public Instruction {
public:
    CompEq(BasicBlock& block, std::string lhs) : Instruction(block), lhs(lhs) {}
    void generateAsm(std::ostream& o) const override;

private:
    std::string lhs;
};

class CompNe : public Instruction {
public:
    CompNe(BasicBlock& block, std::string lhs) : Instruction(block), lhs(lhs) {}
    void generateAsm(std::ostream& o) const override;

private:
    std::string lhs;
};

class Return : public Instruction {
public:
    Return(BasicBlock& block) : Instruction(block) {}
    void generateAsm(std::ostream& o) const override;
};
}
