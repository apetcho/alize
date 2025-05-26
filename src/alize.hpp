#ifndef ALIZE_HPP
#define ALIZE_HPP

#include<stdexcept>
#include<filesystem>
#include<iostream>
#include<cstdint>
#include<fstream>
#include<sstream>
#include<variant>
#include<optional>
#include<memory>
#include<string>
#include<vector>
#include<map>

namespace fs = std::filesystem;

// -*----------------------------------------------------------------*-
// -*- begin::namespace::alz                                        -*-
// -*----------------------------------------------------------------*-
namespace alz{
// -

// -*-
class Env;
class Object;
class AstBase;
class NilAst;
class BoolAst;
class IdentAst;
class IntegerAst;
class FloatAst;
class StringAst;
class ListAst;
class LambdaAst;
class FunAst;
class MacroAst;
class IfAst;
class DefineAst;
class PrognAst;
class ForAst;
class CondAst;
class ImportAst;
class LetAst;

struct Closure;
struct Symbol;

struct Token;
class Tokenizer;    // The Tokenizer
class Parser;       // The Parser
class Alize;        // Interpreter

// -*-
template<typename T>
using Shared = std::shared_ptr<T>;
using Ast = Shared<AstBase>;
using Self = Shared<Object>;
template<typename T>
using Vec = std::vector<T>;
template<typename T>
using Option = std::optional<T>;
using f64 = double;
using usize = std::size_t;
using i64 = std::int64_t;
using u32 = std::uint32_t;
using i32 = std::int32_t;
using Str = std::string;
using CFun = Object (*)(Vec<Object>);
using ArrayList = Vec<Object>;
using Dict = std::map<Str, Object>;
using Args = Vec<Object>;

// -*-
class Error final: std::runtime_error{
public:
    enum class Kind{Default, TypeError, ValueError, SyntaxError, RuntimeError};
    explicit Error(const Str& msg);
    explicit Error(Error::Kind kind, const Str& msg);
    ~Error() = default;
    const char* what(void) const noexcept override;
    Str describe(void) const;

private:
    Error::Kind m_kind;
};

// -*-
struct Symbol final{
    Str data;
    Symbol() = default;
    Symbol(const Str& str): data{str}{}
    ~Symbol() = default;
    operator Str(){ return this->data; }
    friend bool operator==(const Symbol& lhs, const Symbol& rhs){
        return (lhs.data == rhs.data);
    }
    friend bool operator==(const Str& lhs, const Symbol& rhs){
        return (lhs == rhs.data);
    }
    friend bool operator==(const Symbol& lhs, const Str& rhs){
        return (lhs.data == rhs);
    }
};

// -*- Closure
struct Closure final{
private:
    enum Kind{Fun, Lambda, Macro};
    Kind m_kind;
    Ast m_ast;
    Shared<Env> m_env;
public:
    explicit Closure(FunAst ast, Env& env);
    explicit Closure(LambdaAst ast, Env& env);
    explicit Closure(MacroAst ast, Env& env);
    ~Closure();

    Object operator()(Vec<Object> argv, Env& env);
    bool is_function(void) const;
    bool is_lambda(void) const;
    bool is_macro(void) const;
    Str str(void) const;
    Str repr(void) const;
    Str name(void) const;
    const Ast& ast(void) const;
};

// -*----------*-
// -*- Object -*-
// -*----------*-
class Object{
    enum Kind{
        Nil, Bool, Int, Float, String, Sym, Fn, Lambda, Fun, Macro, List,
    };

#define ALIZE_VARIANTS  \
    bool, i64, f64, Str, Symbol, CFun, Closure, ArrayList

    using Value = std::variant<ALIZE_VARIANTS>;

    enum class TypeKind{
        Nil, Bool, Int, Float, String, Sym, Fn, Lambda, Fun, Macro, List,
    };
public:
    explicit Object();                          // Nil
    explicit Object(bool);                      // Boolean
    explicit Object(i64);                       // Integer
    explicit Object(f64);                       // Floating point number
    explicit Object(Str);                       // String literal
    explicit Object(Symbol);                    // Identifier
    explicit Object(Str, CFun);                 // Builtin function
    explicit Object(Closure);                   // Fun, Lambda, Macro
    explicit Object(ArrayList);                 // List
    explicit Object(const Object& other);
    Object(Object&& other);
    Object& operator=(const Object& other);
    Object& operator=(Object&& other);
    virtual ~Object(){}

    // -*- type-cast -*-
    operator bool();
    operator i64();
    operator f64();
    operator Str();
    operator Symbol();
    operator CFun();
    operator Closure();
    operator ArrayList();
    // -*- Predicates -*-
    bool is_nil(void) const;
    bool is_bool(void) const;
    bool is_integer(void) const;
    bool is_float(void) const;
    bool is_number(void) const{
        return this->is_integer() || this->is_float();
    }
    bool is_symbol(void) const;
    bool is_string(void) const;
    bool is_builtin_function(void) const;
    bool is_closure(void) const;
    bool is_list(void) const;

    // -*- stringifiers -*-
    Str str(void) const;
    Str repr(void) const;
    
    // -*- unary-operator: {-, +, ~, } -*-
    Object& operator-();
    Object& operator+();
    Object& operator~();

    // -*- binary-operator: {-, +, *, /, %, and, or,} -*-
    friend Object operator+(const Object& lhs, const Object& rhs);
    friend Object operator-(const Object& lhs, const Object& rhs);
    friend Object operator*(const Object& lhs, const Object& rhs);
    friend Object operator/(const Object& lhs, const Object& rhs);
    friend Object operator%(const Object& lhs, const Object& rhs);
    friend Object operator||(const Object& lhs, const Object& rhs);
    friend Object operator&&(const Object& lhs, const Object& rhs);

    // -*- miscelaneous methods -*-
    Symbol type(void) const;

private:
    TypeKind m_typekind;
    Option<Str> m_name{};
    Value m_value;
};

// -*-------*-
// -*- Env -*-
// -*-------*-
class Env{
public:
    explicit Env();
    explicit Env(Env* parent);
    Env(const Env& other);
    ~Env(){}
    void put(Str key, const Object& val);
    void update(Str key, const Object& val);
    Object get(Str key);
    bool contains(const Str key);
    Env*& parent(void);
private:
    Dict m_bindings;
    Env* m_parent;
};


// -*---------------------*-
// -*- Token & Tokenizer -*-
// -*---------------------*-
#define ALIZE_RESERVED_WORDS()                      \
    ALIZE_DEF(Nil, "nil")                           \
    ALIZE_DEF(True, "true")                         \
    ALIZE_DEF(False, "false")                       \
    ALIZE_DEF(If, "if")                             \
    ALIZE_DEF(Fun, "fun")                           \
    ALIZE_DEF(Var, "var")                           \
    ALIZE_DEF(Macro, "macro")                       \
    ALIZE_DEF(Lambda, "lambda")                     \
    ALIZE_DEF(For, "for")                           \
    ALIZE_DEF(Cond, "cond")                         \
    ALIZE_DEF(Progn, "progn")                       \
    ALIZE_DEF(Let, "let")                           \
    ALIZE_DEF(Import, "import")


#define ALIZE_TOKENS()                      \
    ALIZE_DEF(Undef, "UNDEF")\
    ALIZE_DEF(Eof, "EOF")                   \
    ALIZE_DEF(Ident, "IDENTIFIER")          \
    ALIZE_DEF(Integer, "INTEGER-LITERAL")   \
    ALIZE_DEF(Float, "FLOAT-LITERAL")       \
    ALIZE_DEF(String, "STRING-LITERAL")     \
    ALIZE_DEF(LParen, "(")                  \
    ALIZE_DEF(RParen, ")")                  \
    ALIZE_DEF(Colon, ":")                   \
    ALIZE_DEF(DColon, "::")                 \
    ALIZE_DEF(Dot, ".")                     \
    ALIZE_DEF(Quote, "'")                   \
    ALIZE_DEF(Unquote, ",")                 \
    ALIZE_DEF(Quasiquote, "`")              \
    ALIZE_DEF(UnquoteSplicing, ",@")        \
    ALIZE_DEF(Plus, "+")                    \
    ALIZE_DEF(Minus, "-")                   \
    ALIZE_DEF(Mul, "*")                     \
    ALIZE_DEF(Div, "/")                     \
    ALIZE_DEF(Mod, "%")                     \
    ALIZE_RESERVED_WORDS()


// ALIZE_DEF()

enum class TokenKind{
#define ALIZE_DEF(tok, _) tok,
    ALIZE_TOKENS()
#undef ALIZE_DEF
};


struct Token final{
    TokenKind kind;
    Str lexeme;
    i32 row;
    i32 col;
    explicit Token() : kind{TokenKind::Undef}, lexeme{}, row{-1}, col{-1}{}
    explicit Token(TokenKind kd, const Str& txt, i32 r=-1, i32 c=-1)
    : kind{kd}, lexeme{txt}, row{r}, col{c}{}
};


class Tokenizer final{
private:
    Str m_src;
    i32 m_cur;
    i32 m_pos;
    i32 m_row;
    i32 m_col;

public:
    Tokenizer() = default;
    explicit Tokenizer(const Str& src);
    ~Tokenizer() = default;

    Vec<Token> tokenize(void);
    static bool is_builtin_reserved_word(TokenKind kind){
        // - import, var, fun, lambda, macro, progn
        // - cond, let, if, for
        static const std::map<TokenKind, Str> _myReservedWords{
            {TokenKind::Import, "import"},
            {TokenKind::Var, "var"},
            {TokenKind::Let, "let"},
            {TokenKind::If, "if"},
            {TokenKind::For, "for"},
            {TokenKind::Cond, "cond"},
            {TokenKind::Progn, "progn"},
            {TokenKind::Fun, "fun"},
            {TokenKind::Macro, "macro"},
            {TokenKind::Lambda, "lambda"},
        };
        auto entry = _myReservedWords.find(kind);
        if(entry == _myReservedWords.end()){
            return false;
        }
        return true;
    }
    
    static bool is_builtin_constant(TokenKind kind){
        // - nil, true, false
        static const std::map<TokenKind, Str> _myConstants{
            {TokenKind::Nil, "nil"},
            {TokenKind::True, "true"},
            {TokenKind::False, "false"},
        };
        auto entry = _myConstants.find(kind);
        if(entry == _myConstants.end()){
            return false;
        }
        return true;
    }

private:
    Token next_token(void);
    Token read_identifier(void);
    // Token read_bool(void);
    // Token read_integer(void);
    // Token read_float(void);
    Token read_number(void);
    Token read_str(void);
    //Token read_list(void);
    i32 next(void);
    void advance(void);
    char peek();
    void skip_whitespace(void);
    void skip_comment(void);
    // bool match(const Str& ident);
    bool is_eof(void);
public:
    // static bool is_valid_identifier_start_char(int c);
    static bool is_valid_identifier_char(int c);
};

// -*------------------------*-
// -*- ABSTACT SYNTAX TREES -*-
// -*------------------------*-
#define ALIZE_AST_KINDS()                           \
    ALIZE_DEF(Ident, "IDENITIFIER")                 \
    ALIZE_DEF(Integer, "INTEGER")                   \
    ALIZE_DEF(Float, "FLOAT")                       \
    ALIZE_DEF(String, "STRING")                     \
    ALIZE_DEF(List, "LIST")                         \
    ALIZE_DEF(Lambda, "LAMBDA")                     \
    ALIZE_DEF(Fun, "FUN")                           \
    ALIZE_DEF(Macro, "MACRO")                       \
    ALIZE_DEF(If, "IF")                             \
    ALIZE_DEF(Var, "VAR")                           \
    ALIZE_DEF(Progn, "PROGN")                       \
    ALIZE_DEF(For, "FOR")                           \
    ALIZE_DEF(Cond, "COND")                         \
    ALIZE_DEF(Import, "IMPORT")                     \
    ALIZE_DEF(Let, "LET")


// -*-
enum class AstKind{
#define ALIZE_DEF(tok, _) tok,
    ALIZE_AST_KINDS()
#undef ALIZE_DEF
};

/*
    ALIZE_DEF(Quote, "QUOTE")                       \
    ALIZE_DEF(Unquote, "UNQUOTE")                   \
    ALIZE_DEF(Quasiquote, "QUASIQUTE")              \
    ALIZE_DEF(UnquoteSplicing, "UNQUOTE-SPLICING")  \
    ALIZE_DEF(And, "AND")                           \
    ALIZE_DEF(Or, "OR")                             \
*/

// -*- AstBase -*-
class AstBase{
public:
    AstBase(AstKind kind): m_kind{kind}{}
    virtual ~AstBase() = default;
    AstKind kind() const { return this->m_kind; }
    virtual Object eval([[maybe_unused]] Env& env) = 0;
    virtual Str str(void) const = 0;
    virtual Str repr(void) const = 0;

protected:
    AstKind m_kind;
};

// -*- Identifier AST
// [_\-a-zA-Z0-9\:#~&]
class IdentAst final: public AstBase{
public:
    explicit IdentAst(Token token);
    ~IdentAst() = default;
    Object eval([[maybe_unused]] Env& env) override;
    Str str(void) const override;
    Str repr(void) const override;
    Str literal(void) const;

private:
    Token m_token; // nil, true, false, *reserved-word*, *var-or-func-name*
};

// -*- Integer AST -*-
// [-+](0b[01]+)|(0o[0-7]+)|(0x[0-9]+)
class IntegerAst final: public AstBase{
public:
    explicit IntegerAst(Token token);
    ~IntegerAst() = default;
    Object eval([[maybe_unused]] Env& env) override;
    Str str(void) const override;
    Str repr(void) const override;
    Str literal(void) const;

private:
    Token m_token;
};

// -*- Float AST -*-
// [+-][0-9]+\.[0-9]+[eE][+-][0-9]+
class FloatAst final: public AstBase{
public:
    explicit FloatAst(Token token);
    ~FloatAst() = default;
    Object eval([[maybe_unused]] Env& env) override;
    Str str(void) const override;
    Str repr(void) const override;
    Str literal();

private:
    Token m_token;
};

// -*- String AST -*-
// "..."
class StringAst final: public AstBase{
public:
    explicit StringAst(Token token);
    ~StringAst() = default;
    Object eval([[maybe_unused]] Env& env) override;
    Str str(void) const override;
    Str repr(void) const override;
    Str literal(void) const;

private:
    Token m_token;
};

// -*- List AST -*-
// (list ...)
class ListAst final: public AstBase{
public:
    explicit ListAst(Vec<Token> tokens);
    ~ListAst() = default;
    Object eval([[maybe_unused]] Env& env) override;
    Str str(void) const override;
    Str repr(void) const override;
    Vec<Token> tokens(void) const{ return this->m_vec; }

private:
    Vec<Token> m_vec;
};

// -*- Lambda AST -*-
// (lambda params body)
class LambdaAst final: public AstBase{
public:
    explicit LambdaAst(Vec<Token> params, Vec<Ast> asts);
    ~LambdaAst() = default;
    Object eval([[maybe_unused]] Env& env) override;
    Str str(void) const override;
    Str repr(void) const override;

    Vec<Symbol> params(void) const;
    Vec<Ast> body(void) const;
    // Env scope(void) const;

private:
    Vec<Token> m_params;
    Vec<Ast> m_body;
    // Env m_scope;
};

// -*- User-defined function AST -*-
// (defun name params body)
class FunAst final: public AstBase{
public:
    explicit FunAst(Token name, Vec<Token> params, Vec<Ast> body);
    ~FunAst() = default;
    Object eval([[maybe_unused]] Env& env) override;
    Str str(void) const override;
    Str repr(void) const override;

    Str name(void) const;
    Vec<Symbol> params(void) const;
    Vec<Ast> body(void) const;
    // Env scope(void) const;

private:
    Token m_name;
    Vec<Token> m_params;
    Vec<Ast> m_body;
    // Env m_scope;
};

// -*- Macro AST -*-
// (defmacro name params body)
class MacroAst final: public AstBase{
public:
    explicit MacroAst(Token name, Vec<Token> params, Vec<Ast> body);
    ~MacroAst() = default;
    Object eval([[maybe_unused]] Env& env) override;
    Str str(void) const override;
    Str repr(void) const override;

    Str name(void) const;
    Vec<Symbol> params(void) const;
    Vec<Ast> body(void) const;
    // Env scope(void) const;
    Ast expand(void) const; // return a closure (<function>) object

private:
    Token m_name;
    Vec<Token> m_params;
    Vec<Ast> m_body;
    // Env m_scope;
};

// -*- If AST -*-
// (if test ok alt)
class IfAst final: public AstBase{
public:
    explicit IfAst(Ast test, Ast okay, Ast alt);
    ~IfAst() = default;
    Object eval([[maybe_unused]] Env& env) override;
    Str str(void) const override;
    Str repr(void) const override;

private:
    Ast m_test;
    Ast m_okay;
    Ast m_alt;
};

// -*- Define AST -*-
// (define name sexpr)
class VarAst final: public AstBase{
public:
    explicit VarAst(Token name, Ast ast);
    ~VarAst() = default;
    Object eval([[maybe_unused]] Env& env) override;
    Str str(void) const override;
    Str repr(void) const override;

private:
    Token m_name;
    Ast m_ast;
};

// -*- Progn AST -*-
// (progn ...)
class PrognAst final: public AstBase{
public:
    explicit PrognAst(Vec<Ast> body);
    ~PrognAst() = default;
    Object eval([[maybe_unused]] Env& env) override;
    Str str(void) const override;
    Str repr(void) const override;

private:
    Vec<Ast> m_body;
};

// -*- For AST -*-
// (for (x xs) body)
class ForAst final: public AstBase{
public:
    explicit ForAst(Vec<Ast> args, Vec<Ast> body);
    ~ForAst() = default;
    Object eval([[maybe_unused]] Env& env) override;
    Str str(void) const override;
    Str repr(void) const override;

private:
    Vec<Ast> m_args; // (x xs)
    Vec<Ast> m_body; // body
};

// -*- Cond AST -*-
// (cond (tst1 ast1) (tst2 ast2) ...  )
class CondAst final: public AstBase{
public:
    explicit CondAst(Vec<Ast> clauses);
    ~CondAst() = default;
    Object eval([[maybe_unused]] Env& env) override;
    Str str(void) const override;
    Str repr(void) const override;

private:
    Vec<Ast> m_clauses;
};

// -*--------------*-
// -*- Import AST -*-
// -*--------------*-
// (import modulename)
class ImportAst final: public AstBase{
public:
    explicit ImportAst(Symbol sym);
    ~ImportAst() = default;
    Object eval([[maybe_unused]] Env& env) override;
    Str str(void) const override;
    Str repr(void) const override;

private:
    Symbol m_sym;
    fs::path m_path;
};

// -*-----------*-
// -*- Let AST -*-
// -*-----------*-
// (let ((x xval) (y yval) ...) body)
class LetAst final: public AstBase{
public:
    explicit LetAst(Vec<ListAst> defs, Vec<Ast> body);
    ~LetAst() = default;
    Object eval([[maybe_unused]] Env& env) override;
    Str str(void) const override;
    Str repr(void) const override;

private:
    Vec<ListAst> m_defs;
    Vec<Ast> m_body;
};


// -*- Parser -*-
class Parser final{
private:
    Tokenizer m_tokenizer;

public:
    Parser() = default;
    explicit Parser(const Str& src);
    explicit Parser(std::stringstream* stream);
    Object parse(void);

private:
    Object parse_atom(void); // nil, true, false, integer, float, string
    Object parse_list(void);
    Object parse_fun(void);
    Object parse_macro(void);
    Object parse_define(void);
    Object parse_lambda(void);
    Object parse_progn(void);
    Object parse_if(void);
    Object parse_cond(void);
    Object parse_import(void);

    bool expect(AstKind kind);
};

// -*-
namespace builtins{
// -*-
    Object fn_quote(Args argv);
    Object fn_quasiquote(Args argv);
    Object fn_unquote(Args argv);
    Object fn_unquote_splicing(Args argv);
    Object fn_and(Args argv);
    Object fn_or(Args argv);
    Object fn_not(Args argv);

    Object fn_add(Args argv);
    Object fn_sub(Args argv);
    Object fn_mul(Args argv);
    Object fn_mod(Args argv);
    Object fn_div(Args argv);

    Object fn_len(Args argv);
    Object fn_get(Args argv);
    Object fn_insert(Args argv);
    Object fn_push(Args argv);
    Object fn_pop(Args argv);
    Object fn_index(Args argv);

    Object fn_ltrim(Args argv);
    Object fn_rtrim(Args argv);
    Object fn_trim(Args argv);
    Object fn_lower(Args argv);
    Object fn_upper(Args argv);
    Object fn_replace(Args argv);
    Object fn_join(Args argv);
    Object fn_split(Args argv);
    Object fn_startswith(Args argv);
    Object fn_endswith(Args argv);

    Object fn_addpath(Args argv);
    Object fn_println(Args argv);
    Object fn_eprintln(Args argv);
    Object fn_print(Args argv);
    Object fn_eprint(Args argv);
    Object fn_panic(Args argv);
    Object fn_format(Args argv);
    Object fn_random(Args argv);
    Object fn_filter(Args argv);
    Object fn_map(Args argv);
    Object fn_reduce(Args argv);
    Object fn_zip(Args argv);

    // -*- math -*-
    Object fn_abs(Args argv);
    Object fn_max(Args argv);
    Object fn_min(Args argv);
    Object fn_floor(Args argv);
    Object fn_ceil(Args argv);
    Object fn_round(Args argv);
    Object fn_trunc(Args argv);
    Object fn_sin(Args argv);
    Object fn_cos(Args argv);
    Object fn_tan(Args argv);
    Object fn_asin(Args argv);
    Object fn_acos(Args argv);
    Object fn_atan(Args argv);
    Object fn_atan2(Args argv);
    Object fn_sinh(Args argv);
    Object fn_cosh(Args argv);
    Object fn_tanh(Args argv);
    Object fn_asinh(Args argv);
    Object fn_acosh(Args argv);
    Object fn_atanh(Args argv);
    Object fn_erf(Args argv);
    Object fn_erfc(Args argv);
    Object fn_gamma(Args argv);
    Object fn_lgamma(Args argv);
    Object fn_pow(Args argv);
    Object fn_sqrt(Args argv);
    Object fn_cbrt(Args argv);
    Object fn_exp(Args argv);
    Object fn_exp2(Args argv);
    Object fn_expm1(Args argv);
    Object fn_log(Args argv);
    Object fn_log2(Args argv);
    Object fn_log1p(Args argv);
    Object fn_fma(Args argv);

// -*-
}

// -*---------*-
// -*- Alize -*-
// -*---------*-
class Alize{
public:
    Alize();
    ~Alize() = default;

    static void run(Vec<Str> argv);
    static void repl(Vec<Str> argv);

    static Env runtime;
    static void initialize(void);

    // -*-
    static bool almostEqual(f64 x, f64 y){
        constexpr f64 myMin = std::numeric_limits<f64>::min();
        constexpr f64 myErr = 1.0e-13;
        if(!std::isfinite(x) || !std::isfinite(y)){
            return false;
        }
        auto diff = std::abs(x-y);
        if(diff < myMin){ return true; }
        auto xa = std::abs(x);
        auto ya = std::abs(y);
        auto xymax = std::max(xa, ya);
        return (diff/xymax) <= myErr;
    }

private:
    static Dict prelude;
    static void initialize_prelude(void);
};


// -*----------------------------------------------------------------*-
}//-*- end::namespace::alz                                          -*-
// -*----------------------------------------------------------------*-

#endif