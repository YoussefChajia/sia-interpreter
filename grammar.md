<program> ::= { <statement> ";" }

<block> ::= "{" { <statement> ";" } "}"

<statement> ::= <variable_declaration>
            | <assignment>
            | <expression>
            | <control_structure>
            | <function_declaration>

<!-- is the variable declaration actually needed? -->

<variable_declaration> ::= <identifier> [ "=" <expression> ]

<assignment> ::= <identifier> "=" <expression>

<expression> ::= <literal> | <identifier> | <binary_operation> | <unary_operation> | <function_call>

<binary_operation> ::= <expression> <binary_operator> <expression>
<binary_operator> ::= "+" | "-" | "*" | "/" | "%" | "==" | "!=" | "<" | ">" | "<=" | ">=" | "&&" | "||"

<unary_operation> ::= <unary_operator> <expression>
<unary_operator> ::= "-" | "!"

<literal> ::= <integer> | <float> | <string> | <boolean>

<integer> ::= [ "-" ] <digit> { <digit> }
<double> ::= [ "-" ] <digit> { <digit> } "." <digit> { <digit> }
<string> ::= "\"" { <character> } "\""
<boolean> ::= "true" | "false"

<control_structure> ::= <if_statement> | <loop> | <return_statement>

<if_statement> ::= "if" "(" <expression> ")" "{" <statement> [ "}" "else" "{" <statement> ] "}"

<loop> ::= "loop" "(" <expression> ")" "{" <statement> "}"

<return_statement> ::= "return" <expression>

<function_declaration> ::= "function" <identifier> "(" [ <parameter_list> ] ")" "{" <statement> "}"
<parameter_list> ::= <identifier> { "," <identifier> }

<function_call> ::= <identifier> "(" [ <argument_list> ] ")"
<argument_list> ::= <expression> { "," <expression> }

<identifier> ::= <letter> { <letter> | <digit> | "_" }
<digit> ::= "0" | "1" | "2" | "3" | "4" | "5" | "6" | "7" | "8" | "9"
<letter> ::= "a" | "b" | "c" | ... | "z" | "A" | "B" | "C" | ... | "Z"
<character> ::= any valid character except "\""

<comment> ::= "//" { <character> } "\n"
            | "/*" { <character> } "*/"
