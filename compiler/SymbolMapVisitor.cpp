#include <iostream>
#include "SymbolMapVisitor.h"

/*
    - Ensures that all records of variables (used or not) are empty
    - Visits all children of the program to add and update all needed variables 

    - If, after the program has been visited, unused variables remain, an error is thrown
*/
antlrcpp::Any SymbolMapVisitor::visitProg(ifccParser::ProgContext* ctx)
{
    _contextSymbolMaps.clear();
    _contextUnusedSymbols.clear();
    ifccBaseVisitor::visitProg(ctx);
    return 0;
}

antlrcpp::Any SymbolMapVisitor::visitMain(ifccParser::MainContext* ctx)
{
    pushContext();
    ifccBaseVisitor::visitMain(ctx);
    popContext();
    return 0;
}

antlrcpp::Any SymbolMapVisitor::visitStmt_block(ifccParser::Stmt_blockContext* ctx)
{
    pushContext();
    ifccBaseVisitor::visitStmt_block(ctx);
    popContext();
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
    addVariable(varname);
    visitChildren(ctx);
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
    useVariable(varname);
    return 0;
}



void SymbolMapVisitor::pushContext()
{
    _contextSymbolMaps.push_back(std::set<std::string>());
    _contextUnusedSymbols.push_back(std::set<std::string>());
}

void SymbolMapVisitor::popContext()
{
    const auto& unused = _contextUnusedSymbols[_contextUnusedSymbols.size() - 1];
    if (!unused.empty()) {
        std::cerr << "error: Unused variable detected : ";
        for (const auto& symbol : unused) std::cerr << symbol << " ";
        std::cerr << std::endl;
    }
    _contextSymbolMaps.pop_back();
    _contextUnusedSymbols.pop_back();
}

/*
    - Computes the address where the variable should be stored thanks to the information about the variable size
    - Creates a new line in the symbol table, associating the variable name to a memory address
    - Adds the freshly-created variable to the list of unused table
*/
void SymbolMapVisitor::addVariable(std::string name)
{
    auto& currentMap = _contextSymbolMaps[_contextSymbolMaps.size() - 1];
    if (!currentMap.empty() && currentMap.find(name) != currentMap.end()) {
        std::cerr << "error: Variable already defined." << std::endl;
        exit(1);
    }
    currentMap.insert(name);
    _contextUnusedSymbols[_contextUnusedSymbols.size() - 1].insert(name);
}

void SymbolMapVisitor::useVariable(std::string name)
{
    for (int i = _contextSymbolMaps.size() - 1; i >= 0; i--) {
        const auto& symbolMap = _contextSymbolMaps[i];
        auto& unused = _contextUnusedSymbols.at(i);
        if (symbolMap.empty() || symbolMap.find(name) == symbolMap.end()) continue;
        unused.erase(name);
        return;
    }
    std::cerr << "error: Undefined variable." << std::endl;
    exit(1);
}
