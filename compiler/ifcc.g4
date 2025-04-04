grammar ifcc;

axiom: prog EOF;

prog: main;

main: 'int' 'main' '(' ')' '{' stmt* stmt_jump '}';

stmt:
	stmt_declaration | stmt_expression | stmt_jump | stmt_block ;

// TODO : quand on aura + de types
stmt_declaration: TYPE declaration (',' declaration)* ';';
declaration: IDENTIFIER ('=' expression)?;

stmt_expression: expression? ';';
expression:
	IDENTIFIER						# expr_ident
	| CONST							# expr_const
	| '(' expression ')'			# expr_arithmetic_par
	| (OP_ADD|OP_NOT) expression	# expr_arithmetic_unary
	| expression OP_MULT expression	# expr_arithmetic_mult
	| expression OP_ADD expression	# expr_arithmetic_add
	| expression '&' expression		# expr_arithmetic_bit_and
	| expression '^' expression		# expr_arithmetic_bit_xor
	| expression '|' expression		# expr_arithmetic_bit_or
	| expression OP_COMP expression	# expr_compare	
	| expression OP_EQ expression	# expr_equal
	| IDENTIFIER '=' expression		# expr_assign;

stmt_jump: RETURN expression? ';' # stmt_jump_return;

stmt_block: '{' stmt* '}';
// const_expr:
// 	INTEGER
// 	| CHAR
// 	| FLOAT


// TODO : Implémenter les caractères

OP_MULT: [*/%];
OP_ADD: [+-];
OP_NOT: '!';
OP_COMP: '<' | '>';
OP_EQ: '==' | '!=';
TYPE: 'int';
RETURN: 'return';                   // VARNAME must be after TYPE and RETURN, to avoid rule conflicts.
IDENTIFIER: [a-zA-Z_][a-zA-Z_0-9]*;    // ex : 'int' must be evaluated as a TYPE to avoid it being evaluated
CONST: [0-9]+;                      // as a VARNAME (which does not work) thanks to priorities
COMMENT: '/*' .*? '*/' -> skip;
DIRECTIVE: '#' .*? '\n' -> skip;
WS: [ \t\r\n] -> channel(HIDDEN);