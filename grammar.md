<Program> ::= { <Statement> }

<Statement> ::= <Assignment> | <FunctionCall> | FUNCTION <FunctionName> <FunctionBody>

<Assignment> ::= IDENTIFIER EQUAL <Expression>

<ArgumentList> ::= <Expression> { COMMA <Expression> }

<Expression> ::= <LogicalExpr>

<LogicalExpr> ::= <ComparisonExpr> { (AND | OR) <ComparisonExpr> }

<ComparisonExpr> ::= <ArithmeticExpr> [ <ComparisonOp> <ArithmeticExpr> ]

<ArithmeticExpr> ::= <Term> { (PLUS | MINUS) <Term> }

<Term> ::= <Factor> { (STAR | SLASH | PERCENT) <Factor> }

<Factor> ::= [ MINUS | NOT ] ( NUMBER | STRING | NIL | TRUE | FALSE | IDENTIFIER | LEFT_PAREN <Expression> RIGHT_PAREN )

<ComparisonOp> ::= LESS | LESS_EQUAL | GREATER | GREATER_EQUAL | EQUAL_EQUAL | NOT_EQUAL