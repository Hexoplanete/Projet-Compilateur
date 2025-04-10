#include "CFGVisitor.h"
#include "ir/Instruction.h"
#include <memory>
#include <vector>

CFGVisitor::CFGVisitor(): _cfg(), _returnBlock(nullptr), _mapFuncNameToSignature() {}

CFGVisitor::~CFGVisitor() {}

antlrcpp::Any CFGVisitor::visitProg(ifccParser::ProgContext* ctx) {
    visitChildren(ctx);
    return 0;
}

antlrcpp::Any CFGVisitor::visitFunction_def(ifccParser::Function_defContext* ctx) {
    std::string name(ctx->IDENTIFIER()[0]->getText());
    std::string signature(name);

    _cfg.pushContext();

    IR::GenFunc* genFuncI;
    if (!name.compare("main")) {
        _cfg.createAndAddBlock(name);
        genFuncI = &_cfg.getCurrentBlock().addInstruction<IR::GenFunc>(name);
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
                varList.push_back(_cfg.createSymbolVar(identifiers[i]->getText()));
            int addressPos = 8;
            for (; i <identifiers.size(); ++i) {
                addressPos += 8;
                varList.push_back(_cfg.createSymbolVar(identifiers[i]->getText(), addressPos));
            }
        }
        signature += "(";
        if (!varList.empty())
            signature += "int";
        for (int i = 1; i < varList.size(); ++i)
            signature += ", int";
        signature += ")";

        _cfg.createAndAddBlock(signature);
        genFuncI = &_cfg.getCurrentBlock().addInstruction<IR::GenFunc>(name, varList);
    }
    auto retBlock = _cfg.createBlock();
    _returnBlock = retBlock.get();

    IR::Return retI = retBlock.get()->addInstruction<IR::Return>();
    
    // Visiter le corps de la fonction
    for (auto stmt : ctx->stmt()) {
        visitStmt(stmt);  // Appelle le visiteur sur chaque statement
    }

    // If the func is main, allow no return the be called and return 0
    // This instruction will only be executed if the path we follow has no return
    // If we encountered any return we would jump to the return block directly and skip this instruction
    if (!name.compare("main")) _cfg.getCurrentBlock().addInstruction<IR::LdConst>(0);

    _cfg.addBlock(std::move(retBlock));
    _cfg.popContext();
    _returnBlock = nullptr;
    genFuncI->stackSize = _cfg.resetMemoryCount();
    return 0;
}

antlrcpp::Any CFGVisitor::visitStmt_if(ifccParser::Stmt_ifContext* ctx) {
    IR::BasicBlock& blockCurr = _cfg.getCurrentBlock();
    visit(ctx->expression());

    IR::BasicBlock& blockThen = _cfg.createAndAddBlock();
    blockCurr.setExitTrue(blockThen);
    visit(ctx->stmt());
    IR::BasicBlock& blockThenEnd = _cfg.getCurrentBlock();

    IR::BasicBlock& blockElse = _cfg.createAndAddBlock();
    blockCurr.setExitFalse(blockElse);
    if (ctx->stmt_else()) {
        visit(ctx->stmt_else());
    }
    IR::BasicBlock& blockElseEnd = _cfg.getCurrentBlock();
    
    IR::BasicBlock& blockEndIf = _cfg.createAndAddBlock();
    blockThenEnd.setExit(blockEndIf);
    blockElseEnd.setExit(blockEndIf);

    return 0;
}

antlrcpp::Any CFGVisitor::visitStmt_else(ifccParser::Stmt_elseContext* ctx) {
    if (ctx->stmt()) {
        visit(ctx->stmt());
    }
    return 0;
}

antlrcpp::Any CFGVisitor::visitStmt_while(ifccParser::Stmt_whileContext* ctx) {

    IR::BasicBlock& blockConditionWhile = _cfg.createAndAddBlock();
    visit(ctx->expression());

    IR::BasicBlock& blockBody = _cfg.createAndAddBlock();
    blockConditionWhile.setExitTrue(blockBody);
    visit(ctx->stmt_block());
    IR::BasicBlock& blockBodyEnd = _cfg.getCurrentBlock();
    blockBodyEnd.setExit(blockConditionWhile);

    IR::BasicBlock& blockEndWhile = _cfg.createAndAddBlock();
    blockConditionWhile.setExitFalse(blockEndWhile);
    
    return 0;
}

antlrcpp::Any CFGVisitor::visitDeclaration(ifccParser::DeclarationContext* ctx) {
    std::string varname = ctx->IDENTIFIER()->getText();
    const auto& variable = _cfg.createSymbolVar(varname);

    if (ctx->expression()) {
        visit(ctx->expression());
        _cfg.getCurrentBlock().addInstruction<IR::Store>(variable);
    }
    return 0;
}

antlrcpp::Any CFGVisitor::visitExpr_assign(ifccParser::Expr_assignContext* ctx) {
    std::string varname = ctx->IDENTIFIER()->getText();
    const auto& variable = _cfg.getSymbolVar(varname);

    visit(ctx->expression());
    _cfg.getCurrentBlock().addInstruction<IR::Store>(variable);
    return 0;
}

antlrcpp::Any CFGVisitor::visitExpr_arithmetic_add(ifccParser::Expr_arithmetic_addContext* ctx) {
    const auto& lhs = visitAndStoreExpr(ctx->expression(0));
    visit(ctx->expression(1));

    if (ctx->OP_ADD()->getText() == "+") _cfg.getCurrentBlock().addInstruction<IR::Add>(lhs);
    else _cfg.getCurrentBlock().addInstruction<IR::Sub>(lhs);
    return 0;
}
antlrcpp::Any CFGVisitor::visitExpr_arithmetic_mult(ifccParser::Expr_arithmetic_multContext* ctx) {
    const auto& lhs = visitAndStoreExpr(ctx->expression(0));
    visit(ctx->expression(1));

    auto op = ctx->OP_MULT()->getText();
    if (op == "*") _cfg.getCurrentBlock().addInstruction<IR::Mul>(lhs);
    else if (op == "/") _cfg.getCurrentBlock().addInstruction<IR::Div>(lhs);
    else _cfg.getCurrentBlock().addInstruction<IR::Mod>(lhs);
    return 0;
}
antlrcpp::Any CFGVisitor::visitExpr_arithmetic_unary(ifccParser::Expr_arithmetic_unaryContext* ctx) {
    visit(ctx->expression());
    if (ctx->OP_NOT()) {
        _cfg.getCurrentBlock().addInstruction<IR::LogicalNot>();
    }
    else if (ctx->OP_ADD()->getText() == "-") {
        _cfg.getCurrentBlock().addInstruction<IR::Negate>();
    }
    return 0;
}

antlrcpp::Any CFGVisitor::visitExpr_arithmetic_bit_and(ifccParser::Expr_arithmetic_bit_andContext* ctx) {
    const auto& lhs = visitAndStoreExpr(ctx->expression(0));
    visit(ctx->expression(1));

    _cfg.getCurrentBlock().addInstruction<IR::BitAnd>(lhs);
    return 0;
}

antlrcpp::Any CFGVisitor::visitExpr_arithmetic_bit_xor(ifccParser::Expr_arithmetic_bit_xorContext* ctx) {
    const auto& lhs = visitAndStoreExpr(ctx->expression(0));
    visit(ctx->expression(1));

    _cfg.getCurrentBlock().addInstruction<IR::BitXor>(lhs);
    return 0;
}

antlrcpp::Any CFGVisitor::visitExpr_arithmetic_bit_or(ifccParser::Expr_arithmetic_bit_orContext* ctx) {
    const auto& lhs = visitAndStoreExpr(ctx->expression(0));
    visit(ctx->expression(1));

    _cfg.getCurrentBlock().addInstruction<IR::BitOr>(lhs);
    return 0;
}

antlrcpp::Any CFGVisitor::visitExpr_compare(ifccParser::Expr_compareContext* ctx) {
    const auto& lhs = visitAndStoreExpr(ctx->expression(0));
    visit(ctx->expression(1));

    std::string op = ctx->OP_COMP()->getText();
    if (op == ">") _cfg.getCurrentBlock().addInstruction<IR::CompGt>(lhs);
    else if (op == "<") _cfg.getCurrentBlock().addInstruction<IR::CompLt>(lhs);
    else if (op == ">=") _cfg.getCurrentBlock().addInstruction<IR::CompGtEq>(lhs);
    else _cfg.getCurrentBlock().addInstruction<IR::CompLtEq>(lhs);
    return 0;
}

antlrcpp::Any CFGVisitor::visitExpr_equal(ifccParser::Expr_equalContext* ctx) {
    const auto& lhs = visitAndStoreExpr(ctx->expression(0));
    visit(ctx->expression(1));

    std::string op = ctx->OP_EQ()->getText();
    if (op == "==") _cfg.getCurrentBlock().addInstruction<IR::CompEq>(lhs);
    else _cfg.getCurrentBlock().addInstruction<IR::CompNe>(lhs);
    return 0;
}

antlrcpp::Any CFGVisitor::visitExpr_arithmetic_aff_add(ifccParser::Expr_arithmetic_aff_addContext* ctx) {
    // a = a + ?
    std::string varname = ctx->IDENTIFIER()->getText();
    const auto& variable = _cfg.getSymbolVar(varname);
    // compute variable = a + ?
    visit(ctx->expression());

    if (ctx->OP_AFF_ADD()->getText() == "+=") {
        _cfg.getCurrentBlock().addInstruction<IR::Add>(variable);
    }
    else {
        _cfg.getCurrentBlock().addInstruction<IR::Sub>(variable);
    }
    // do a = variable
    _cfg.getCurrentBlock().addInstruction<IR::Store>(variable);
    return 0;
}

antlrcpp::Any CFGVisitor::visitExpr_post_incr(ifccParser::Expr_post_incrContext* ctx) {
    // b++ : we save the value of b in a tmp var that is put in eax at the end
    std::string varname = ctx->IDENTIFIER()->getText();
    const auto& variable = _cfg.getSymbolVar(varname);
    const auto& variableTmp = _cfg.createTmpVar();
    _cfg.getCurrentBlock().addInstruction<IR::LdLoc>(variable);
    _cfg.getCurrentBlock().addInstruction<IR::Store>(variableTmp);

    _cfg.getCurrentBlock().addInstruction<IR::LdConst>(1);

    if (ctx->OP_INCR()->getText() == "++") {
        _cfg.getCurrentBlock().addInstruction<IR::Add>(variable);
    }
    else {
        _cfg.getCurrentBlock().addInstruction<IR::Sub>(variable);
    }
    _cfg.getCurrentBlock().addInstruction<IR::Store>(variable);
    _cfg.getCurrentBlock().addInstruction<IR::LdLoc>(variableTmp);
    return 0;
}

antlrcpp::Any CFGVisitor::visitExpr_pre_incr(ifccParser::Expr_pre_incrContext* ctx) {
    // ++b : we simply add 1 to b
    std::string varname = ctx->IDENTIFIER()->getText();
    const auto& variable = _cfg.getSymbolVar(varname);

    _cfg.getCurrentBlock().addInstruction<IR::LdConst>(1);

    if (ctx->OP_INCR()->getText() == "++") {
        _cfg.getCurrentBlock().addInstruction<IR::Add>(variable);
    }
    else {
        _cfg.getCurrentBlock().addInstruction<IR::Sub>(variable);
    }
    _cfg.getCurrentBlock().addInstruction<IR::Store>(variable);
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
            _cfg.getCurrentBlock().addInstruction<IR::MovToReg>(varVector[i], registre[i]);
        }
        for (int i = expressions.size() - 1; i > 5; --i) {
            _cfg.getCurrentBlock().addInstruction<IR::MovToReg>(varVector[i], "%edi");
            _cfg.getCurrentBlock().addInstruction<IR::PushQ>();
        }
        _cfg.getCurrentBlock().addInstruction<IR::MovToReg>(varVector[0], registre[0]);
    }
    
    _cfg.getCurrentBlock().addInstruction<IR::CallFunc>(ctx->IDENTIFIER()->getText(), varVector); // CHANGERa

    return 0;
}

antlrcpp::Any CFGVisitor::visitExpr_arithmetic_lazy_and(ifccParser::Expr_arithmetic_lazy_andContext* ctx) {
    visit(ctx->expression(0));  // On fait que visiter, on n'a pas besoin de stocker le résultat
    _cfg.getCurrentBlock().addInstruction<IR::CastBool>();

    // Createblock change le bloc actuel donc on a besoin de sauvegarder le bloc précédent
    IR::BasicBlock& leftBlock = _cfg.getCurrentBlock();

    // On change de bloc, on crée un nouveau bloc
    // On veut aller dans ce bloc là si le précédent est vrai
    IR::BasicBlock& rightBlock = _cfg.createAndAddBlock();

    // Si leftBlock donne vrai, on va sur le rightBlock
    leftBlock.setExitTrue(rightBlock);

    // Expression vient du contexte, de la syntaxe antlr 
    // Va le stocker par defaut dans reg
    visit(ctx->expression(1));
    _cfg.getCurrentBlock().addInstruction<IR::CastBool>();

    IR::BasicBlock& afterBlock = _cfg.createAndAddBlock();
    leftBlock.setExitFalse(afterBlock);

    // Peu importe comment est évalué la partie droite, son résultat détermine le résultat de left && right
    rightBlock.setExit(afterBlock);

    return 0;
}

antlrcpp::Any CFGVisitor::visitExpr_arithmetic_lazy_or(ifccParser::Expr_arithmetic_lazy_orContext* ctx) {
    visit(ctx->expression(0));  // On fait que visiter, on n'a pas besoin de stocker le résultat
    _cfg.getCurrentBlock().addInstruction<IR::CastBool>();

    // Createblock change le bloc actuel donc on a besoin de sauvegarder le bloc précédent
    IR::BasicBlock& leftBlock = _cfg.getCurrentBlock();

    // On change de bloc, on crée un nouveau bloc
    // On veut aller dans ce bloc là si le précédent est vrai
    IR::BasicBlock& rightBlock = _cfg.createAndAddBlock();

    // Si leftBlock donne faux, on va sur le rightBlock
    leftBlock.setExitFalse(rightBlock);

    // Expression vient du contexte, de la syntaxe antlr 
    // Va le stocker par defaut dans reg
    visit(ctx->expression(1));
    _cfg.getCurrentBlock().addInstruction<IR::CastBool>();

    IR::BasicBlock& afterBlock = _cfg.createAndAddBlock();
    leftBlock.setExitTrue(afterBlock);

    // Peu importe comment est évalué la partie droite, son résultat détermine le résultat de left || right
    rightBlock.setExit(afterBlock);

    return 0;
}


antlrcpp::Any CFGVisitor::visitExpr_ident(ifccParser::Expr_identContext* ctx) {
    std::string varname = ctx->IDENTIFIER()->getText();
    const auto& variable = _cfg.getSymbolVar(varname);
    _cfg.getCurrentBlock().addInstruction<IR::LdLoc>(variable);
    return 0;
}

antlrcpp::Any CFGVisitor::visitExpr_const(ifccParser::Expr_constContext* ctx) {
    int value;
    if (ctx->CONST_INT()) value = std::stoi(ctx->CONST_INT()->getText());
    else value = (int)ctx->CONST_CHAR()->getText()[1];
    _cfg.getCurrentBlock().addInstruction<IR::LdConst>(value);
    return 0;
}

antlrcpp::Any CFGVisitor::visitStmt_jump_return(ifccParser::Stmt_jump_returnContext* ctx) {
    if (ctx->expression()) visit(ctx->expression());
    _cfg.getCurrentBlock().setExit(*_returnBlock);
    _cfg.createAndAddBlock();
    return 0;
}

const IR::Variable& CFGVisitor::visitAndStoreExpr(ifccParser::ExpressionContext* ctx)
{
    visit(ctx);
    const IR::Variable& lhs = _cfg.createTmpVar();
    _cfg.getCurrentBlock().addInstruction<IR::Store>(lhs);
    return lhs;
}

antlrcpp::Any CFGVisitor::visitStmt_block(ifccParser::Stmt_blockContext* ctx)
{
    _cfg.pushContext();
    ifccBaseVisitor::visitStmt_block(ctx);
    _cfg.popContext();
    return 0;
}