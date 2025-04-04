#pragma once

#include "antlr4-runtime.h"
#include "generated/ifccBaseVisitor.h"
#include "ir/ControlFlowGraph.h"
class  CFGVisitor : public ifccBaseVisitor {
public:
    CFGVisitor();
    ~CFGVisitor();

    virtual antlrcpp::Any visitProg(ifccParser::ProgContext* ctx) override;
    virtual antlrcpp::Any visitDeclaration(ifccParser::DeclarationContext* ctx) override;
    virtual antlrcpp::Any visitExpr_assign(ifccParser::Expr_assignContext* ctx) override;

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
    
    // virtual antlrcpp::Any visitStmt_block(ifccParser::Stmt_blockContext *ctx) override;

    virtual antlrcpp::Any visitStmt_jump_return(ifccParser::Stmt_jump_returnContext *ctx) override;

    std::string visitAndStoreExpr(ifccParser::ExpressionContext* ctx);

    inline IR::ControlFlowGraph& getCFG() { return cfg; }
private:
    IR::ControlFlowGraph cfg;
};
