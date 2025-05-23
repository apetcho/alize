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

static const std::set<Str> _RESERVED_CONSTANTS = {
    "true", "false", "nil",
};

// -*-
static const std::set<Str> _OPERATORS = {
    "+", "-", "*", "/", "%", "<", "<=", ">", ">=", "=="
};

static inline bool _is_reserved_word(const Str& word){
    return _RESERVED_WORDS.find(word) != _RESERVED_WORDS.end();
}

static inline bool _is_reserved_constant(const Str& word){
    return _RESERVED_CONSTANTS.find(word) != _RESERVED_CONSTANTS.end();
}

static inline bool _is_identifier(const Str& word){
    if(word.length()==0){ return false; }
    if(_OPERATORS.find(word) == _OPERATORS.end()){
        return false;
    }
    for(auto idx = 0; idx < word.size(); idx++){
        if(!Tokenizer::is_valid_identifier_char(word[idx])){
            return false;
        }
    }
    return true;
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

// -*--------------*-
// -*- String AST -*-
// -*--------------*-
StringAst::StringAst(Token token)
: AstBase{AstKind::String}
, m_token{token}{}

// -*-
Object StringAst::eval([[maybe_unused]] Env& env){
    return Object(this->m_token.lexeme);
}

// -*-
Str StringAst::str(void) const{
    return this->m_token.lexeme;
}

// -*-
Str StringAst::repr(void) const{
    return this->m_token.lexeme;
}

// -*-
Str StringAst::literal(void) const{
    return this->m_token.lexeme;
}

// -*------------*-
// -*- List AST -*-
// -*------------*-
// (list ...)
ListAst::ListAst(Vec<Token> tokens)
: AstBase{AstKind::List}
, m_vec{tokens}{}

// -*-
Object ListAst::eval([[maybe_unused]] Env& env){ 
    Vec<Object> vec{};
    for(auto idx = 0; idx < this->m_vec.size(); idx++){
        Token token = this->m_vec[idx];
        Object self{};
        switch(token.kind){
        case TokenKind::Nil:
            self = Object();
            break;
        case TokenKind::False:
            self = Object(false);
            break;
        case TokenKind::True:
            self = Object(true);
            break;
        case TokenKind::Ident:
            self = Object(Symbol(token.lexeme));
            break;
        case TokenKind::Integer:
            self = IntegerAst(token).eval(env);
            break;
        case TokenKind::Float:
            self = FloatAst(token).eval(env);
            break;
        case TokenKind::String:
            self = StringAst(token).eval(env);
            break;
        case TokenKind::LParen:{
                Vec<Token> items{};
                ++idx;
                while(this->m_vec[idx].kind != TokenKind::RParen){
                    items.push_back(this->m_vec[idx]);
                }
                self = ListAst(items).eval(env);
            }
            break;
        default:{
                std::stringstream stream;
                auto row = this->m_vec[0].row;
                auto col = this->m_vec[0].col;
                stream << "malformed list at line " << row;
                stream << " and column " << col;
                throw Error(Error::Kind::Default, stream.str());
            }
            break;
        }
        vec.push_back(self);
    }

    return Object(vec);
}

// -*-
Str ListAst::str(void) const{
    if(this->m_vec.size()==0){
        return "'()";
    }
    std::stringstream stream;
    stream << "(";
    auto len = this->m_vec.size();
    for(auto idx=0; idx < len; idx++){
        stream << this->m_vec[idx].lexeme;
        if(idx < (len-1)){
            stream << " ";
        }
    }
    stream << ")";
    return stream.str();
}

// -*-
Str ListAst::repr(void) const{
    if(this->m_vec.size()==0){
        return "'()";
    }
    std::stringstream stream;
    stream << "(";
    auto len = this->m_vec.size();
    for(auto idx=0; idx < len; idx++){
        auto lexeme = this->m_vec[idx].lexeme;
        if(this->m_vec[idx].kind == TokenKind::String){
            stream << "\"" << lexeme << "\"";
        }else{
            stream << lexeme;
        }
        if(idx < (len-1)){
            stream << " ";
        }
    }
    stream << ")";
    return stream.str();
}

// -*--------------*-
// -*- Lambda AST -*-
// -*--------------*-
// (lambda params body)
LambdaAst::LambdaAst(Vec<Token> params, Vec<Ast> asts)
: AstBase{AstKind::Lambda}
, m_params{params}
, m_body{asts}{}

// -*-
Object LambdaAst::eval([[maybe_unused]] Env& env){
    return Object(Closure(*this, env));
}

// -*-
Str LambdaAst::str(void) const{
    std::stringstream stream;
    stream << "lambda at 0x" << &this->m_body;
    return stream.str();
}

// -*-
Str LambdaAst::repr(void) const{
    std::stringstream stream;
    stream << "(lambda ";
    auto params = ListAst(this->m_params);
    stream << params.repr();
    for(const auto& ast: this->m_body){
        stream << ast->repr();
    }
    stream << ")";
    return stream.str();
}

// -*-
Vec<Symbol> LambdaAst::params(void) const{
    Vec<Symbol> result{};
    for(const auto& token: this->m_params){
        auto lexeme = token.lexeme;
        if(!_is_identifier(lexeme)){
            std::stringstream stream;
            stream << "invalid parameter `" << lexeme << "' to lambda";
            throw Error(Error::Kind::Default, stream.str());
        }
        result.emplace_back(lexeme);
    }
    return result;
}

// -*-
Vec<Ast> LambdaAst::body(void) const{
    return this->m_body;
}

// -*-----------------------------*-
// -*- User-defined function AST -*-
// -*-----------------------------*-
// (defun name params body)
FunAst::FunAst(Token name, Vec<Token> params, Vec<Ast> body)
: AstBase{AstKind::Fun}
, m_name{name}
, m_params{params}
, m_body{body}{}

// -*-
Object FunAst::eval([[maybe_unused]] Env& env){
    return Object(Closure(*this, env));
}

Str FunAst::str(void) const{
    std::stringstream stream;
    stream << "Function `" << this->m_name.lexeme << "' at 0x";
    stream << std::hex << &this->m_body;
    return stream.str();
}

// -*-
Str FunAst::repr(void) const{
    std::stringstream stream;
    stream << "(fun " << this->m_name.lexeme;
    auto params = ListAst(this->m_params);
    stream << params.repr();
    for(const auto& ast: this->m_body){
        stream << ast->repr();
    }
    stream << ")";
    return stream.str();
}

// -*-
Str FunAst::name(void) const{
    return this->m_name.lexeme;
}

// -*-
Vec<Symbol> FunAst::params(void) const{
    Vec<Symbol> result{};
    for(const auto& token: this->m_params){
        auto lexeme = token.lexeme;
        if(!_is_identifier(lexeme)){
            std::stringstream stream;
            stream << "invalid function parameter `" << lexeme << "'";
            throw Error(Error::Kind::Default, stream.str());
        }
        result.emplace_back(lexeme);
    }
    return result;
}

// -*-
Vec<Ast> FunAst::body(void) const{
    return this->m_body;
}

// -*-------------*-
// -*- Macro AST -*-
// -*-------------*-
// (defmacro name params body)
MacroAst::MacroAst(Token name, Vec<Token> params, Vec<Ast> body)
: AstBase{AstKind::Macro}
, m_name{name}
, m_params{params}
, m_body{body}{}

// -*-
Object MacroAst::eval([[maybe_unused]] Env& env){
    return Object(Closure(*this, env));
}

// -*-
Str MacroAst::str(void) const{
    std::stringstream stream;
    stream << "macro `" << this->m_name.lexeme << "' at 0x";
    stream << std::hex << &this->m_body;
    return stream.str();
}

// -*-
Str MacroAst::repr(void) const{
    std::stringstream stream;
    stream << "(macro " << this->m_name.lexeme;
    auto params = ListAst(this->m_params);
    stream << params.repr();
    for(const auto& ast: this->m_body){
        stream << ast->repr();
    }
    stream << ")";
    return stream.str();    
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


class MacroAst:: final: public AstBase{
public:
    ~MacroAst() = default;



Str MacroAst::name(void) const{}
Vec<Symbol> MacroAst::params(void) const{}
Vec<Ast> MacroAst::body(void) const{}
// Env MacroAst::scope(void) const{}
Ast MacroAst::expand(void) const{}

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