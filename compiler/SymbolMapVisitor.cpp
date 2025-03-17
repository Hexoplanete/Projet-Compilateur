#include <iostream>
#include "SymbolMapVisitor.h"

antlrcpp::Any SymbolMapVisitor::visitProg(ifccParser::ProgContext* ctx)
{
    _symbolMap.clear(); // Ensure that all records of symbols (used or not) are empty.
    _unusedSymbols.clear();

    visitChildren(ctx);
    // ifccBaseVisitor::visitProg(ctx); // Proceed to the visit of the whole program.

    if (!_unusedSymbols.empty()) { // If unused variables remain :
        std::cerr << "error: Unused variable detected : ";
        for (const auto& symbol : _unusedSymbols) std::cerr << symbol << " ";
        std::cerr << std::endl;
    }

    return 0;
}

antlrcpp::Any SymbolMapVisitor::visitDeclaration_stmt(ifccParser::Declaration_stmtContext* ctx)
{
    std::vector<antlr4::tree::TerminalNode*> vars = ctx->VARNAME();
    for (antlr4::tree::TerminalNode* var : vars) {
        std::string varname = var->getText(); // Extract the VARNAME symbol from the 'define' expression, then extract its name.
        if (_symbolMap.find(varname) != _symbolMap.end()) { // If the variable we want to create is already in the symbol table : (if nothing is found we return the end)
            std::cerr << "error: Variable already defined." << std::endl;
            exit(1);
        }
        int size = 4; // An integer variable is 4 bytes long
        _currentAddress -= size;
        _symbolMap[varname] = _currentAddress; // Associate the variable name to its location in the allocated memory.
        _unusedSymbols.insert(varname); // Since the variable has just been initialized, specify that it has not been used yet.
    }
    return 0;
}

antlrcpp::Any SymbolMapVisitor::visitValue_expr_add(ifccParser::Value_expr_addContext* ctx)
{
    std::string tmpVarName = "@tmp" + std::to_string(_tmpCount);
    _tmpCount++;
    int size = 4;
    
    _currentAddress -= size;
    _symbolMap[tmpVarName] = _currentAddress;

    visit(ctx->value_expr(0));
    visit(ctx->value_expr(1));
    return 0;
}

antlrcpp::Any SymbolMapVisitor::visitValue_expr_mult(ifccParser::Value_expr_multContext* ctx)
{
    std::string tmpVarName = "@tmp" + std::to_string(_tmpCount);
    _tmpCount++;
    int size = 4;

    _currentAddress -= size;
    _symbolMap[tmpVarName] = _currentAddress;

    visit(ctx->value_expr(0));
    visit(ctx->value_expr(1));
    return 0;
}


antlrcpp::Any SymbolMapVisitor::visitValue_expr_var(ifccParser::Value_expr_varContext* ctx)
{
    std::string varname = ctx->VARNAME()->getText(); // Extract the VARNAME symbol from the expression that uses it, then extract its name.
    if (_symbolMap.find(varname) == _symbolMap.end()) { // If the variable we want to assign a value to is not in the symbol table :
        // TODO throw exception (scream)
        std::cerr << "error: Undefined variable." << std::endl;
        exit(1);
    }
    _unusedSymbols.erase(varname); // Specify that the variable has been used at least once.
    return 0;
}
