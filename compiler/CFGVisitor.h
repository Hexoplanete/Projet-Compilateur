#pragma once

#include "antlr4-runtime.h"
#include "generated/ifccBaseVisitor.h"
#include "ir/ControlFlowGraph.h"
class  CFGVisitor : public ifccBaseVisitor {
public:
    CFGVisitor();
    ~CFGVisitor();

    virtual antlrcpp::Any visitProg(ifccParser::ProgContext* ctx) override;
    virtual antlrcpp::Any visitFunction_def(ifccParser::Function_defContext* ctx) override;
    virtual antlrcpp::Any visitStmt_if(ifccParser::Stmt_ifContext* ctx) override;
    virtual antlrcpp::Any visitStmt_else(ifccParser::Stmt_elseContext* ctx) override;
    virtual antlrcpp::Any visitStmt_while(ifccParser::Stmt_whileContext* ctx) override;

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
    virtual antlrcpp::Any visitExpr_arithmetic_lazy_and(ifccParser::Expr_arithmetic_lazy_andContext* ctx) override;
    virtual antlrcpp::Any visitExpr_arithmetic_lazy_or(ifccParser::Expr_arithmetic_lazy_orContext* ctx) override;
    virtual antlrcpp::Any visitExpr_arithmetic_aff_add(ifccParser::Expr_arithmetic_aff_addContext* ctx) override;
    virtual antlrcpp::Any visitExpr_post_incr(ifccParser::Expr_post_incrContext* ctx) override;
    virtual antlrcpp::Any visitExpr_pre_incr(ifccParser::Expr_pre_incrContext* ctx) override;
    virtual antlrcpp::Any visitExpr_fct_call(ifccParser::Expr_fct_callContext* ctx) override;

    virtual antlrcpp::Any visitExpr_ident(ifccParser::Expr_identContext* ctx) override;
    virtual antlrcpp::Any visitExpr_const(ifccParser::Expr_constContext* ctx) override;
    
    virtual antlrcpp::Any visitStmt_block(ifccParser::Stmt_blockContext *ctx) override;

    virtual antlrcpp::Any visitStmt_jump_return(ifccParser::Stmt_jump_returnContext *ctx) override;

    const IR::Variable& visitAndStoreExpr(ifccParser::ExpressionContext* ctx);

    inline IR::ControlFlowGraph& getCFG() { return _cfg; }
private:
    IR::ControlFlowGraph _cfg;
    IR::BasicBlock* _returnBlock;
    std::map<std::string, std::string> _mapFuncNameToSignature;
};
