#pragma once

#include <string>
#include <map>
#include <set>
#include "antlr4-runtime.h"
#include "generated/ifccBaseVisitor.h"


class SymbolMapVisitor : public ifccBaseVisitor {
public:
    // This method only parses the grammar elements which lead to a modification of the symbol map and/or the used symbol map
    virtual antlrcpp::Any visitProg(ifccParser::ProgContext* ctx) override;


    virtual antlrcpp::Any visitMain(ifccParser::MainContext* ctx) override;
    virtual antlrcpp::Any visitStmt_block(ifccParser::Stmt_blockContext* ctx) override;

    // Those methods generate non-temporary variables
    virtual antlrcpp::Any visitDeclaration(ifccParser::DeclarationContext* ctx) override;

    // This method updates the usage of status of variable
    virtual antlrcpp::Any visitExpr_ident(ifccParser::Expr_identContext* ctx) override;

    void pushContext();
    void popContext();
    void addVariable(std::string name);
    void useVariable(std::string name);
private:
    std::vector<std::set<std::string>> _contextSymbolMaps; // The symbol table associates a block name and a variable names to their location in the allocated memory.
    std::vector<std::set<std::string>> _contextUnusedSymbols;
};
