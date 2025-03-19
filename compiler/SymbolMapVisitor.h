#pragma once

#include <string>
#include <map>
#include <set>
#include "antlr4-runtime.h"
#include "generated/ifccBaseVisitor.h"


class SymbolMapVisitor : public ifccBaseVisitor {
public:
    // Only the grammar elements which lead to a modification of the symbol map and/or the used symbol map
    virtual antlrcpp::Any visitProg(ifccParser::ProgContext* ctx) override;

    // Create a new entry in the Symbol Map
    virtual antlrcpp::Any visitStmt_declaration(ifccParser::Stmt_declarationContext* ctx) override;
    virtual antlrcpp::Any visitDeclaration(ifccParser::DeclarationContext* ctx) override;

    // Variable usage check
    virtual antlrcpp::Any visitExpr_ident(ifccParser::Expr_identContext* ctx) override;

    // Operations temporary variable gen
    virtual antlrcpp::Any visitExpr_arithmetic_add(ifccParser::Expr_arithmetic_addContext* ctx) override;
    virtual antlrcpp::Any visitExpr_arithmetic_mult(ifccParser::Expr_arithmetic_multContext* ctx) override;


    inline std::map<std::string, int>& getSymbolMap() { return _symbolMap; }

    void addVariable(std::string name, int size, bool used = false);

private:
    // TODO : edit symbol map to contain variable types (int, char, double...)
    std::map<std::string, int> _symbolMap; // The symbol table associates variable names to their location in the allocated memory.
    std::set<std::string> _unusedSymbols;
    int _currentStmtDeclarationSize = 0;
    int _currentAddress = 0; // The current memory address where we will save a variable. It is initialized as the top of the stack.
    int _tmpCount = 0;
};
