grammar ifcc;

axiom: prog EOF;

prog: 'int' 'main' '(' ')' '{' stmt* return_stmt '}';

stmt: (define_stmt | assign_stmt | return_stmt);

// TODO :  implémenter le 'int a, b, c'
define_stmt: TYPE VARNAME ';' /*(',' VARNAME)* */  /* ('=' value_expr)? */;
assign_stmt: VARNAME '=' value_expr ';';
return_stmt: RETURN value_expr ';';
value_expr: CONST   #value_expr_const
        |   VARNAME #value_expr_var;

TYPE: 'int';
RETURN: 'return';                   // VARNAME must be after TYPE and RETURN, to avoid rule conflicts.
VARNAME: [a-zA-Z_][a-zA-Z_0-9]*;    // ex : 'int' must be evaluated as a TYPE to avoid it being evaluated
CONST: [0-9]+;                      // as a VARNAME (which does not work) thanks to priorities
COMMENT: '/*' .*? '*/' -> skip;
DIRECTIVE: '#' .*? '\n' -> skip;
WS: [ \t\r\n] -> channel(HIDDEN);