#include "CodeGenVisitor.h"

CodeGenVisitor::CodeGenVisitor(std::map<std::string, std::map<std::string, int>>& symbols, std::map<std::string, std::string>& blockParent) : _symbolMap(symbols), _blockParentMap(blockParent), _tmpCount(0), _blockCount(0) {}

CodeGenVisitor::~CodeGenVisitor() {}

/*
    To convert the whole program into assembly language, the visitor goes through those steps :
    - Writes the prologue
    - Visits all children of the Prog symbol so that the actual program assembly code can be written
    - Writes the epilogue
*/

antlrcpp::Any CodeGenVisitor::visitProg(ifccParser::ProgContext* ctx)
{
    ifccBaseVisitor::visitProg(ctx);
    return 0;
}


antlrcpp::Any CodeGenVisitor::visitMain(ifccParser::MainContext* ctx)
{
    std::cout << ".globl main\n";
    std::cout << "main:\n";
    std::cout << "\t# prologue:\n";
    std::cout << "\tpushq %rbp # save %rbp on the stack\n";
    std::cout << "\tmovq %rsp, %rbp # define %rbp for the current function\n\n";

    std::cout << "\t# body:\n";

    _currentBlock = "main";
    ifccBaseVisitor::visitMain(ctx);
    _currentBlock = _blockParentMap[_currentBlock];

    std::cout << "\n\t# epilogue:\n";
    std::cout << "\tpopq %rbp # restore %rbp from the stack\n";
    std::cout << "\tret # return to the caller (here the shell)" << std::endl;
    
    return 0;
}

/*
    If the declaration expression is of the type "TYPE IDENTIFIER = EXPRESSION" :
    - Visits the 'expression' in question to copy its value into eax
    - Fetches from the symbol table the address of the variable corresponding to the IDENTIFIER
    - Copies the expression value from eax to the address of the variable

    Otherwise, if the declaration expression is of the type "TYPE IDENTIFIER" :
    - There is no need to generate any assembly code.
*/
antlrcpp::Any CodeGenVisitor::visitDeclaration(ifccParser::DeclarationContext* ctx)
{
    if (ctx->expression()) {
        visit(ctx->expression());
        int identAddress = getAddress(ctx->IDENTIFIER()->getText());
        std::cout << "\tmovl\t%eax, " << identAddress << "(%rbp)\n";
    }
    return 0;
}

/*
    - Fetches from the symbol table the address of the variable corresponding to the IDENTIFIER
    - Copies the associated value in memory to eax to make it accessible to parent nodes
*/
antlrcpp::Any CodeGenVisitor::visitExpr_ident(ifccParser::Expr_identContext* ctx)
{
    int identAddress = getAddress(ctx->IDENTIFIER()->getText());
    std::cout << "\tmovl\t" << identAddress << "(%rbp), %eax\n";
    return 0;
}

/*
    - Decrypts the value 'CONST' into an integer value
    - Moves the constant value into eax
*/
antlrcpp::Any CodeGenVisitor::visitExpr_const(ifccParser::Expr_constContext* ctx)
{
    int constval = stoi(ctx->CONST()->getText());
    std::cout << "\tmovl\t$" << constval << ", %eax\n";
    return 0;
}

// /*
//     - Move the character to put in the 32-bit general-purpose register (%edi)
//     - Then call putchar
// */
// antlrcpp::Any CodeGenVisitor::visitExpr_putchar(ifccParser::Expr_putcharContext* ctx)
// {
//     visit(ctx->expression());
//     std::cout << "\tmovl\t%eax, %edi\n";
//     std::cout << "\tcall\tputchar\n";
//     return 0;
// }

// /*
//     - Call getchar and put its value in !reg (%eax)
// */
// antlrcpp::Any CodeGenVisitor::visitExpr_getchar(ifccParser::Expr_getcharContext* ctx)
// {
//     std::cout << "\tcall\tgetchar\n";
//     return 0;
// }


/*
    - Visits the 'expression' we want to perform the operation on, to copy its value into eax
    - Depending on the operator :
        If the unary operation is "-", changes the sign of the expression value in eax
        If the unary opration is "+", does nothing
*/
antlrcpp::Any CodeGenVisitor::visitExpr_arithmetic_unary(ifccParser::Expr_arithmetic_unaryContext* ctx)
{
    visit(ctx->expression());
    if (ctx->OP_ADD() && ctx->OP_ADD()->getText() == "-")
        std::cout << "\tnegl\t%eax\n";
    else if (ctx->OP_NOT()) {
        std::cout << "\tcmpl\t$0, %eax\n";
        std::cout << "\tsete\t%al\n";
        std::cout << "\tmovzbl\t%al, %eax\n";
    }
    return 0;
}

/*
    - Visits the 'expression' on the left side of the arithmetic expresssion to copy its value into eax
    - Creates a new temporary variable '@tmpX' and adds it to the symbol map (and then increases _tmpCount)
    - Copies the left expression value (inside eax) to this temporary variable in order to free eax

    - Visits the 'expression' on the right side of the arithmetic expression to copy its value into eax

    - Decides which assembly operation should be used given the operator in the arithmetic expression (either '+' or '-')

    - Places the result of 'left + right' or 'left - right' into the temporary variable
    - Copies the result from the temporary variable to eax
*/
antlrcpp::Any CodeGenVisitor::visitExpr_arithmetic_add(ifccParser::Expr_arithmetic_addContext* ctx)
{
    visit(ctx->expression(0));
    int tmpAddress = getAddress("@tmp" + std::to_string(_tmpCount++));
    std::cout << "\tmovl\t%eax, " << tmpAddress << "(%rbp)\n";
    visit(ctx->expression(1));

    std::string op = ctx->OP_ADD()->getText() == "+" ? "addl" : "subl";
    std::cout << "\t" << op << "\t%eax, " << tmpAddress << "(%rbp)\n";
    std::cout << "\tmovl\t" << tmpAddress << "(%rbp), %eax\n";
    return 0;
}

/*
    - Visits the expression on the left side of the arithmetic expresssion to copy its value into eax
    - Creates a new temporary variable '@tmpX' and adds it to the symbol map (and then increases _tmpCount)
    - Copies the left expression value (inside eax) to this temporary variable in order to free eax

    - Visits the expression on the right side of the arithmetic expression to copy its value into eax

    - Depending on the operator found in the arithmetic operation, the generated assembly code differs :

        Case of the multiplication ('*') :
        - Simply places the result of 'left * right' into eax

        Case of the division ('/') or modulo ('%'):
        - Creates a new temporary variable '@tmpY' and adds it to the symbol map (and then increases _tmpCount)
        - Copies the right expression value (inside eax) to this temporary variable in order to free eax
        - Copies the left expression value (inside '@tmpX') to eax, it will be the dividend
        - Performs a division 'left / right', then places the quotient into eax and the remainder into edx

        - For the division, the quotient is already inside eax. For the modulo, copies the value inside edx to eax
*/
antlrcpp::Any CodeGenVisitor::visitExpr_arithmetic_mult(ifccParser::Expr_arithmetic_multContext* ctx)
{
    visit(ctx->expression(0));
    int tmpAddressL = getAddress("@tmp" + std::to_string(_tmpCount++));
    std::cout << "\tmovl\t%eax, " << tmpAddressL << "(%rbp)\n";
    visit(ctx->expression(1));

    std::string op = ctx->OP_MULT()->getText().c_str();

    if (op == "*") { // Case of the multiplication : simple assembly code ------
        std::cout << "\timull\t" << tmpAddressL << "(%rbp), %eax\n";
        return 0;
    }

    // Case of the division and modulo : more complex assembly code ----------

    int tmpAddressR = getAddress("@tmp" + std::to_string(_tmpCount++));
    std::cout << "\tmovl\t%eax, " << tmpAddressR << "(%rbp)\n";

    std::cout << "\tmovl\t" << tmpAddressL << "(%rbp), %eax\n";
    std::cout << "\tcltd\n"; // 'cltd' = 'Convert Long To Double'

    std::cout << "\tidivl\t" << tmpAddressR << "(%rbp)\n";

    if (op == "%") { // Case of the modulo : must fetch the result from edx --
        std::cout << "\tmovl\t %edx, %eax\n";
    }
    return 0;
}

/*
    - Visits the 'expression' on the left side of the arithmetic expresssion to copy its value into eax
    - Creates a new temporary variable '@tmpX' and adds it to the symbol map (and then increases _tmpCount)
    - Copies the left expression value (inside eax) to this temporary variable in order to free eax

    - Visits the 'expression' on the right side of the arithmetic expression to copy its value into eax
    
    - Places the result of 'left & right' into the temporary variable
    - Copies the result from the temporary variable to eax
*/
antlrcpp::Any CodeGenVisitor::visitExpr_arithmetic_bit_and(ifccParser::Expr_arithmetic_bit_andContext* ctx)
{
    visit(ctx->expression(0));
    int tmpAddress = getAddress("@tmp" + std::to_string(_tmpCount++));
    std::cout << "\tmovl\t%eax, " << tmpAddress << "(%rbp)\n";
    visit(ctx->expression(1));
    
    std::cout << "\tandl\t%eax, " << tmpAddress << "(%rbp)\n";
    std::cout << "\tmovl\t" << tmpAddress << "(%rbp), %eax\n";
    return 0;
}

/*
    - Visits the 'expression' on the left side of the arithmetic expresssion to copy its value into eax
    - Creates a new temporary variable '@tmpX' and adds it to the symbol map (and then increases _tmpCount)
    - Copies the left expression value (inside eax) to this temporary variable in order to free eax

    - Visits the 'expression' on the right side of the arithmetic expression to copy its value into eax
    
    - Places the result of 'left ^ right' into the temporary variable
    - Copies the result from the temporary variable to eax
*/
antlrcpp::Any CodeGenVisitor::visitExpr_arithmetic_bit_xor(ifccParser::Expr_arithmetic_bit_xorContext* ctx)
{
    visit(ctx->expression(0));
    int tmpAddress = getAddress("@tmp" + std::to_string(_tmpCount++));
    std::cout << "\tmovl\t%eax, " << tmpAddress << "(%rbp)\n";
    visit(ctx->expression(1));
    
    std::cout << "\txorl\t%eax, " << tmpAddress << "(%rbp)\n";
    std::cout << "\tmovl\t" << tmpAddress << "(%rbp), %eax\n";
    return 0;
}

/*
    - Visits the 'expression' on the left side of the arithmetic expresssion to copy its value into eax
    - Creates a new temporary variable '@tmpX' and adds it to the symbol map (and then increases _tmpCount)
    - Copies the left expression value (inside eax) to this temporary variable in order to free eax

    - Visits the 'expression' on the right side of the arithmetic expression to copy its value into eax
    
    - Places the result of 'left | right' into the temporary variable
    - Copies the result from the temporary variable to eax
*/
antlrcpp::Any CodeGenVisitor::visitExpr_arithmetic_bit_or(ifccParser::Expr_arithmetic_bit_orContext* ctx)
{
    visit(ctx->expression(0));
    int tmpAddress = getAddress("@tmp" + std::to_string(_tmpCount++));
    std::cout << "\tmovl\t%eax, " << tmpAddress << "(%rbp)\n";
    visit(ctx->expression(1));
    
    std::cout << "\torl\t%eax, " << tmpAddress << "(%rbp)\n";
    std::cout << "\tmovl\t" << tmpAddress << "(%rbp), %eax\n";
    return 0;
}

/*
    - Visits the 'expression' on the left side of the arithmetic expresssion to copy its value into eax
    - Creates a new temporary variable '@tmpX' and adds it to the symbol map (and then increases _tmpCount)
    - Copies the left expression value (inside eax) to this temporary variable in order to free eax

    - Visits the 'expression' on the right side of the arithmetic expression to copy its value into eax
    
    - Decides which assembly operation should be used given the operator in the arithmetic expression (either '+' or '-')
    
    - Places the result of 'left + right' or 'left - right' into the temporary variable
    - Copies the result from the temporary variable to eax
*/
antlrcpp::Any CodeGenVisitor::visitExpr_compare(ifccParser::Expr_compareContext* ctx)
{
    visit(ctx->expression(0));
    int tmpAddress = getAddress("@tmp" + std::to_string(_tmpCount++));
    std::cout << "\tmovl\t%eax, " << tmpAddress << "(%rbp)\n";
    visit(ctx->expression(1));

    std::cout << "\tcmpl\t" << "%eax, " << tmpAddress << "(%rbp)\n";
    std::string op = ctx->OP_COMP()->getText() == "<" ? "setl" : "setg";
    std::cout << "\t" << op << "\t" <<"%al\n";
    std::cout << "\tmovzbl\t%al, %eax\n";
    return 0;
}

/*
    - Visits the 'expression' on the left side of the arithmetic expresssion to copy its value into eax
    - Creates a new temporary variable '@tmpX' and adds it to the symbol map (and then increases _tmpCount)
    - Copies the left expression value (inside eax) to this temporary variable in order to free eax

    - Visits the 'expression' on the right side of the arithmetic expression to copy its value into eax
    
    - Decides which assembly operation should be used given the operator in the arithmetic expression (either '+' or '-')
    
    - Places the result of 'left + right' or 'left - right' into the temporary variable
    - Copies the result from the temporary variable to eax
*/
antlrcpp::Any CodeGenVisitor::visitExpr_equal(ifccParser::Expr_equalContext* ctx)
{
    visit(ctx->expression(0));
    int tmpAddress = getAddress("@tmp" + std::to_string(_tmpCount++));
    std::cout << "\tmovl\t%eax, " << tmpAddress << "(%rbp)\n";
    visit(ctx->expression(1));

    std::cout << "\tcmpl\t" << "%eax, " << tmpAddress << "(%rbp)\n";
    std::string op = ctx->OP_EQ()->getText() == "==" ? "sete" : "setne";
    std::cout << "\t" << op << "\t" <<"%al\n";
    std::cout << "\tmovzbl\t%al, %eax\n";
    return 0;
}


/*
    - Visits the 'expression' that will be assigned to an IDENTIFIER, to copy its value into eax
    - Fetches from the symbol table the address of the variable corresponding to the IDENTIFIER
    - Copies the value of the expression (inside eax) to the memory address of the variable
*/
antlrcpp::Any CodeGenVisitor::visitExpr_assign(ifccParser::Expr_assignContext* ctx)
{
    visit(ctx->expression());

    int identAddress = getAddress(ctx->IDENTIFIER()->getText());
    std::cout << "\tmovl\t%eax, " << identAddress << "(%rbp)\n";
    return 0;
}

/*
    Simply visits the returned expression to copy its value into eax
*/
antlrcpp::Any CodeGenVisitor::visitStmt_jump_return(ifccParser::Stmt_jump_returnContext* ctx)
{
    visit(ctx->expression());
    return 0;
}

antlrcpp::Any CodeGenVisitor::visitStmt_block(ifccParser::Stmt_blockContext *ctx)
{
    std::string blockNumber = "block" + std::to_string(_blockCount++);
    _currentBlock = blockNumber;
    ifccBaseVisitor::visitStmt_block(ctx);
    _currentBlock = _blockParentMap[_currentBlock];
    return 0;
}


int CodeGenVisitor::getAddress(std::string varName)
{
    std::string block(_currentBlock);
    while (block.compare("")){
        if (_symbolMap[block].find(varName) != _symbolMap[block].end()) {
            return _symbolMap[block][varName];
        }
        block = _blockParentMap[block];
    }
    return 0; // Just in case
}
