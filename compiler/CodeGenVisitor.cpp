#include "CodeGenVisitor.h"

CodeGenVisitor::CodeGenVisitor(std::map<std::string, int>& symbols) : _symbolMap(symbols), _tmpCount(0) {}

CodeGenVisitor::~CodeGenVisitor() {}

antlrcpp::Any CodeGenVisitor::visitProg(ifccParser::ProgContext* ctx)
{
    std::cout << ".globl main\n";
    std::cout << "main:\n";
    std::cout << "\t# prologue:\n";
    std::cout << "\tpushq %rbp # save %rbp on the stack\n";
    std::cout << "\tmovq %rsp, %rbp # define %rbp for the current function\n\n";

    std::cout << "\t# body:\n";
    ifccBaseVisitor::visitProg(ctx);

    std::cout << "\n\t# epilogue:\n";
    std::cout << "\tpopq %rbp # restore %rbp from the stack\n";
    std::cout << "\tret # return to the caller (here the shell)" << std::endl;

    return 0;
}

antlrcpp::Any CodeGenVisitor::visitDeclaration(ifccParser::DeclarationContext* ctx)
{
    if (ctx->expression()) {
        visit(ctx->expression());
        int identAddress = _symbolMap[ctx->IDENTIFIER()->getText()];
        std::cout << "\tmovl\t%eax, " << identAddress << "(%rbp)\n";
    }
    return 0;
}

antlrcpp::Any CodeGenVisitor::visitExpr_ident(ifccParser::Expr_identContext* ctx)
{
    int identAddress = _symbolMap[ctx->IDENTIFIER()->getText()];
    std::cout << "\tmovl\t" << identAddress << "(%rbp), %eax\n";
    return 0;
}

antlrcpp::Any CodeGenVisitor::visitExpr_const(ifccParser::Expr_constContext* ctx)
{
    int constval = stoi(ctx->CONST()->getText());
    std::cout << "\tmovl\t$" << constval << ", %eax\n";
    return 0;
}

antlrcpp::Any CodeGenVisitor::visitExpr_arithmetic_add_unary(ifccParser::Expr_arithmetic_add_unaryContext* ctx)
{
    visit(ctx->expression());
    if (ctx->OP_ADD()->getText() == "-") {
        std::cout << "\tnegl\t%eax\n";
    }
    return 0;
}

antlrcpp::Any CodeGenVisitor::visitExpr_arithmetic_add(ifccParser::Expr_arithmetic_addContext* ctx)
{
    visit(ctx->expression(0));
    int tmpAddress = _symbolMap["@tmp" + std::to_string(_tmpCount++)];
    std::cout << "\tmovl\t%eax, " << tmpAddress << "(%rbp)\n";
    visit(ctx->expression(1));
    
    std::string op = ctx->OP_ADD()->getText() == "+" ? "addl" : "subl";
    std::cout << "\t" << op << "\t%eax, " << tmpAddress << "(%rbp)\n";
    std::cout << "\tmovl\t" << tmpAddress << "(%rbp), %eax\n";
    return 0;
}

antlrcpp::Any CodeGenVisitor::visitExpr_arithmetic_mult(ifccParser::Expr_arithmetic_multContext* ctx)
{
    visit(ctx->expression(0));
    int tmpAddressL = _symbolMap["@tmp" + std::to_string(_tmpCount++)];
    std::cout << "\tmovl\t%eax, " << tmpAddressL << "(%rbp)\n";
    visit(ctx->expression(1));
    
    std::string op = ctx->OP_MULT()->getText().c_str();
    
    if(op == "*"){
        std::cout << "\timull\t" << tmpAddressL << "(%rbp), %eax\n";
        return 0;
    }

    int tmpAddressR = _symbolMap["@tmp" + std::to_string(_tmpCount++)];
    std::cout << "\tmovl\t%eax, " << tmpAddressR << "(%rbp)\n";
    
    std::cout << "\tmovl\t" << tmpAddressL << "(%rbp), %eax\n";
    std::cout << "\tcltd\n";
    
    std::cout << "\tidivl\t" << tmpAddressR << "(%rbp)\n";
    if (op == "%") {
        std::cout << "\tmovl\t %edx, %eax\n";
    }
    return 0;
}

antlrcpp::Any CodeGenVisitor::visitExpr_assign(ifccParser::Expr_assignContext* ctx)
{
    visit(ctx->expression());

    int identAddress = _symbolMap[ctx->IDENTIFIER()->getText()];
    std::cout << "\tmovl\t%eax, " << identAddress << "(%rbp)\n";
    return 0;
}

antlrcpp::Any CodeGenVisitor::visitStmt_jump_return(ifccParser::Stmt_jump_returnContext* ctx)
{
    visit(ctx->expression());
    return 0;
}
