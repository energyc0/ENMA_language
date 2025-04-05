# ENMA programming language


## Info

**Token types**
1. Integer constant
2. Identifier
3. Operator
3. Keyword
4. Punctuation

## Grammar

    statements:   statement
                | statement statements
                ;

    statement:    print_statement
                | variable_statement
                | compound_statement
                | if_statement
                | for_statement
                | while_statement
                ;

    print_statement: "print" "(" expression ")" ";"

    variable_statement: | declaration_statement
                        | assignment_statement
                        ;
    declaration_statement: "let" identifier "=" expression ";"

    assignment_statement: identifier "=" expression ";"

    compound_statement: "{" "}"
                        | "{" statements "}"
                        ;

    if_statement:   if_head
                    | if_head "else" compound_statement
                    ;

    if_head: "if" "=>" "(" unary_expression ")" compound_statement ;

    for_statement: "for" "=>" "(" for_loop_start_statement "to" expression [ ":" expression ] ")" compound_statement ;

    for_loop_start_statement:     identifier
                                | variable_statement
                                ;

    while_statement: "while" "=>" "(" unary expression ")" compound_statement ;

    expression  : number
                | identifier
                | expression "+" expression
                | expression "-" expression
                | expression "/" expression
                | expression "*" expression
                | "("expression")"
                | identifier "=" expression
                | unary_expression
                ;

    unary_expression: expression "==" expression
                    | expression "!=" expression
                    | expression ">" expression
                    | expression "<" expression
                    | expression ">=" expression
                    | expression "<=" expression
                    ;


    number :    int constant
    
    identifier: string constant

## Syntax

**Keywords** - `let`, `return`, `if`,`else`, `for`, `while`

**Binary operators** - `+`, `-`, `/`, `*`, `(`, `)`, `=`

**Unary operators** - `>`, `<`, `==`, `!=`, `>=`, `<=`

**Punctuation** - `:`, `;`, `,`, `=>`, `{` , `}`

**Variable declaration** - `let 'variable name' = 'expression';`

**`for` loop** - `for => ('variable iterator name' = 'expression' to 'expression' [: iterator change expression]) {'code'}`

**`while` loop** - `while => ('condition') {'code'}`

**Other** - ENMA doesn't care about spaces and new line symbols. You can write them all you want. Spaces and new line symbols are some sort of dividers between tokens.

## Program structure
```
let a = 0;
while => (a < 9){
    print(a);
    a = a + 3;
}

let b = 5;
for => (let i = a to 0 : -1){
    b =b + 1;
}
print(b);
```