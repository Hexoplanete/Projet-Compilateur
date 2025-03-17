#pragma once

#include <string>
#include <map>
#include <set>
#include "antlr4-runtime.h"
#include "generated/ifccBaseVisitor.h"


class SymbolMapVisitor : public ifccBaseVisitor {
public:
    virtual antlrcpp::Any visitProg(ifccParser::ProgContext* ctx) override;
    virtual antlrcpp::Any visitDeclaration_stmt(ifccParser::Declaration_stmtContext* ctx) override;

    virtual antlrcpp::Any visitValue_expr_add(ifccParser::Value_expr_addContext* ctx) override;
    virtual antlrcpp::Any visitValue_expr_mult(ifccParser::Value_expr_multContext* ctx) override;

    virtual antlrcpp::Any visitValue_expr_var(ifccParser::Value_expr_varContext* ctx) override;

    inline std::map<std::string, int>& getSymbolMap() { return _symbolMap; }

private:
    std::map<std::string, int> _symbolMap; // The symbol table associates variable names to their location in the allocated memory.
    std::set<std::string> _unusedSymbols;
    int _currentAddress = 0; // The current memory address where we will save a variable. It is initialized as the top of the stack.
    int _tmpCount = 0;
};
