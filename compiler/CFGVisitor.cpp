#include "CFGVisitor.h"
#include "ir/Instruction.h"
#include <memory>

CFGVisitor::CFGVisitor() {}

CFGVisitor::~CFGVisitor() {}

antlrcpp::Any CFGVisitor::visitProg(ifccParser::ProgContext* ctx) {
    cfg.createBlock("main");
    ifccBaseVisitor::visitProg(ctx);
    return 0;
}

antlrcpp::Any CFGVisitor::visitDeclaration(ifccParser::DeclarationContext* ctx) {
    std::string varname = ctx->IDENTIFIER()->getText();
    cfg.createSymbolVar(varname);

    if (ctx->expression()) {
        visit(ctx->expression());
        cfg.getCurrentBlock().addInstruction<IR::Store>(varname);
    }
    return 0;
}

antlrcpp::Any CFGVisitor::visitExpr_assign(ifccParser::Expr_assignContext* ctx) {
    std::string varname = ctx->IDENTIFIER()->getText();
    visit(ctx->expression());
    cfg.getCurrentBlock().addInstruction<IR::Store>(varname);
    return 0;
}

antlrcpp::Any CFGVisitor::visitExpr_arithmetic_add(ifccParser::Expr_arithmetic_addContext* ctx) {
    visit(ctx->expression(0));
    std::string lhs = cfg.createTmpVar();
    cfg.getCurrentBlock().addInstruction<IR::Store>(lhs);
    visit(ctx->expression(1));

    if (ctx->OP_ADD()->getText() == "+") cfg.getCurrentBlock().addInstruction<IR::Add>(lhs);
    else cfg.getCurrentBlock().addInstruction<IR::Sub>(lhs);
    return 0;
}
antlrcpp::Any CFGVisitor::visitExpr_arithmetic_mult(ifccParser::Expr_arithmetic_multContext* ctx) {
    visit(ctx->expression(0));
    std::string lhs = cfg.createTmpVar();
    cfg.getCurrentBlock().addInstruction<IR::Store>(lhs);
    visit(ctx->expression(1));

    auto op = ctx->OP_MULT()->getText();
    if (op == "*") cfg.getCurrentBlock().addInstruction<IR::Mul>(lhs);
    else if (op == "/") cfg.getCurrentBlock().addInstruction<IR::Div>(lhs);
    else cfg.getCurrentBlock().addInstruction<IR::Mod>(lhs);
    return 0;
}
antlrcpp::Any CFGVisitor::visitExpr_arithmetic_add_unary(ifccParser::Expr_arithmetic_add_unaryContext* ctx) {
    visit(ctx->expression());
    if (ctx->OP_ADD()->getText() == "-") {
        cfg.getCurrentBlock().addInstruction<IR::Negate>();
    }
    return 0;
}

antlrcpp::Any CFGVisitor::visitExpr_ident(ifccParser::Expr_identContext* ctx) {
    std::string varname = ctx->IDENTIFIER()->getText();
    cfg.getCurrentBlock().addInstruction<IR::LdLoc>(varname);
    return 0;
}
antlrcpp::Any CFGVisitor::visitExpr_const(ifccParser::Expr_constContext* ctx) {
    int constValue = std::stoi(ctx->CONST()->getText());
    cfg.getCurrentBlock().addInstruction<IR::LdConst>(constValue);
    return 0;
}

antlrcpp::Any CFGVisitor::visitStmt_jump_return(ifccParser::Stmt_jump_returnContext* ctx) {
    visit(ctx->expression());
    cfg.getCurrentBlock().addInstruction<IR::Return>();
    return 0;
}