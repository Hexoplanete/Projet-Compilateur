#include "CodeGenVisitor.h"

antlrcpp::Any CodeGenVisitor::visitProg(ifccParser::ProgContext *ctx) 
{
    std::cout << ".globl main\n";
    std::cout << "main:\n";
    std::cout << "\t# prologue:\n";
    std::cout << "\tpushq %rbp # save %rbp on the stack\n";
    std::cout << "\tmovq %rsp, %rbp # define %rbp for the current function\n\n";
    
    std::cout << "\t# body:\n";
    this->visit(ctx->return_stmt());

    std::cout << "\n\t# epilogue:\n";
    std::cout << "\tpopq %rbp # restore %rbp from the stack\n";
    std::cout << "\tret # return to the caller (here the shell)" << std::endl;

    return 0;
}


antlrcpp::Any CodeGenVisitor::visitReturn_stmt(ifccParser::Return_stmtContext *ctx)
{
    int retval = stoi(ctx->CONST()->getText());

    std::cout << "\tmovl $"<<retval<<", %eax\n" ;

    return 0;
}
