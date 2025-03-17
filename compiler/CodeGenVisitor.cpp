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

antlrcpp::Any CodeGenVisitor::visitValue_expr_add(ifccParser::Value_expr_addContext* ctx)
{
    visit(ctx->value_expr(0));
    int tmpAddress = _symbolMap["@tmp" + std::to_string(_tmpCount++)];
    std::cout << "\tmovl\t%eax, " << tmpAddress << "(%rbp)\n";
    visit(ctx->value_expr(1));

    std::string op = ctx->OP_ADD()->getText() == "+" ? "addl" : "subl";
    std::cout << "\t" << op << "\t%eax, " << tmpAddress << "(%rbp)\n";
    std::cout << "\tmovl\t" << tmpAddress << "(%rbp), %eax\n";
    return 0;
}

antlrcpp::Any CodeGenVisitor::visitValue_expr_mult(ifccParser::Value_expr_multContext* ctx)
{
    visit(ctx->value_expr(0));
    int tmpAddress = _symbolMap["@tmp" + std::to_string(_tmpCount++)];
    std::cout << "\tmovl\t%eax, " << tmpAddress << "(%rbp)\n";
    visit(ctx->value_expr(1));

    std::string op = ctx->OP_MULT()->getText() == "*" ? "imull" : "divl";
    // std::cout << "\t" << op << "\t%eax, " << tmpAddress << "(%rbp)\n";
    std::cout << "\t" << op << "\t" << tmpAddress << "(%rbp), %eax\n";
    return 0;
}


antlrcpp::Any CodeGenVisitor::visitValue_expr_add_unary(ifccParser::Value_expr_add_unaryContext* ctx)
{
    visit(ctx->value_expr());
    if (ctx->OP_ADD()->getText() == "-") {
        std::cout << "\tnegl\t%eax\n";
    }
    return 0;
}

antlrcpp::Any CodeGenVisitor::visitValue_expr_var(ifccParser::Value_expr_varContext* ctx)
{
    int varaddress = _symbolMap[ctx->VARNAME()->getText()];
    std::cout << "\tmovl\t" << varaddress << "(%rbp), %eax\n";
    return 0;
}

antlrcpp::Any CodeGenVisitor::visitValue_expr_const(ifccParser::Value_expr_constContext* ctx)
{
    int constval = stoi(ctx->CONST()->getText());
    std::cout << "\tmovl\t$" << constval << ", %eax\n";
    return 0;
}
