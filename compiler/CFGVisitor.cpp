#include "CFGVisitor.h"
#include "ir/Instruction.h"
#include <memory>
#include <vector>

CFGVisitor::CFGVisitor() { _mapFuncNameToSignature.clear(); }

CFGVisitor::~CFGVisitor() {}

antlrcpp::Any CFGVisitor::visitProg(ifccParser::ProgContext* ctx) {
    visitChildren(ctx);
    return 0;
}

antlrcpp::Any CFGVisitor::visitFunction_def(ifccParser::Function_defContext* ctx) {
    std::string name(ctx->IDENTIFIER()[0]->getText());
    std::string signature(name);

    cfg.pushContext();

    IR::GenFunc* genFuncI;
    if (!name.compare("main")) {
        cfg.createBlock(name);
        genFuncI = &cfg.getCurrentBlock().addInstruction<IR::GenFunc>(name);
    }
    else {
        // Liste des types
        auto types = ctx->TYPE();
        // Liste des identifiants (paramètres)
        auto preIdentifiers = ctx->IDENTIFIER();
        auto identifiers = std::vector<antlr4::tree::TerminalNode *>(preIdentifiers.begin() + 1, preIdentifiers.end());

        std::vector<IR::Variable> varList;
        varList.clear();
        if (!types.empty() && !identifiers.empty()) {
            int i;
            for (i = 0; i < identifiers.size() && i < 6; ++i)
                varList.push_back(cfg.createSymbolVar(identifiers[i]->getText()));
            int addressPos = 8;
            for (; i <identifiers.size(); ++i) {
                addressPos += 8;
                varList.push_back(cfg.createSymbolVar(identifiers[i]->getText(), addressPos));
            }
        }
        signature += "(";
        if (!varList.empty())
            signature += "int";
        for (int i = 1; i < varList.size(); ++i)
            signature += ", int";
        signature += ")";

        cfg.createBlock(signature);
        genFuncI = &cfg.getCurrentBlock().addInstruction<IR::GenFunc>(name, varList);
    }

    // Visiter le corps de la fonction
    for (auto stmt : ctx->stmt()) {
        visitStmt(stmt);  // Appelle le visiteur sur chaque statement
    }
    cfg.popContext();
    genFuncI->stackSize = cfg.resetMemoryCount();
    return 0;
}

antlrcpp::Any CFGVisitor::visitStmt_if(ifccParser::Stmt_ifContext* ctx) {
    IR::BasicBlock& blockCurr = cfg.getCurrentBlock();
    visit(ctx->expression());

    IR::BasicBlock& blockThen = cfg.createBlock();
    blockCurr.setExitTrue(blockThen);
    visit(ctx->stmt_then());
    IR::BasicBlock& blockThenEnd = cfg.getCurrentBlock();

    IR::BasicBlock& blockElse = cfg.createBlock();
    blockCurr.setExitFalse(blockElse);
    if (ctx->stmt_else()) {
        visit(ctx->stmt_else());
    }
    IR::BasicBlock& blockElseEnd = cfg.getCurrentBlock();
    
    IR::BasicBlock& blockEndIf = cfg.createBlock();
    blockThenEnd.setExit(blockEndIf);
    blockElseEnd.setExit(blockEndIf);

    return 0;
}

antlrcpp::Any CFGVisitor::visitStmt_then(ifccParser::Stmt_thenContext* ctx) {
    if (ctx->stmt_block()) {
        visit(ctx->stmt_block());
    }
    else if (ctx->stmt_expression()) {
        visit(ctx->stmt_expression());
    }

    return 0;
}

antlrcpp::Any CFGVisitor::visitStmt_else(ifccParser::Stmt_elseContext* ctx) {
    if (ctx->stmt_block()) {
        visit(ctx->stmt_block());
    }
    else if (ctx->stmt_expression()) {
        visit(ctx->stmt_expression());
    }
    else if (ctx->stmt_if()) {
        visit(ctx->stmt_expression());
    }

    return 0;
}

antlrcpp::Any CFGVisitor::visitStmt_while(ifccParser::Stmt_whileContext* ctx) {

    IR::BasicBlock& blockConditionWhile = cfg.createBlock();
    visit(ctx->expression());

    IR::BasicBlock& blockBody = cfg.createBlock();
    blockConditionWhile.setExitTrue(blockBody);
    visit(ctx->stmt_block());
    IR::BasicBlock& blockBodyEnd = cfg.getCurrentBlock();
    blockBodyEnd.setExit(blockConditionWhile);

    IR::BasicBlock& blockEndWhile = cfg.createBlock();
    blockConditionWhile.setExitFalse(blockEndWhile);
    
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
    }
    else if (ctx->OP_ADD()->getText() == "-") {
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

antlrcpp::Any CFGVisitor::visitExpr_fct_call(ifccParser::Expr_fct_callContext* ctx) {
    // Liste des identifiants (paramètres)
    auto expressions = ctx->expression();
    std::vector<std::string> registre = {"%edi", "%esi", "%edx", "%ecx", "%r8d", "%r9d"};
    std::vector<IR::Variable> varVector;
    varVector.clear();
    if (!expressions.empty()) {
        for (int j = 0; j < expressions.size(); ++j){
            varVector.push_back(visitAndStoreExpr(expressions[j]));
        }
        for (int i = 1; i < expressions.size() && i < 6; ++i) {
            cfg.getCurrentBlock().addInstruction<IR::MovToReg>(varVector[i], registre[i]);
        }
        for (int i = expressions.size() - 1; i > 5; --i) {
            cfg.getCurrentBlock().addInstruction<IR::MovToReg>(varVector[i], "%edi");
            cfg.getCurrentBlock().addInstruction<IR::PushQ>();
        }
        cfg.getCurrentBlock().addInstruction<IR::MovToReg>(varVector[0], registre[0]);
    }
    
    cfg.getCurrentBlock().addInstruction<IR::CallFunc>(ctx->IDENTIFIER()->getText(), varVector); // CHANGERa

    return 0;
}

antlrcpp::Any CFGVisitor::visitExpr_arithmetic_lazy_and(ifccParser::Expr_arithmetic_lazy_andContext* ctx) {
    visit(ctx->expression(0));  // On fait que visiter, on n'a pas besoin de stocker le résultat
    cfg.getCurrentBlock().addInstruction<IR::CastBool>();

    // Createblock change le bloc actuel donc on a besoin de sauvegarder le bloc précédent
    IR::BasicBlock& leftBlock = cfg.getCurrentBlock();

    // On change de bloc, on crée un nouveau bloc
    // On veut aller dans ce bloc là si le précédent est vrai
    IR::BasicBlock& rightBlock = cfg.createBlock();

    // Si leftBlock donne vrai, on va sur le rightBlock
    leftBlock.setExitTrue(rightBlock);

    // Expression vient du contexte, de la syntaxe antlr 
    // Va le stocker par defaut dans reg
    visit(ctx->expression(1));
    cfg.getCurrentBlock().addInstruction<IR::CastBool>();

    IR::BasicBlock& afterBlock = cfg.createBlock();
    leftBlock.setExitFalse(afterBlock);

    // Peu importe comment est évalué la partie droite, son résultat détermine le résultat de left && right
    rightBlock.setExit(afterBlock);

    return 0;
}

antlrcpp::Any CFGVisitor::visitExpr_arithmetic_lazy_or(ifccParser::Expr_arithmetic_lazy_orContext* ctx) {
    visit(ctx->expression(0));  // On fait que visiter, on n'a pas besoin de stocker le résultat
    cfg.getCurrentBlock().addInstruction<IR::CastBool>();

    // Createblock change le bloc actuel donc on a besoin de sauvegarder le bloc précédent
    IR::BasicBlock& leftBlock = cfg.getCurrentBlock();

    // On change de bloc, on crée un nouveau bloc
    // On veut aller dans ce bloc là si le précédent est vrai
    IR::BasicBlock& rightBlock = cfg.createBlock();

    // Si leftBlock donne faux, on va sur le rightBlock
    leftBlock.setExitFalse(rightBlock);

    // Expression vient du contexte, de la syntaxe antlr 
    // Va le stocker par defaut dans reg
    visit(ctx->expression(1));
    cfg.getCurrentBlock().addInstruction<IR::CastBool>();

    IR::BasicBlock& afterBlock = cfg.createBlock();
    leftBlock.setExitTrue(afterBlock);

    // Peu importe comment est évalué la partie droite, son résultat détermine le résultat de left || right
    rightBlock.setExit(afterBlock);

    return 0;
}


antlrcpp::Any CFGVisitor::visitExpr_ident(ifccParser::Expr_identContext* ctx) {
    std::string varname = ctx->IDENTIFIER()->getText();
    const auto& variable = cfg.getSymbolVar(varname);
    cfg.getCurrentBlock().addInstruction<IR::LdLoc>(variable);
    return 0;
}

antlrcpp::Any CFGVisitor::visitExpr_const(ifccParser::Expr_constContext* ctx) {
    int value;
    if (ctx->CONST_INT()) value = std::stoi(ctx->CONST_INT()->getText());
    else value = (int)ctx->CONST_CHAR()->getText()[1];
    cfg.getCurrentBlock().addInstruction<IR::LdConst>(value);
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