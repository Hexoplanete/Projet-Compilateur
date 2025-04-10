#include <iostream>
#include <fstream>
#include <sstream>
#include <cstdlib>
#include <filesystem>

#include "antlr4-runtime.h"
#include "generated/ifccLexer.h"
#include "generated/ifccParser.h"
#include "generated/ifccBaseVisitor.h"

#include "CFGVisitor.h"
#include "SymbolMapVisitor.h"

int main(int argc, char* const * argv)
{
    if (argc != 2 && argc != 4) {
        std::cerr << "usage: ifcc INPUT [-o OUTPUT]" << std::endl;
        exit(1);
    }
    std::filesystem::path inputPath(argv[1]);
    std::filesystem::path outputPath;
    if (argc == 4) outputPath = std::filesystem::path(argv[3]);
    else outputPath = std::filesystem::path(inputPath).filename().replace_extension(".s");

    std::ifstream lecture(inputPath);
    if (!lecture.good()) {
        std::cerr << "error: cannot read file: " << argv[1] << std::endl;
        exit(1);
    }
    std::stringstream in;
    in << lecture.rdbuf();

    antlr4::ANTLRInputStream input(in.str());
    ifccLexer lexer(&input);
    antlr4::CommonTokenStream tokens(&lexer);
    tokens.fill();

    ifccParser parser(&tokens);
    antlr4::tree::ParseTree* tree = parser.axiom();

    if (parser.getNumberOfSyntaxErrors() != 0) {
        std::cerr << "error: syntax error during parsing" << std::endl;
        exit(1);
    }

    /*
    - ConstraintCheckVisitor parses the AST and checks variable usage using the symbol table
    - CFGVisitor parses the AST and creates the CFG
    - A call to a function of the CFG recursively generates the assembly code from the CFG
    */

    SymbolMapVisitor symbolsVisitor;
    symbolsVisitor.visit(tree);

    CFGVisitor cfgVisitor;
    cfgVisitor.visit(tree);


    std::ofstream ecriture(outputPath);
    cfgVisitor.getCFG().generateAsm(ecriture);

    return 0;
}
