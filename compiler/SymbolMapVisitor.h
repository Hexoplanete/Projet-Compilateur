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
    virtual antlrcpp::Any visitStmt_declaration(ifccParser::Stmt_declarationContext* ctx) override;
    virtual antlrcpp::Any visitDeclaration(ifccParser::DeclarationContext* ctx) override;
    
    // This method updates the usage of status of variable
    virtual antlrcpp::Any visitExpr_ident(ifccParser::Expr_identContext* ctx) override;

    // Those methods generate temporary variables
    virtual antlrcpp::Any visitExpr_arithmetic_add(ifccParser::Expr_arithmetic_addContext* ctx) override;
    virtual antlrcpp::Any visitExpr_arithmetic_mult(ifccParser::Expr_arithmetic_multContext* ctx) override;
    virtual antlrcpp::Any visitExpr_arithmetic_bit_and(ifccParser::Expr_arithmetic_bit_andContext* ctx) override;
    virtual antlrcpp::Any visitExpr_arithmetic_bit_xor(ifccParser::Expr_arithmetic_bit_xorContext* ctx) override;
    virtual antlrcpp::Any visitExpr_arithmetic_bit_or(ifccParser::Expr_arithmetic_bit_orContext* ctx) override;
    virtual antlrcpp::Any visitExpr_compare(ifccParser::Expr_compareContext* ctx) override;
    virtual antlrcpp::Any visitExpr_equal(ifccParser::Expr_equalContext* ctx) override;

    inline std::map<std::string, std::map<std::string, int>>& getSymbolMap() { return _symbolMap; }
    inline std::map<std::string, std::string>& getBlockParentBlock() { return _blockParentMap; }
    void addVariable(std::string name, int size, bool used = false);

private:
    // TODO : edit symbol map to contain variable types (int, char, double...)
    std::map<std::string, std::map<std::string, int>> _symbolMap; // The symbol table associates a block name and a variable names to their location in the allocated memory.
    std::map<std::string, std::set<std::string>> _unusedSymbols;
    int _currentStmtDeclarationSize = 0; // Indicates the size of the current variable to be declared
    std::map<std::string, int> _currentAddress; // The current memory address where we will save a variable. It is initialized as the top of the stack.
    std::string _currentBlock = ""; // The name of the current block the visitor is in
    unsigned int _blockCount = 0;
    std::map<std::string, std::string> _blockParentMap; // Gives the name of the block parent of a block with its name (gives "" when there is no parent)
    std::map<std::string, std::string> _parentFunction; // Gives the name of the function a block's name is in
    unsigned int _tmpCount = 0;
};
