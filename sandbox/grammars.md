# Alize Grammars

```c++

/// statement -> exprStmt
///               | ifStmt
///               | printStmt
///				  | returnStmt
///	              | whileStmt
///               | block ;

/// whileStmt -> "while" "(" expression ")" statement ;

/// ifStmt -> "if" "(" expression ")" statement
///               ( "else" statement )? ;

/// forStmt -> "for" "(" plainForParams | rangeBasedForParams ")" statement ;
/// plainForParams ->  ( varStmt ";" | exprStmt | ";" )
///                 expression? ";"
///                 expression?
/// rangeBasedForParams -> varStmt ":" expression

/// returnStmt -> "return" expression? ";" ;
/// expr -> comma
/// comma ->  conditional ( "," conditional )* ;
/// conditional -> assigment ("?" expr ":" conditional )? ;

/// initializer -> conditional (not expr because comma expression will lead to ambiguity
///               | list '{' initializer_list '}'
///			      | map '{' map_initializer_list '}'

/// initializer_list -> initializer (',' initializer)*

/// initializer_pair -> initializer ":" initializer
/// map_initializer_list -> initializer_pair (',' initializer_pair)*

/// assignment -> IDENTIFIER "=" assignment
///               | logical_or

/// logical_or -> logical_and ("or" logical_and)*

/// logical_and -> equality ("and" equality)*

/// equality -> comparison ( ( "!=" | "==" ) comparison )*

/// comparison -> term ( ( ">" | ">=" | "<" | "<=" ) term )*

/// term -> factor ( ( "-" | "+" ) factor )*

/// factor -> unary ( ( "/" | "*" ) unary )*

/// unary -> ( "!" | "-" ) unary | exponent;

/// exponent -> (prefix "**" unary)
///            | prefix ;

/// prefix -> ("++" | "--") primary
///            | postfix

/// postfix -> primary ("--"|"++")* | primary "[" lambda "]" | lambda ;

/// lambda_expr -> "fun" lambda_body ;
/// lambda_body -> IDENTIFIER "(" parameters? ")" ":" typeExpr  block ;
/// parameter ->  IDENTIFIER ":" typeExpr
/// parameters -> parameter ( "," parameter )* ;
/// lambda -> lambda_expr | collection_initializer

/// call -> primary ( "(" arguments? ")" )* ;
/// arguments      → expression ( "," expression )* ;

///primary        → "true" | "false" | "nil" | "this"
///               | NUMBER | STRING | IDENTIFIER | "(" expression ")"
///               | "base" "." IDENTIFIER ;

/// declaration -> classDecl
/// 			  | funDecl
///               | varDecl
///               | statement ;

/// classDecl -> "class" IDENTIFIER "{" class_member* "}" ;

/// classMember -> funDecl | constructorDecl | var_decl

/// funDecl -> "fun" function ;
/// function -> IDENTIFIER "(" parameters? ")" ":" typeExpr  block ;
/// parameter ->  IDENTIFIER ":" typeExpr
/// parameters -> parameter ( "," parameter )* ;

/// var_stmt -> "var" IDENTIFIER (":" typeExpr)? ("=" initializer)?
// 	var_decl -> var_stmt ";"

/// type_expr ->  type_expr (PIPE non_union_type)?

/// non_union_type -> callable_type
///                | list[non_union_type]
///                | map[non_union_type,non_union_type] ;

/// callable_type -> "fun"  "(" parameter_types? ")" ":" typeExpr
/// parameter_types -> type_expr *

/// variable_type -> IDENTIFIER

/// block -> "{" declaration* "}" ;



```
