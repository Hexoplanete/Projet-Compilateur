#include "CFGVisitor.h"
#include "ir/Instruction.h"
#include <memory>

CFGVisitor::CFGVisitor() {}

CFGVisitor::~CFGVisitor() {}

antlrcpp::Any CFGVisitor::visitMain(ifccParser::MainContext* ctx) {
    cfg.createBlock("main");

    cfg.pushContext(); // TODO update when we do functions
    ifccBaseVisitor::visitMain(ctx);
    cfg.popContext();
    return 0;
}

antlrcpp::Any CFGVisitor::visitDeclaration(ifccParser::DeclarationContext* ctx) {
    std::string varname = ctx->IDENTIFIER()->getText();
    const auto& variable = cfg.createSymbolVar(varname);
    
    if (ctx->expression()) {
        visit(ctx->expression());
        cfg.getCurrentBlock().addInstruction<IR::Store>(variable);
    }
    return 0;
}

antlrcpp::Any CFGVisitor::visitExpr_assign(ifccParser::Expr_assignContext* ctx) {
    std::string varname = ctx->IDENTIFIER()->getText();
    const auto& variable = cfg.getSymbolVar(varname);

    visit(ctx->expression());
    cfg.getCurrentBlock().addInstruction<IR::Store>(variable);
    return 0;
}

antlrcpp::Any CFGVisitor::visitExpr_arithmetic_add(ifccParser::Expr_arithmetic_addContext* ctx) {
    const auto& lhs = visitAndStoreExpr(ctx->expression(0));
    visit(ctx->expression(1));

    if (ctx->OP_ADD()->getText() == "+") cfg.getCurrentBlock().addInstruction<IR::Add>(lhs);
    else cfg.getCurrentBlock().addInstruction<IR::Sub>(lhs);
    return 0;
}
antlrcpp::Any CFGVisitor::visitExpr_arithmetic_mult(ifccParser::Expr_arithmetic_multContext* ctx) {
    const auto& lhs = visitAndStoreExpr(ctx->expression(0));
    visit(ctx->expression(1));

    auto op = ctx->OP_MULT()->getText();
    if (op == "*") cfg.getCurrentBlock().addInstruction<IR::Mul>(lhs);
    else if (op == "/") cfg.getCurrentBlock().addInstruction<IR::Div>(lhs);
    else cfg.getCurrentBlock().addInstruction<IR::Mod>(lhs);
    return 0;
}
antlrcpp::Any CFGVisitor::visitExpr_arithmetic_unary(ifccParser::Expr_arithmetic_unaryContext* ctx) {
    visit(ctx->expression());
    if (ctx->OP_NOT()) {
        cfg.getCurrentBlock().addInstruction<IR::LogicalNot>();
    } else if (ctx->OP_ADD()->getText() == "-") {
        cfg.getCurrentBlock().addInstruction<IR::Negate>();
    }
    return 0;
}

antlrcpp::Any CFGVisitor::visitExpr_arithmetic_bit_and(ifccParser::Expr_arithmetic_bit_andContext* ctx) {
    const auto& lhs = visitAndStoreExpr(ctx->expression(0));
    visit(ctx->expression(1));

    cfg.getCurrentBlock().addInstruction<IR::BitAnd>(lhs);
    return 0;
}

antlrcpp::Any CFGVisitor::visitExpr_arithmetic_bit_xor(ifccParser::Expr_arithmetic_bit_xorContext* ctx) {
    const auto& lhs = visitAndStoreExpr(ctx->expression(0));
    visit(ctx->expression(1));

    cfg.getCurrentBlock().addInstruction<IR::BitXor>(lhs);
    return 0;
}

antlrcpp::Any CFGVisitor::visitExpr_arithmetic_bit_or(ifccParser::Expr_arithmetic_bit_orContext* ctx) {
    const auto& lhs = visitAndStoreExpr(ctx->expression(0));
    visit(ctx->expression(1));

    cfg.getCurrentBlock().addInstruction<IR::BitOr>(lhs);
    return 0;
}

antlrcpp::Any CFGVisitor::visitExpr_compare(ifccParser::Expr_compareContext* ctx) {
    const auto& lhs = visitAndStoreExpr(ctx->expression(0));
    visit(ctx->expression(1));

    std::string op = ctx->OP_COMP()->getText();
    if (op == ">") cfg.getCurrentBlock().addInstruction<IR::CompGt>(lhs);
    else if (op == "<") cfg.getCurrentBlock().addInstruction<IR::CompLt>(lhs);
    else if (op == ">=") cfg.getCurrentBlock().addInstruction<IR::CompGtEq>(lhs);
    else cfg.getCurrentBlock().addInstruction<IR::CompLtEq>(lhs);
    return 0;
}

antlrcpp::Any CFGVisitor::visitExpr_equal(ifccParser::Expr_equalContext* ctx) {
    const auto& lhs = visitAndStoreExpr(ctx->expression(0));
    visit(ctx->expression(1));
    
    std::string op = ctx->OP_EQ()->getText();
    if (op == "==") cfg.getCurrentBlock().addInstruction<IR::CompEq>(lhs);
    else cfg.getCurrentBlock().addInstruction<IR::CompNe>(lhs);
    return 0;
}

antlrcpp::Any CFGVisitor::visitExpr_arithmetic_aff_add(ifccParser::Expr_arithmetic_aff_addContext* ctx) {
    // a = a + ?
    std::string varname = ctx->IDENTIFIER()->getText();
    const auto& variable = cfg.getSymbolVar(varname);
    // compute variable = a + ?
    visit(ctx->expression());

    if (ctx->OP_AFF_ADD()->getText() == "+=") {
        cfg.getCurrentBlock().addInstruction<IR::Add>(variable);
    }
    else {
        cfg.getCurrentBlock().addInstruction<IR::Sub>(variable);
    }
    // do a = variable
    cfg.getCurrentBlock().addInstruction<IR::Store>(variable);
    return 0;
}

antlrcpp::Any CFGVisitor::visitExpr_post_incr(ifccParser::Expr_post_incrContext* ctx) {
    // b++ : we save the value of b in a tmp var that is put in eax at the end
    std::string varname = ctx->IDENTIFIER()->getText();
    const auto& variable = cfg.getSymbolVar(varname);
    const auto& variableTmp = cfg.createTmpVar();
    cfg.getCurrentBlock().addInstruction<IR::LdLoc>(variable);
    cfg.getCurrentBlock().addInstruction<IR::Store>(variableTmp);

    cfg.getCurrentBlock().addInstruction<IR::LdConst>(1);

    if (ctx->OP_INCR()->getText() == "++") {
        cfg.getCurrentBlock().addInstruction<IR::Add>(variable);
    }
    else {
        cfg.getCurrentBlock().addInstruction<IR::Sub>(variable);
    }
    cfg.getCurrentBlock().addInstruction<IR::Store>(variable);
    cfg.getCurrentBlock().addInstruction<IR::LdLoc>(variableTmp);
    return 0;
}

antlrcpp::Any CFGVisitor::visitExpr_pre_incr(ifccParser::Expr_pre_incrContext* ctx) {
    // ++b : we simply add 1 to b
    std::string varname = ctx->IDENTIFIER()->getText();
    const auto& variable = cfg.getSymbolVar(varname);

    cfg.getCurrentBlock().addInstruction<IR::LdConst>(1);

    if (ctx->OP_INCR()->getText() == "++") {
        cfg.getCurrentBlock().addInstruction<IR::Add>(variable);
    }
    else {
        cfg.getCurrentBlock().addInstruction<IR::Sub>(variable);
    }
    cfg.getCurrentBlock().addInstruction<IR::Store>(variable);
    return 0;
}

antlrcpp::Any CFGVisitor::visitExpr_ident(ifccParser::Expr_identContext* ctx) {
    std::string varname = ctx->IDENTIFIER()->getText();
    const auto& variable = cfg.getSymbolVar(varname);
    cfg.getCurrentBlock().addInstruction<IR::LdLoc>(variable);
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

const IR::Variable& CFGVisitor::visitAndStoreExpr(ifccParser::ExpressionContext* ctx)
{
    visit(ctx);
    const IR::Variable& lhs = cfg.createTmpVar();
    cfg.getCurrentBlock().addInstruction<IR::Store>(lhs);
    return lhs;
}

antlrcpp::Any CFGVisitor::visitStmt_block(ifccParser::Stmt_blockContext* ctx)
{
    cfg.pushContext();
    ifccBaseVisitor::visitStmt_block(ctx);
    cfg.popContext();
    return 0;
}