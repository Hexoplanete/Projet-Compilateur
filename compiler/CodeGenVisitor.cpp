#include "CodeGenVisitor.h"

CodeGenVisitor::CodeGenVisitor(std::map<std::string, int>& symbols): _symbolMap(symbols) {}

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

antlrcpp::Any CodeGenVisitor::visitAssign_stmt(ifccParser::Assign_stmtContext* ctx)
{
    visit(ctx->value_expr());
    
    int varaddress = _symbolMap[ctx->VARNAME()->getText()];
    std::cout << "\tmovl\t%eax, " << varaddress << "(%rbp)\n";
    return 0;
}

antlrcpp::Any CodeGenVisitor::visitReturn_stmt(ifccParser::Return_stmtContext* ctx)
{
    visit(ctx->value_expr());
    return 0;
}

antlrcpp::Any CodeGenVisitor::visitValue_expr_const(ifccParser::Value_expr_constContext* ctx)
{
    int constval = stoi(ctx->CONST()->getText());
    std::cout << "\tmovl\t$" << constval << ", %eax\n";
    return 0;
}

antlrcpp::Any CodeGenVisitor::visitValue_expr_var(ifccParser::Value_expr_varContext* ctx)
{
    int varaddress = _symbolMap[ctx->VARNAME()->getText()];
    std::cout << "\tmovl\t" << varaddress << "(%rbp), %eax\n";
    return 0;
}