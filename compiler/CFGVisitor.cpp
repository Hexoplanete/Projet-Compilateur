#include "CFGVisitor.h"
#include "ir/Instruction.h"
#include <memory>

CFGVisitor::CFGVisitor(){}

CFGVisitor::~CFGVisitor(){}

antlrcpp::Any CFGVisitor::visitProg(ifccParser::ProgContext* ctx) {
    cfg.createBlock("name");
    return antlrcpp::Any();
}

antlrcpp::Any CFGVisitor::visitDeclaration(ifccParser::DeclarationContext* ctx) {
    std::string varname = ctx->IDENTIFIER()->getText();
    cfg.addToSymbolTable(varname);
    
    if (ctx->expression()) {
        std::string tmpExpression = visit(ctx->expression());
        cfg.getCurrentBlock().addInstruction<IR::Copy>(varname, tmpExpression);
    }
    return antlrcpp::Any();
}

antlrcpp::Any CFGVisitor::visitExpr_assign(ifccParser::Expr_assignContext* ctx) {
    std::string varname = ctx->IDENTIFIER()->getText();    
    std::string tmpExpr = visit(ctx->expression());
    cfg.getCurrentBlock().addInstruction<IR::Copy>(varname, tmpExpr);
    return varname;
}

antlrcpp::Any CFGVisitor::visitExpr_arithmetic_add(ifccParser::Expr_arithmetic_addContext* ctx) {
    std::string tmpLHS = visit(ctx->expression(0));
    std::string tmpRHS = visit(ctx->expression(1));
    std::string tmpDest = cfg.createTmpvar();
    cfg.getCurrentBlock().addInstruction<IR::Add>(tmpDest, tmpLHS, tmpRHS);
    return tmpDest;
}
antlrcpp::Any CFGVisitor::visitExpr_arithmetic_mult(ifccParser::Expr_arithmetic_multContext* ctx) {
    std::string tmpLHS = visit(ctx->expression(0));
    std::string tmpRHS = visit(ctx->expression(1));
    std::string tmpDest = cfg.createTmpvar();
    cfg.getCurrentBlock().addInstruction<IR::Mul>(tmpDest, tmpLHS, tmpRHS);
    return tmpDest;
}
antlrcpp::Any CFGVisitor::visitExpr_arithmetic_add_unary(ifccParser::Expr_arithmetic_add_unaryContext* ctx) {
    std::string tmpExpr = visit(ctx->expression());
    if (ctx->OP_ADD()->getText() == "-") {
        std::string tmpDest = cfg.createTmpvar();
        cfg.getCurrentBlock().addInstruction<IR::Negate>(tmpDest, tmpExpr);
        return tmpDest;
    }
    return tmpExpr;
}

antlrcpp::Any CFGVisitor::visitExpr_ident(ifccParser::Expr_identContext* ctx) {
    return ctx->IDENTIFIER()->getText();
}
antlrcpp::Any CFGVisitor::visitExpr_const(ifccParser::Expr_constContext* ctx) {
    std::string tmpConst = cfg.createTmpvar();
    int constValue = std::stoi(ctx->CONST()->getText());
    cfg.getCurrentBlock().addInstruction<IR::LdConst>(tmpConst, constValue);
    return tmpConst;
}

antlrcpp::Any CFGVisitor::visitStmt_jump_return(ifccParser::Stmt_jump_returnContext* ctx) {
    std::string tmpExpr = visit(ctx->expression());
    cfg.getCurrentBlock().addInstruction<IR::Return>(tmpExpr);
    return antlrcpp::Any();
}