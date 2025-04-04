#pragma once


#include "antlr4-runtime.h"
#include "generated/ifccBaseVisitor.h"

class  CodeGenVisitor : public ifccBaseVisitor {
public:
    CodeGenVisitor(std::map<std::string, std::map<std::string, int>>& symbols, std::map<std::string, std::string>& blockParent);
    ~CodeGenVisitor();

    virtual antlrcpp::Any visitProg(ifccParser::ProgContext* ctx) override;
    virtual antlrcpp::Any visitMain(ifccParser::MainContext* ctx) override;
    virtual antlrcpp::Any visitDeclaration(ifccParser::DeclarationContext* ctx) override;
    virtual antlrcpp::Any visitExpr_assign(ifccParser::Expr_assignContext* ctx) override;

    // virtual antlrcpp::Any visitExpr_putchar(ifccParser::Expr_putcharContext* ctx) override;
    // virtual antlrcpp::Any visitExpr_getchar(ifccParser::Expr_getcharContext* ctx) override;

    virtual antlrcpp::Any visitExpr_arithmetic_add(ifccParser::Expr_arithmetic_addContext* ctx) override;
    virtual antlrcpp::Any visitExpr_arithmetic_mult(ifccParser::Expr_arithmetic_multContext* ctx) override;
    virtual antlrcpp::Any visitExpr_arithmetic_unary(ifccParser::Expr_arithmetic_unaryContext* ctx) override;
    virtual antlrcpp::Any visitExpr_arithmetic_bit_and(ifccParser::Expr_arithmetic_bit_andContext* ctx) override;
    virtual antlrcpp::Any visitExpr_arithmetic_bit_xor(ifccParser::Expr_arithmetic_bit_xorContext* ctx) override;
    virtual antlrcpp::Any visitExpr_arithmetic_bit_or(ifccParser::Expr_arithmetic_bit_orContext* ctx) override;
    virtual antlrcpp::Any visitExpr_compare(ifccParser::Expr_compareContext* ctx) override;
    virtual antlrcpp::Any visitExpr_equal(ifccParser::Expr_equalContext* ctx) override;
    
    virtual antlrcpp::Any visitExpr_ident(ifccParser::Expr_identContext* ctx) override;
    virtual antlrcpp::Any visitExpr_const(ifccParser::Expr_constContext* ctx) override;
    
    virtual antlrcpp::Any visitStmt_jump_return(ifccParser::Stmt_jump_returnContext *ctx) override;

    virtual antlrcpp::Any visitStmt_block(ifccParser::Stmt_blockContext *ctx) override;

private:
    int getAddress(std::string varName);

    // The symbol map, for now, associates variable names to the address of the corresponding variable in memory.
    std::map<std::string, std::map<std::string, int>>& _symbolMap;
    std::map<std::string, std::string>& _blockParentMap;
    // The _tmpCount variable enables to create numbered temporary variables (ex : '@temp12') in a logical order with no repetition
    unsigned int _tmpCount;
    unsigned int _blockCount;
    std::string _currentBlock = ""; // The name of the current block the visitor is in
};
