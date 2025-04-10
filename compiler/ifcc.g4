grammar ifcc;

axiom: prog EOF;

prog: function_def*;

function_def: (TYPE | VOID) IDENTIFIER '(' (TYPE IDENTIFIER (',' TYPE IDENTIFIER)*)? ')' '{' stmt* '}';

stmt_block: '{' stmt* '}';

stmt:
	stmt_declaration | stmt_expression | stmt_jump | stmt_block | stmt_if | stmt_while;

// TODO : quand on aura + de types
stmt_declaration: TYPE declaration (',' declaration)* ';';
declaration: IDENTIFIER ('=' expression)?;

stmt_expression: expression? ';';
expression:
	'(' expression ')'										# expr_arithmetic_par
	| IDENTIFIER OP_INCR 									# expr_post_incr
	| IDENTIFIER '(' (expression (',' expression)*)? ')'	# expr_fct_call
	| (OP_ADD|OP_NOT) expression							# expr_arithmetic_unary
	| expression OP_MULT expression							# expr_arithmetic_mult
	| expression OP_ADD expression							# expr_arithmetic_add
	| expression '&' expression								# expr_arithmetic_bit_and
	| expression '^' expression								# expr_arithmetic_bit_xor
	| expression '|' expression								# expr_arithmetic_bit_or
	| OP_INCR IDENTIFIER 									# expr_pre_incr
	| expression OP_COMP expression							# expr_compare	
	| expression OP_EQ expression							# expr_equal
	| expression '&&' expression							# expr_arithmetic_lazy_and
	| expression '||' expression							# expr_arithmetic_lazy_or
	| IDENTIFIER '=' expression								# expr_assign
	| IDENTIFIER OP_AFF_ADD expression 						# expr_arithmetic_aff_add
	| IDENTIFIER											# expr_ident
	| (CONST_INT|CONST_CHAR)													# expr_const;

stmt_jump: RETURN expression? ';' # stmt_jump_return;

// const_expr:
// 	INTEGER
// 	| CHAR
// 	| FLOAT

stmt_if : 'if' '(' expression ')' stmt_then stmt_else?;
stmt_then : stmt_block | stmt_expression;
stmt_else : 'else' (stmt_block | stmt_expression | stmt_if);

stmt_while : 'while' '(' expression ')' stmt_block;

// TODO : Implémenter les caractères

OP_MULT: [*/%];
OP_ADD: [+-];
OP_AFF_ADD: '+=' | '-=';
OP_INCR: '++' | '--';
OP_NOT: '!';
OP_COMP: '<' | '>' | '<=' | '>=';
OP_EQ: '==' | '!=';
TYPE: 'int';
VOID: 'void';
RETURN: 'return';                   // VARNAME must be after TYPE and RETURN, to avoid rule conflicts.
IDENTIFIER: [a-zA-Z_][a-zA-Z_0-9]*;    // ex : 'int' must be evaluated as a TYPE to avoid it being evaluated
CONST_INT: [0-9]+;                      // as a VARNAME (which does not work) thanks to priorities
CONST_CHAR:'\'' . '\'';                      // as a VARNAME (which does not work) thanks to priorities
COMMENT: '/*' .*? '*/' -> skip;
DIRECTIVE: '#' .*? '\n' -> skip;
WS: [ \t\r\n] -> channel(HIDDEN);