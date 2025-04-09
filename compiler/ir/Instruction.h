#pragma once

#include <vector>
#include <string>
#include <iostream>

namespace IR {

class BasicBlock;
struct Variable;

class Instruction {

public:
    //  Actual code generation
    virtual void generateAsm(std::ostream& o) const = 0;

    std::string varToAsm(const Variable& variable) const;

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
    LdLoc(BasicBlock& block, const Variable& loc) : Instruction(block), loc(loc) {}
    void generateAsm(std::ostream& o) const override;

private:
    const Variable& loc;
};

class Store : public Instruction {
public:
    Store(BasicBlock& block, const Variable& loc) : Instruction(block), loc(loc) {}
    void generateAsm(std::ostream& o) const override;

private:
    const Variable& loc;
};

class Add : public Instruction {
public:
    Add(BasicBlock& block, const Variable& lhs) : Instruction(block), lhs(lhs) {}
    void generateAsm(std::ostream& o) const override;

private:
    const Variable& lhs;
};

class Sub : public Instruction {
public:
    Sub(BasicBlock& block, const Variable& lhs) : Instruction(block), lhs(lhs) {}
    void generateAsm(std::ostream& o) const override;

private:
    const Variable& lhs;
};


class Mul : public Instruction {
public:
    Mul(BasicBlock& block, const Variable& lhs) : Instruction(block), lhs(lhs) {}
    void generateAsm(std::ostream& o) const override;

private:
    const Variable& lhs;
};

class Div : public Instruction {
public:
    Div(BasicBlock& block, const Variable& lhs) : Instruction(block), lhs(lhs) {}
    void generateAsm(std::ostream& o) const override;

private:
    const Variable& lhs;
};

class Mod : public Instruction {
public:
    Mod(BasicBlock& block, const Variable& lhs) : Instruction(block), lhs(lhs) {}
    void generateAsm(std::ostream& o) const override;

private:
    const Variable& lhs;
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
    BitAnd(BasicBlock& block, const Variable& lhs) : Instruction(block), lhs(lhs) {}
    void generateAsm(std::ostream& o) const override;

private:
    const Variable& lhs;
};

class BitXor : public Instruction {
public:
    BitXor(BasicBlock& block, const Variable& lhs) : Instruction(block), lhs(lhs) {}
    void generateAsm(std::ostream& o) const override;

private:
    const Variable& lhs;
};

class BitOr : public Instruction {
public:
    BitOr(BasicBlock& block, const Variable& lhs) : Instruction(block), lhs(lhs) {}
    void generateAsm(std::ostream& o) const override;

private:
    const Variable& lhs;
};

class CompGt : public Instruction {
public:
    CompGt(BasicBlock& block, const Variable& lhs) : Instruction(block), lhs(lhs) {}
    void generateAsm(std::ostream& o) const override;

private:
    const Variable& lhs;
};

class CompLt : public Instruction {
public:
    CompLt(BasicBlock& block, const Variable& lhs) : Instruction(block), lhs(lhs) {}
    void generateAsm(std::ostream& o) const override;

private:
    const Variable& lhs;
};

class CompGtEq : public Instruction {
public:
    CompGtEq(BasicBlock& block, const Variable& lhs) : Instruction(block), lhs(lhs) {}
    void generateAsm(std::ostream& o) const override;

private:
    const Variable& lhs;
};

class CompLtEq : public Instruction {
public:
    CompLtEq(BasicBlock& block, const Variable& lhs) : Instruction(block), lhs(lhs) {}
    void generateAsm(std::ostream& o) const override;

private:
    const Variable& lhs;
};

class CompEq : public Instruction {
public:
    CompEq(BasicBlock& block, const Variable& lhs) : Instruction(block), lhs(lhs) {}
    void generateAsm(std::ostream& o) const override;

private:
    const Variable& lhs;
};

class CompNe : public Instruction {
public:
    CompNe(BasicBlock& block, const Variable& lhs) : Instruction(block), lhs(lhs) {}
    void generateAsm(std::ostream& o) const override;

private:
    const Variable& lhs;
};

class CastBool : public Instruction {
public:
    CastBool(BasicBlock& block) : Instruction(block) {}
    void generateAsm(std::ostream& o) const override;
};

class BrTrue : public Instruction {
public:
    BrTrue(BasicBlock& block, BasicBlock& target) : Instruction(block), target(target) {}
    void generateAsm(std::ostream& o) const override;

private:
    BasicBlock& target;
};

class Br : public Instruction {
public:
    Br(BasicBlock& block, BasicBlock& target) : Instruction(block), target(target) {}
    void generateAsm(std::ostream& o) const override;
private:
    BasicBlock& target;
};

class Return : public Instruction {
public:
    Return(BasicBlock& block) : Instruction(block) {}
    void generateAsm(std::ostream& o) const override;
};
}
