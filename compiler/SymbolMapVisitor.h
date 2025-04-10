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

    virtual antlrcpp::Any visitFunction_def(ifccParser::Function_defContext* ctx) override;
    virtual antlrcpp::Any visitExpr_fct_call(ifccParser::Expr_fct_callContext* ctx) override;
    
    virtual antlrcpp::Any visitStmt_block(ifccParser::Stmt_blockContext* ctx) override;

    // Those methods generate non-temporary variables
    virtual antlrcpp::Any visitDeclaration(ifccParser::DeclarationContext* ctx) override;

    // This method updates the usage of status of variable
    virtual antlrcpp::Any visitExpr_ident(ifccParser::Expr_identContext* ctx) override;

    void pushContext();
    void popContext();
    void addVariable(std::string name);
    void useVariable(std::string name);
    inline std::set<std::string>& getFunctionSet() { return _functions; }
private:
    std::vector<std::set<std::string>> _contextSymbolMaps; // The symbol table associates a block name and a variable names to their location in the allocated memory.
    std::vector<std::set<std::string>> _contextUnusedSymbols;
    
    std::set<std::string> _functions; // Set with all the names of all functions
    std::map<std::string, int> _functionParams; // A map indicating the number of parameters a function has
};
