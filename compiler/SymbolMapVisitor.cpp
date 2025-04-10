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
    _functions.clear();
    _functionParams.clear();

    _functions.insert("putchar");
    _functionParams["putchar"] = 1;
    _functions.insert("getchar");
    _functionParams["getchar"] = 0;

    ifccBaseVisitor::visitProg(ctx);
    return 0;
}


antlrcpp::Any SymbolMapVisitor::visitFunction_def(ifccParser::Function_defContext* ctx)
{
    pushContext();
    std::string funcName(ctx->IDENTIFIER()[0]->getText());
    if (_functions.find(funcName) != _functions.end()) {
        std::cerr << "error: Function " << funcName << " already defined." << std::endl;
        exit(1);
    }
    _functions.insert(funcName);

    // Liste des types
    auto types = ctx->TYPE();
    // Liste des identifiants (paramètres)
    auto preIdentifiers = ctx->IDENTIFIER();
    auto identifiers = std::vector<antlr4::tree::TerminalNode *>(preIdentifiers.begin() + 1, preIdentifiers.end());

    if (!types.empty() && !identifiers.empty()) {
        _functionParams[funcName] = identifiers.size();
        for (int i = 0; i < identifiers.size(); ++i)
            addVariable(identifiers[i]->getText());
    }
    else
        _functionParams[funcName] = 0;

    // Visiter le corps de la fonction
    for (auto stmt : ctx->stmt()) {
        visitStmt(stmt);  // Appelle le visiteur sur chaque statement
    }
    popContext();
    return 0;
}

antlrcpp::Any SymbolMapVisitor::visitExpr_fct_call(ifccParser::Expr_fct_callContext* ctx)
{
    std::string funcName(ctx->IDENTIFIER()->getText());
    if (_functions.find(funcName) == _functions.end()) {
        std::cerr << "error: Function " << funcName << " not defined." << std::endl;
        exit(1);
    }
    auto expre = ctx->expression();
    int nbParam;
    if (!expre.empty())
        nbParam = expre.size();
    else
        nbParam = 0;
    if (nbParam != _functionParams[funcName]) {
        std::cerr << "error: Function call " << funcName << " with not enough parameters." << std::endl;
        exit(1);
    }
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
