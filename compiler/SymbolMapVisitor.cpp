#include <iostream>
#include "SymbolMapVisitor.h"

/*
    - Ensures that all records of variables (used or not) are empty
    - Visits all children of the program to add and update all needed variables 

    - If, after the program has been visited, unused variables remain, an error is thrown
*/
antlrcpp::Any SymbolMapVisitor::visitProg(ifccParser::ProgContext* ctx)
{
    _symbolMap.clear();
    _unusedSymbols.clear();

    visitChildren(ctx);

    if (!_unusedSymbols.empty()) {
        std::cerr << "error: Unused variable detected : ";
        for (const auto& symbol : _unusedSymbols) std::cerr << symbol << " ";
        std::cerr << std::endl;
    }

    return 0;
}

/*
    - From the the 'TYPE' in a 'TYPE IDENT = EXPRESSION' declaration, deduces the size of the variable to be declared
    - Visits the children of the declaration statement so that the variable can be added to the symbol table
*/
antlrcpp::Any SymbolMapVisitor::visitStmt_declaration(ifccParser::Stmt_declarationContext* ctx)
{
    _currentStmtDeclarationSize = 4; // We only work with int for now
    visitChildren(ctx);
    return 0;
}

/*
    - Extracts the variable name from the 'VARNAME' symbol
    - If the variable name is already in the symbol table, the variable is already defined : throws an error
    - Otherwise, adds a new line corresponding to the new variable to the symbol table
    - Visits all children of the declaration
*/
antlrcpp::Any SymbolMapVisitor::visitDeclaration(ifccParser::DeclarationContext* ctx)
{
    std::string varname = ctx->IDENTIFIER()->getText();
    if (_symbolMap.find(varname) != _symbolMap.end()) {
        std::cerr << "error: Variable already defined." << std::endl;
        exit(1);
    }
    addVariable(varname, _currentStmtDeclarationSize);

    visitChildren(ctx);
    return 0;
}

/*
    - Creates a temporary variable '@tmpX' in the symbol table, used later during assembly code generation
    - Visits both left and right expressions to add their own variables to the symbol table
*/
antlrcpp::Any SymbolMapVisitor::visitExpr_arithmetic_add(ifccParser::Expr_arithmetic_addContext* ctx)
{
    addVariable("@tmp" + std::to_string(_tmpCount), 4, true); // TODO Use the actual size of the variable instead of 4
    _tmpCount++;

    visit(ctx->expression(0));
    visit(ctx->expression(1));
    return 0;
}

/*
    - Creates a temporary variable '@tmpX' in the symbol table, used later during assembly code generation
    - Creates another temporary variable '@tmpY' in the symbol table in the case of a division or modulo
    - Visits both left and right expressions to add their own variables to the symbol table
*/
antlrcpp::Any SymbolMapVisitor::visitExpr_arithmetic_mult(ifccParser::Expr_arithmetic_multContext* ctx)
{
    addVariable("@tmp" + std::to_string(_tmpCount), 4, true);
    _tmpCount++;
    if (ctx->OP_MULT()->getText() != "*") {
        addVariable("@tmp" + std::to_string(_tmpCount), 4, true);
        _tmpCount++;
    }
    
    visit(ctx->expression(0));
    visit(ctx->expression(1));
    return 0;
}

/*
    - Creates a temporary variable '@tmpX' in the symbol table, used later during assembly code generation
    - Visits both left and right expressions to add their own variables to the symbol table
*/
antlrcpp::Any SymbolMapVisitor::visitExpr_arithmetic_bit_and(ifccParser::Expr_arithmetic_bit_andContext* ctx)
{
    addVariable("@tmp" + std::to_string(_tmpCount), 4, true); // TODO Use the actual size of the variable instead of 4
    _tmpCount++;

    visit(ctx->expression(0));
    visit(ctx->expression(1));
    return 0;
}

/*
    - Creates a temporary variable '@tmpX' in the symbol table, used later during assembly code generation
    - Visits both left and right expressions to add their own variables to the symbol table
*/
antlrcpp::Any SymbolMapVisitor::visitExpr_arithmetic_bit_xor(ifccParser::Expr_arithmetic_bit_xorContext* ctx)
{
    addVariable("@tmp" + std::to_string(_tmpCount), 4, true); // TODO Use the actual size of the variable instead of 4
    _tmpCount++;

    visit(ctx->expression(0));
    visit(ctx->expression(1));
    return 0;
}

/*
    - Creates a temporary variable '@tmpX' in the symbol table, used later during assembly code generation
    - Visits both left and right expressions to add their own variables to the symbol table
*/
antlrcpp::Any SymbolMapVisitor::visitExpr_arithmetic_bit_or(ifccParser::Expr_arithmetic_bit_orContext* ctx)
{
    addVariable("@tmp" + std::to_string(_tmpCount), 4, true); // TODO Use the actual size of the variable instead of 4
    _tmpCount++;

    visit(ctx->expression(0));
    visit(ctx->expression(1));
    return 0;
}

/*
    - Creates a temporary variable '@tmpX' in the symbol table, used later during assembly code generation
    - Visits both left and right expressions to add their own variables to the symbol table
*/
antlrcpp::Any SymbolMapVisitor::visitExpr_compare(ifccParser::Expr_compareContext* ctx)
{
    addVariable("@tmp" + std::to_string(_tmpCount), 4, true); // TODO Use the actual size of the variable instead of 4
    _tmpCount++;

    visit(ctx->expression(0));
    visit(ctx->expression(1));
    return 0;
}

/*
    - Creates a temporary variable '@tmpX' in the symbol table, used later during assembly code generation
    - Visits both left and right expressions to add their own variables to the symbol table
*/
antlrcpp::Any SymbolMapVisitor::visitExpr_equal(ifccParser::Expr_equalContext* ctx)
{
    addVariable("@tmp" + std::to_string(_tmpCount), 4, true); // TODO Use the actual size of the variable instead of 4
    _tmpCount++;

    visit(ctx->expression(0));
    visit(ctx->expression(1));
    return 0;
}

/*
    - Extracts the variable name from the 'VARNAME' symbol
    - If the variable name is not in the symbol table, the variable is used while not defined : throws an error
    - Otherwise, marks the variable as used
*/
antlrcpp::Any SymbolMapVisitor::visitExpr_ident(ifccParser::Expr_identContext* ctx)
{
    std::string varname = ctx->IDENTIFIER()->getText();
    if (_symbolMap.find(varname) == _symbolMap.end()) { 
        std::cerr << "error: Undefined variable." << std::endl;
        exit(1);
    }
    _unusedSymbols.erase(varname);
    return 0;
}

/*
    - Computes the address where the variable should be stored thanks to the information about the variable size
    - Creates a new line in the symbol table, associating the variable name to a memory address
    - Adds the freshly-created variable to the list of unused table
*/
void SymbolMapVisitor::addVariable(std::string name, int size, bool used)
{    
    _currentAddress -= size;
    _symbolMap[name] = _currentAddress;
    if (!used) _unusedSymbols.insert(name);
}
