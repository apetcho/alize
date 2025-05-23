#include "alize.hpp"
#include<iomanip>
#include<set>

// -*----------------------------------------------------------------*-
// -*- begin::namespace::alz                                        -*-
// -*----------------------------------------------------------------*-
namespace alz{
// -*-
static const std::set<Str> _RESERVED_WORDS = {
    "import", "let", "var", "cond", "for",
    "progn", "if", "macro", "fun", "lambda"
};

static const std::set<Str> _RESERVED_CONSTANTS{
    "true", "false", "nil",
};

static inline bool _is_reserved_word(const Str& word){
    return _RESERVED_WORDS.find(word) != _RESERVED_WORDS.end();
}

static inline bool _is_reserved_constant(const Str& word){
    return _RESERVED_CONSTANTS.find(word) != _RESERVED_CONSTANTS.end();
}

// -*------------------*-
// -*- Identifier AST -*-
// -*------------------*-
// [_\-a-zA-Z0-9\:#~&]
IdentAst::IdentAst(Token token)
: AstBase{AstKind::Ident}
, m_token{token}{}

// -*-
Object IdentAst::eval([[maybe_unused]] Env& env){
    static const std::set<Str> _reservedWords = {
        "import", "let", "var", "cond", "for", "progn",
        "if", "macro", "fun", "lambda"
    };
    auto lexeme = this->m_token.lexeme;
    if(lexeme == "true"){ return Object(true); }
    if(lexeme == "false"){ return Object(false); }
    if(lexeme == "nil"){ return Object(); }
    if(_is_reserved_word(lexeme)){ // it is a reserved word
        return Object(Symbol(lexeme));
    }
    // it is a user-defined identifier or builtin function or constant identifier
    return env.get(lexeme);
}

// -*-
Str IdentAst::str(void) const{
    return this->m_token.lexeme;
}

// -*-
Str IdentAst::repr(void) const{
    return this->m_token.lexeme;
}

// -*-
Str IdentAst::literal(void) const{
    return this->m_token.lexeme;
}

// -*---------------*-
// -*- Integer AST -*-
// -*---------------*-
// [-+](0b[01]+)|(0o[0-7]+)|(0x[0-9]+) | "numstr"
IntegerAst::IntegerAst(Token token)
: AstBase{AstKind::Integer}
, m_token{token} {}

// -*-
Object IntegerAst::eval([[maybe_unused]] Env& env){
    auto lexeme = this->m_token.lexeme;
    i64 num{};
    try{
        num = static_cast<i64>(std::stoll(lexeme));
    }catch(std::invalid_argument& err){
        std::stringstream stream;
        stream << err.what() << "\n";
        stream << "at Line " << this->m_token.row;
        stream << " and column " << this->m_token.col;
        throw Error(Error::Kind::ValueError, stream.str());
    }catch(std::out_of_range& err){
        std::stringstream stream;
        stream << err.what() << "\n";
        stream << "at Line " << this->m_token.row;
        stream << " and column " << this->m_token.col;
        throw Error(Error::Kind::ValueError, stream.str());
    }catch(...){
        std::stringstream stream;
        stream << "unexpected error occured";
        throw Error(Error::Kind::Default, stream.str());
    }

    return Object(num);
}

// -*-
Str IntegerAst::str(void) const{
    return this->m_token.lexeme;
}

// -*-
Str IntegerAst::repr(void) const{
    std::stringstream stream;
    stream << "\"" << this->m_token.lexeme << "\"";
    return stream.str();
}

// -*-
Str IntegerAst::literal(void) const{
    return this->m_token.lexeme;
}

// -*-------------*-
// -*- Float AST -*-
// -*-------------*-
// [+-][0-9]+\.[0-9]+[eE][+-][0-9]+ | "numstr"
FloatAst::FloatAst(Token token)
: AstBase{AstKind::Float}
, m_token{token} {}

// -*-
Object FloatAst::eval([[maybe_unused]] Env& env){
    auto lexeme = this->m_token.lexeme;
    f64 num{};
    try{
        num = std::stod(lexeme);
    }catch(std::invalid_argument& err){
        std::stringstream stream;
        stream << err.what() << "\n";
        stream << "at Line " << this->m_token.row;
        stream << " and column " << this->m_token.col;
        throw Error(Error::Kind::ValueError, stream.str());
    }catch(std::out_of_range& err){
        std::stringstream stream;
        stream << err.what() << "\n";
        stream << "at Line " << this->m_token.row;
        stream << " and column " << this->m_token.col;
        throw Error(Error::Kind::ValueError, stream.str());
    }catch(...){
        std::stringstream stream;
        stream << "unexpected error occured";
        throw Error(Error::Kind::Default, stream.str());
    }

    return Object(num);
}

// -*-
Str FloatAst::str(void) const{
    return this->m_token.lexeme;
}

// -*-
Str FloatAst::repr(void) const{
    std::stringstream stream;
    stream << "\"" << this->m_token.lexeme << "\"";
    return stream.str();
}

// -*-
Str FloatAst::literal(){
    return this->m_token.lexeme;
}

/*
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


class FloatAst final: public AstBase{
public:
    ~FloatAst() = default;




private:
    Token m_token;
};

// -*- String AST -*-
// "..."
class StringAst final: public AstBase{
public:
StringAst::StringAst(Token token);
    ~StringAst() = default;
Object StringAst::eval([[maybe_unused]] Env& env) override;
Str StringAst::str(void) const override;
Str StringAst::repr(void) const override;
Str StringAst::literal(void) const;

private:
    Token m_token;
};

// -*- List AST -*-
// (list ...)
class ListAst:: final: public AstBase{
public:
ListAst::ListAst(Vec<Token> tokens);
    ~ListAst() = default;
Object ListAst::eval([[maybe_unused]] Env& env) override;
Str ListAst::str(void) const override;
Str ListAst::repr(void) const override;

private:
    Vec<Token> m_vec;
};

// -*- Lambda AST -*-
// (lambda params body)
class LambdaAst final: public AstBase{
public:
LambdaAst::LambdaAst(Vec<Token> params, Vec<Ast> asts);
    ~LambdaAst() = default;
Object LambdaAst::eval([[maybe_unused]] Env& env) override;
Str LambdaAst::str(void) const override;
Str LambdaAst::repr(void) const override;

Vec<Symbol> LambdaAst::params(void) const;
Vec<Ast> LambdaAst::body(void) const;
Env LambdaAst::scope(void) const;

private:
    Vec<Token> m_params;
    Vec<Ast> m_body;
    Env m_scope;
};

// -*- User-defined function AST -*-
// (defun name params body)
class FunAst final: public AstBase{
public:
FunAst::FunAst(Token name, Vec<Token> params, Vec<Ast> body);
    ~FunAst() = default;
Object FunAst::eval([[maybe_unused]] Env& env) override;
Str FunAst::str(void) const override;
Str FunAst::repr(void) const override;

Str FunAst::name(void) const;
Vec<Symbol> FunAst::params(void) const;
Vec<Ast> FunAst::body(void) const;
Env FunAst::scope(void) const;

private:
    Token m_name;
    Vec<Token> m_params;
    Vec<Ast> m_body;
    Env m_scope;
};

// -*- Macro AST -*-
// (defmacro name params body)
class MacroAst:: final: public AstBase{
public:
MacroAst::MacroAst(Token name, Vec<Token> params, Vec<Ast> body);
    ~MacroAst() = default;
Object MacroAst::eval([[maybe_unused]] Env& env) override;
Str MacroAst::str(void) const override;
Str MacroAst::repr(void) const override;

Str MacroAst::name(void) const;
Vec<Symbol> MacroAst::params(void) const;
Vec<Ast> MacroAst::body(void) const;
Env MacroAst::scope(void) const;
Ast MacroAst::expand(void) const;

private:
    Token m_name;
    Vec<Token> m_params;
    Vec<Ast> m_body;
    Env m_scope;
};

// -*- If AST -*-
// (if test ok alt)
class IfAst final: public AstBase{
public:
IfAst::IfAst(Ast test, Ast okay, Ast alt);
    ~IfAst() = default;
Object IfAst::eval([[maybe_unused]] Env& env) override;
Str IfAst::str(void) const override;
Str IfAst::repr(void) const override;

private:
    Ast m_test;
    Ast m_okay;
    Ast m_alt;
};

// -*- Define AST -*-
// (define name sexpr)
class VarAst final: public AstBase{
public:
VarAst::VarAst(Token name, Ast ast);
    ~VarAst() = default;
Object VarAst::eval([[maybe_unused]] Env& env) override;
Str VarAst::str(void) const override;
Str VarAst::repr(void) const override;

private:
    Token m_name;
    Ast m_ast;
};

// -*- Progn AST -*-
// (progn ...)
class PrognAst final: public AstBase{
public:
PrognAst::PrognAst(Vec<Ast> body);
    ~PrognAst() = default;
Object PrognAst::eval([[maybe_unused]] Env& env) override;
Str PrognAst::str(void) const override;
Str PrognAst::repr(void) const override;

private:
    Vec<Ast> m_body;
};

// -*- For AST -*-
// (for (x xs) body)
class ForAst final: public AstBase{
public:
ForAst::ForAst(Vec<Ast> args, Vec<Ast> body);
    ~ForAst() = default;
Object ForAst::eval([[maybe_unused]] Env& env) override;
Str ForAst::str(void) const override;
Str ForAst::repr(void) const override;

private:
    Vec<Ast> m_args; // (x xs)
    Vec<Ast> m_body; // body
};

// -*- Cond AST -*-
// (cond (tst1 ast1) (tst2 ast2) ...  )
class CondAst final: public AstBase{
public:
CondAst::CondAst(Vec<Ast> clauses);
    ~CondAst() = default;
Object CondAst::eval([[maybe_unused]] Env& env) override;
Str CondAst::str(void) const override;
Str CondAst::repr(void) const override;

private:
    Vec<Ast> m_clauses;
};

// -*--------------*-
// -*- Import AST -*-
// -*--------------*-
// (import modulename)
class ImportAst final: public AstBase{
public:

ImportAst::ImportAst(Symbol sym);
~ImportAst() = default;
Object ImportAst::eval([[maybe_unused]] Env& env) override;
Str ImportAst::str(void) const override;
Str ImportAst::repr(void) const override;

private:
    Symbol m_sym;
    fs::path m_path;
};

// -*-----------*-
// -*- Let AST -*-
// -*-----------*-
// (let ((x xval) (y yval) ...) body)
class LetAst:: final: public AstBase{
public:

LetAst::LetAst(Vec<ListAst> defs, Vec<Ast> body);
    ~LetAst() = default;
Object LetAst::eval([[maybe_unused]] Env& env) override;
Str LetAst::str(void) const override;
Str LetAst::repr(void) const override;

private:
    Vec<ListAst> m_defs;
    Vec<Ast> m_body;
};
*/

// -*----------------------------------------------------------------*-
}//-*- end::namespace::alz                                          -*-
// -*----------------------------------------------------------------*-