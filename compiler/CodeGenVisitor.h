#pragma once


#include "antlr4-runtime.h"
#include "generated/ifccBaseVisitor.h"

class  CodeGenVisitor : public ifccBaseVisitor {
public:
    CodeGenVisitor(std::map<std::string, int>& symbols);
    ~CodeGenVisitor();
    virtual antlrcpp::Any visitProg(ifccParser::ProgContext* ctx) override;
    virtual antlrcpp::Any visitDeclaration(ifccParser::DeclarationContext* ctx) override;
    virtual antlrcpp::Any visitExpr_assign(ifccParser::Expr_assignContext* ctx) override;

    virtual antlrcpp::Any visitExpr_arithmetic_add(ifccParser::Expr_arithmetic_addContext* ctx) override;
    virtual antlrcpp::Any visitExpr_arithmetic_mult(ifccParser::Expr_arithmetic_multContext* ctx) override;
    virtual antlrcpp::Any visitExpr_arithmetic_add_unary(ifccParser::Expr_arithmetic_add_unaryContext* ctx) override;
    
    virtual antlrcpp::Any visitExpr_ident(ifccParser::Expr_identContext* ctx) override;
    virtual antlrcpp::Any visitExpr_const(ifccParser::Expr_constContext* ctx) override;
    
    virtual antlrcpp::Any visitStmt_jump_return(ifccParser::Stmt_jump_returnContext *ctx) override;

private:
    std::map<std::string, int>& _symbolMap;
    unsigned int _tmpCount;
};
