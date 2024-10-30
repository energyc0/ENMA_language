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

    for_statement: "for" "=>" "(" variable_statement "to" expression [ ":" assignment_statement ] ")" compound_statement ;

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

**Function definition** - `'function name' => (['arg_name1' 'type1', 'arg_name2' 'type2', ...]) {'code'}`

**Function call** - `'function name'([ arg1, arg2, ...]);`

**Functions** - `exit(exit_value)`

**Return function value** - `return [value];`

**Other** - ENMA doesn't care about spaces and new line symbols. You can write them all you want. Spaces and new line symbols are some sort of dividers between tokens.

## Program structure
```
sum => (a, b) {
    return a + b;
}

foo=>() {
    return;
}

let a = 10;
let b = 11;

let c = sum(a, b);

foo();

for => (i = c to 0 : -1) {
    b = b + 1;
}

while => (a < b) {
    a = a + 3;
}
```