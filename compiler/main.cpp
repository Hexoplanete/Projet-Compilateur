#include <iostream>
#include <fstream>
#include <sstream>
#include <cstdlib>

#include "antlr4-runtime.h"
#include "generated/ifccLexer.h"
#include "generated/ifccParser.h"
#include "generated/ifccBaseVisitor.h"

#include "CodeGenVisitor.h"
#include "SymbolMapVisitor.h"

int main(int argn, const char** argv)
{
    std::stringstream in;
    if (argn == 2)
    {
        std::ifstream lecture(argv[1]);
        if (!lecture.good())
        {
            std::cerr << "error: cannot read file: " << argv[1] << std::endl;
            exit(1);
        }
        in << lecture.rdbuf();
    }
    else
    {
        std::cerr << "usage: ifcc path/to/file.c" << std::endl;
        exit(1);
    }

    antlr4::ANTLRInputStream input(in.str());

    ifccLexer lexer(&input);
    antlr4::CommonTokenStream tokens(&lexer);

    tokens.fill();

    ifccParser parser(&tokens);
    antlr4::tree::ParseTree* tree = parser.axiom();

    if (parser.getNumberOfSyntaxErrors() != 0)
    {
        std::cerr << "error: syntax error during parsing" << std::endl;
        exit(1);
    }

    // First step of compilation : the SymbolMapVisitor parses the AST to establish the table,
    // which will associate variable names to memory addresses
    SymbolMapVisitor symbolsVisitor;
    symbolsVisitor.visit(tree);

    // Second step of compilation : the CodeGenVisitor parses the AST once again and, with the
    // help of the symbol table, generates assembly code corresponding to the tree
    CodeGenVisitor generatorVisitor(symbolsVisitor.getSymbolMap());
    generatorVisitor.visit(tree);

    return 0;
}
