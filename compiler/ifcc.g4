grammar ifcc;

axiom: prog EOF;

prog: 'int' 'main' '(' ')' '{' stmt* return_stmt '}';

stmt: (declaration_stmt | assign_stmt | return_stmt);

declaration_stmt: TYPE VARNAME (',' VARNAME)* /* ('=' value_expr)? */ ';';

// TODO : gérer les 'a=b=c=42'
assign_stmt: VARNAME '=' value_expr ';';
return_stmt: RETURN value_expr ';';

value_expr: 
    VARNAME #value_expr_var
|   CONST   #value_expr_const
|   '(' value_expr ')' #value_expr_par
|   OP_ADD value_expr #value_expr_add_unary
|   value_expr OP_MULT value_expr #value_expr_mult
|   value_expr OP_ADD value_expr #value_expr_add
;

// TODO : Implémenter les caractères
// TODO : Implémenter le modulo
// TODO : Implémenter les opérateurs de comparaison

OP_MULT: [*/];
OP_ADD: [+-];
TYPE: 'int';
RETURN: 'return';                   // VARNAME must be after TYPE and RETURN, to avoid rule conflicts.
VARNAME: [a-zA-Z_][a-zA-Z_0-9]*;    // ex : 'int' must be evaluated as a TYPE to avoid it being evaluated
CONST: [0-9]+;                      // as a VARNAME (which does not work) thanks to priorities
COMMENT: '/*' .*? '*/' -> skip;
DIRECTIVE: '#' .*? '\n' -> skip;
WS: [ \t\r\n] -> channel(HIDDEN);