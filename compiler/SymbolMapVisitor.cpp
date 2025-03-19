#include <iostream>
#include "SymbolMapVisitor.h"

antlrcpp::Any SymbolMapVisitor::visitProg(ifccParser::ProgContext* ctx)
{
    _symbolMap.clear(); // Ensure that all records of symbols (used or not) are empty.
    _unusedSymbols.clear();

    visitChildren(ctx);

    if (!_unusedSymbols.empty()) { // If unused variables remain :
        std::cerr << "error: Unused variable detected : ";
        for (const auto& symbol : _unusedSymbols) std::cerr << symbol << " ";
        std::cerr << std::endl;
    }

    return 0;
}

antlrcpp::Any SymbolMapVisitor::visitStmt_declaration(ifccParser::Stmt_declarationContext* ctx)
{
    _currentStmtDeclarationSize = 4; // We only work with int
    visitChildren(ctx);
    return 0;
}

antlrcpp::Any SymbolMapVisitor::visitDeclaration(ifccParser::DeclarationContext* ctx)
{
    std::string varname = ctx->IDENTIFIER()->getText(); // Extract the VARNAME symbol from the 'define' expression, then extract its name.
    if (_symbolMap.find(varname) != _symbolMap.end()) { // If the variable we want to create is already in the symbol table : (if nothing is found we return the end)
        std::cerr << "error: Variable already defined." << std::endl;
        exit(1);
    }
    addVariable(varname, _currentStmtDeclarationSize);

    visitChildren(ctx);
    return 0;
}

antlrcpp::Any SymbolMapVisitor::visitExpr_arithmetic_add(ifccParser::Expr_arithmetic_addContext* ctx)
{
    addVariable("@tmp" + std::to_string(_tmpCount), 4, true); // TODO Use the size of the varibles
    _tmpCount++;

    visit(ctx->expression(0));
    visit(ctx->expression(1));
    return 0;
}

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

antlrcpp::Any SymbolMapVisitor::visitExpr_ident(ifccParser::Expr_identContext* ctx)
{
    std::string varname = ctx->IDENTIFIER()->getText(); // Extract the VARNAME symbol from the expression that uses it, then extract its name.
    if (_symbolMap.find(varname) == _symbolMap.end()) { // If the variable we want to assign a value to is not in the symbol table :
        std::cerr << "error: Undefined variable." << std::endl;
        exit(1);
    }
    _unusedSymbols.erase(varname); // Specify that the variable has been used at least once.
    return 0;
}

void SymbolMapVisitor::addVariable(std::string name, int size, bool used)
{    
    _currentAddress -= size;
    _symbolMap[name] = _currentAddress; // Associate the variable name to its location in the allocated memory.
    if (!used) _unusedSymbols.insert(name); // Since the variable has just been initialized, specify that it has not been used yet.
}
