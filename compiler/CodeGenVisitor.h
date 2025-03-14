#pragma once


#include "antlr4-runtime.h"
#include "generated/ifccBaseVisitor.h"


class  CodeGenVisitor : public ifccBaseVisitor {
public:
    CodeGenVisitor(std::map<std::string, int>& symbols);
    ~CodeGenVisitor();
    virtual antlrcpp::Any visitProg(ifccParser::ProgContext* ctx) override;
    virtual antlrcpp::Any visitAssign_stmt(ifccParser::Assign_stmtContext* ctx) override;
    virtual antlrcpp::Any visitValue_expr_const(ifccParser::Value_expr_constContext *ctx) override;
    virtual antlrcpp::Any visitValue_expr_var(ifccParser::Value_expr_varContext *ctx) override;
    virtual antlrcpp::Any visitReturn_stmt(ifccParser::Return_stmtContext *ctx) override;

private:
    std::map<std::string, int>& _symbolMap;
};
