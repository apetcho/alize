#include "alize.hpp"
#include<iomanip>
#include<stack>
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

// -*-
Str MacroAst::name(void) const{
    return this->m_name.lexeme;
}

// -*-
Vec<Symbol> MacroAst::params(void) const{
    Vec<Symbol> result{};
    for(const auto& token: this->m_params){
        auto lexeme = token.lexeme;
        if(!_is_identifier(lexeme)){
            std::stringstream stream;
            stream << "invalid macro parameter `" << lexeme << "'";
            throw Error(Error::Kind::Default, stream.str());
        }
        result.emplace_back(lexeme);
    }
    return result;
}

// -*-
Vec<Ast> MacroAst::body(void) const{
    return this->m_body;
}

// -*-
/**
 * @brief Macro::expand()
 * Expand the body of the macro by processing `quasiquote` and other.
 * 
 * The expanded body is now a list of AST suitable as a regular function body.
 * 
 * @return Vec<Ast> 
 */
Vec<Ast> MacroAst::expand(void) const{
    //! @todo
    // (01) Handle `quote`              <==> 'arg
    // (02) Handle `unquote`            <==> ,arg
    // (03) Handle `quasiquote`         <==> `arg
    // (04) Handle `unquote-splicing`   <==> ,@arg
    Vec<Ast> result{};
    for(const auto& ast: this->body()){
        // `ast' can be a list AST or an `atom` AST i.e
        //  -> identifiers (true, false, nil, <<builtinSymbols>>, <<userDefinedSymbols>>)
        //  -> numbers, or string
        //! @warning: we do not support `import`, `macro` and `fun` as symbol in the body
        //            of a macro
        if(ast->kind()==AstKind::List){ // a list AST
            auto self = dynamic_cast<ListAst*>(ast.get());
            if(self == nullptr){
                throw Error(Error::Kind::RuntimeError, "unexpected error occured");
            }
            auto tokens = self->tokens(); // '(', ..., ')'
            if(tokens.size() > 2){
                // CASES:
                //  (1) (quote arg)
                //  (2) (unquote arg)
                //  (3) (quasiquote arg)
                //  (4) (unquote-splicing arg)
                //  (5) (<function-like-identifier> ...)
                //   Otherwise an error has occurred

                // Expecte first-argument to be an identifier
                if(!Tokenizer::is_identifier(tokens[1].kind)){
                    std::stringstream stream;
                    stream << "malformed macro `" << this->name() << "' at ";
                    stream << "line " << this->m_name.row << " and column " << this->m_name.col;
                    throw Error(Error::Kind::SyntaxError, stream.str());
                }
                auto ident = tokens[1].lexeme;
                std::set<Str> quotes{"quote", "unquote", "unquote-splicing", "quasiquote"};
                if(quotes.find(ident) != quotes.end()){
                    // We're handling quotings
                    // (_quoting_ident_ __arg__)
                    if((ident=="unquote-splicing") && (tokens[3].kind!=TokenKind::LParen)){
                        // We expect the argument to be a list
                        std::stringstream stream;
                        stream << "malformed macro `" << this->name() << "' at ";
                        stream << "line " << tokens[3].row << " and column " << tokens[3].col << ".";
                        stream << "Expected a list as argument to `unquote-splicing`";
                        throw Error(Error::Kind::SyntaxError, stream.str());
                    }else if((ident=="unquote-splicing") && (tokens[3].kind==TokenKind::LParen)){
                        // let collect the item in the list
                        std::stack<TokenKind> myArgs{};
                        myArgs.push(TokenKind::LParen);
                        Vec<Token> _args_{};
                        for(auto i=4; i < tokens.size()-1; i++){
                            if(myArgs.empty()){ break;}
                            auto _tok = tokens[i];
                            if(_tok.kind==TokenKind::LParen){
                                myArgs.push(TokenKind::LParen);
                                continue;
                            }
                            if(_tok.kind==TokenKind::RParen){
                                myArgs.pop();
                                continue;
                            }
                            _args_.push_back(_tok);
                        }
                        if(!myArgs.empty()){
                            std::stringstream stream;
                            stream << "malformed macro `" << this->name() << "'";
                            throw Error(Error::Kind::SyntaxError, stream.str());
                        }
                        // Now, each element in `_args_` list should be unquoted
                        Vec<Token> __args__{};
                        for(const auto& elem: _args_){
                            __args__.push_back(Token{TokenKind::LParen, "("});
                            __args__.push_back(Token{TokenKind::Unquote, "unquote"});
                            __args__.push_back(elem);
                            __args__.push_back(Token{TokenKind::RParen, ")"});
                        }
                        result.push_back(std::make_shared<ListAst>(__args__));
                        continue;
                    }else{
                        Vec<Token> _items{};
                        _items.push_back(Token{TokenKind::LParen, "("});
                        if(ident == "quote"){
                            _items.push_back(Token{TokenKind::Quote, "quote"});
                        }else if(ident == "unquote"){
                            _items.push_back(Token{TokenKind::Unquote, "unquote"});
                        }else if(ident == "quasiquote"){
                            _items.push_back(Token{TokenKind::Quasiquote, "quasiquote"});
                        }else{
                            std::stringstream stream;
                            stream << "malformed macro `" << this->name() << "' at ";
                            stream << "line " << this->m_name.row << " at column " << this->m_name.col;
                            throw Error(Error::Kind::SyntaxError, stream.str());
                        }
                        for(auto i=2; i < tokens.size()-1; i++){
                            _items.push_back(tokens[i]);
                        }
                        _items.push_back(Token{TokenKind::RParen, ")"});
                        result.push_back(std::make_shared<ListAst>(_items));
                        continue;                      
                    }
                }else{
                    Vec<Token> _items{};
                    _items.push_back(Token{TokenKind::LParen, "("});
                    _items.push_back(Token{TokenKind::Ident, ident});
                    for(auto i=2; i < tokens.size()-1; i++){
                        _items.push_back(tokens[i]);
                    }
                    _items.push_back(Token{TokenKind::RParen, ")"});
                    result.push_back(std::make_shared<ListAst>(_items));
                    continue;
                }
            }else if(tokens.size() == 2){ // ()  ==> nil
                auto token = tokens[0];
                if(tokens[0].kind != TokenKind::LParen && tokens[1].kind != TokenKind::RParen){
                    std::stringstream stream;
                    stream << "unexpected error occured in macro `" << this->name() << "' ";
                    stream << "at line " << token.row << " and column " << token.row;
                    throw Error(Error::Kind::RuntimeError, stream.str());
                }
                result.push_back(std::make_shared<ListAst>(ListAst{tokens}));
            }else{
                std::stringstream stream;
                stream << "unexpected error occured in macro `" << this->name() << "' ";
                stream << "at line " << tokens[0].row << " and column " << tokens[0].col;
                throw Error(Error::Kind::SyntaxError, stream.str());
            }
        }else{
            result.push_back(std::move(ast));
        }
    }

    return result;
}

// -*----------*-
// -*- If AST -*-
// -*----------*-
// (if test ok alt)
IfAst::IfAst(Ast test, Ast okay, Ast alt)
: AstBase{AstKind::If}
, m_test{std::move(test)}
, m_okay{std::move(okay)}
, m_alt{std::move(alt)}
{}

// -*-
Object IfAst::eval([[maybe_unused]] Env& env){
    auto test = static_cast<bool>(this->m_test->eval(env));
    if(test){
        return this->m_okay->eval(env);
    }
    return this->m_alt->eval(env);
}

// -*-
Str IfAst::str(void) const{
    std::stringstream stream;
    stream << "(if " << this->m_test->str();
    stream << this->m_okay->str() << " ";
    stream << this->m_alt->str() << ")";
    return stream.str();
}

// -*-
Str IfAst::repr(void) const{
    std::stringstream stream;
    stream << "(if " << this->m_test->repr();
    stream << this->m_okay->repr() << " ";
    stream << this->m_alt->repr() << ")";
    return stream.str();
}

// -*--------------*-
// -*- Define AST -*-
// -*--------------*-
// (var name sexpr)
VarAst::VarAst(Token name, Ast ast)
: AstBase{AstKind::Var}
, m_name{name}, m_value{std::move(ast)}
{}

// -*-
Object VarAst::eval([[maybe_unused]] Env& env){
    if(this->m_value == nullptr){ return Object(); }
    return this->m_value->eval(env);
}

Str VarAst::str(void) const{
    if(this->m_value == nullptr){ return "nil"; }
    return this->m_value->str();
}

// -*-
Str VarAst::repr(void) const{
    if(this->m_value == nullptr){ return "\"nil\""; }
    return this->m_value->repr();
}

// -*-------------*-
// -*- Progn AST -*-
// -*-------------*-
// (progn ...)
PrognAst::PrognAst(Vec<Ast> body)
: AstBase{AstKind::Progn}
, m_body{std::move(body)}{}

// -*-
Object PrognAst::eval([[maybe_unused]] Env& env){
    Object result{};
    for(const auto ast: this->m_body){
        result = ast->eval(env);
    }
    return result;
}

// -*-
Str PrognAst::str(void) const{
    std::stringstream stream;
    stream << "(progn\n";
    for(const auto& ast: this->m_body){
        stream << ast->str();
    }
    stream << "\n)";
    return stream.str();
}

// -*-
Str PrognAst::repr(void) const{
    std::stringstream stream;
    stream << "(progn\n";
    for(const auto& ast: this->m_body){
        stream << ast->repr();
    }
    stream << "\n)";
    return stream.str();
}

// -*-----------*-
// -*- For AST -*-
// -*-----------*-
// (for (x xs) body)
ForAst::ForAst(Token var, ListAst iterable, Vec<Ast> body)
: AstBase{AstKind::For}
, m_var{var}
, m_iterable{iterable}
, m_body{std::move(body)}
{}

// -*-
Object ForAst::eval([[maybe_unused]] Env& env){
    Object result{};
    Env ctx(&env);
    if(this->m_var.kind != TokenKind::Ident){
        throw Error(Error::Kind::SyntaxError, "malformed `for` expression");
    }
    ctx.put(this->m_var.lexeme, Object());
    auto self = this->m_iterable.eval(env);
    if(!self.is_list()){
        throw Error(Error::Kind::SyntaxError, "malformed `for` expression");
    }
    auto vec = static_cast<Args>(self);
    for(const auto& elem: vec){
        ctx.update(this->m_var.lexeme, elem);
        for(const auto& x: this->m_body){
            result = x->eval(ctx);
        }
    }

    return result;
}

// -*-
Str ForAst::str(void) const{
    std::stringstream stream;
    stream << "(for (" << this->m_var.lexeme << " ";
    auto tokens = this->m_iterable.tokens();
    for(const auto& tok: tokens){
        stream << tok.lexeme << " ";
    }
    stream << ")\n";
    for(const auto& ast: this->m_body){
        stream << "    " << ast->str() << "\n";
    }
    stream << ")";

    return stream.str();
}

// -*-
Str ForAst::repr(void) const{
    std::stringstream stream;
    stream << "(for (" << this->m_var.lexeme << " ";
    auto tokens = this->m_iterable.tokens();
    for(const auto& tok: tokens){
        stream << tok.lexeme << " ";
    }
    stream << ")\n";
    for(const auto& ast: this->m_body){
        stream << "    " << ast->repr() << "\n";
    }
    stream << ")";

    return stream.str();
}

// -*------------*-
// -*- Cond AST -*-
// -*------------*-
// (cond 
//      (tst1 ast1)
//      (tst2 ast2)
//       ...  )
CondAst::CondAst(Vec<std::pair<Ast, Ast>> clauses)
: AstBase{AstKind::Cond}
, m_clauses{std::move(clauses)}
{}

// -*-
Object CondAst::eval([[maybe_unused]] Env& env){
    Object result{};
    for(const auto& clause: this->m_clauses){
        auto test = clause.first;
        auto self = clause.second;
        if(test->eval(env)){
            result = self->eval(env);
            break;
        }
    }

    return result;
}

// -*-
Str CondAst::str(void) const{
    std::stringstream stream;
    stream << "(cond\n";
    for(const auto& clause: this->m_clauses){
        stream << "(" << clause.first->str() << " " << clause.second->str() << ")\n";
    }
    stream << ")";

    return stream.str();
}

// -*-
Str CondAst::repr(void) const{
    std::stringstream stream;
    stream << "(cond\n";
    for(const auto& clause: this->m_clauses){
        stream << "(" << clause.first->repr() << " " << clause.second->repr() << ")\n";
    }
    stream << ")";

    return stream.str();
}

// -*--------------*-
// -*- Import AST -*-
// -*--------------*-
// (import modulename)
ImportAst::ImportAst(Symbol sym)
: AstBase{AstKind::Import}
, m_sym{sym}{}

// -*-
Object ImportAst::eval([[maybe_unused]] Env& env){
    auto name = this->m_sym.data;
    auto entry = Alize::modules.find(name);
    if(entry == Alize::modules.end()){
        std::stringstream stream;
        stream << "Module `" << name << "' is not available on this system.";
        throw Error(Error::Kind::RuntimeError, stream.str());
    }
    auto self = entry->second;
    auto bindings = self.eval().bindings();
    // populate the current environment
    for(auto [key, val]: bindings){
        env.put(key, val);
    }
    return Object(); // nil
}

// -*-
Str ImportAst::str(void) const{
    std::stringstream stream;
    stream << "(import " << this->m_sym.data << ")";
    return stream.str();
}

// -*-
Str ImportAst::repr(void) const{
    std::stringstream stream;
    stream << "(import " << this->m_sym.data << ")";
    return stream.str();
}

// -*-----------*-
// -*- Let AST -*-
// -*-----------*-
// (let ((x xval) (y yval) ...) body)
LetAst::LetAst(Vec<ListAst> defs, Vec<Ast> body)
: AstBase{AstKind::Let}
, m_defs{std::move(defs)}
, m_body{std::move(body)}
{}


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


class LetAst:: final: public AstBase{
public:


    ~LetAst() = default;
Object LetAst::eval([[maybe_unused]] Env& env){}
Str LetAst::str(void) const{}
Str LetAst::repr(void) const{}

private:
    Vec<ListAst> m_defs;
    Vec<Ast> m_body;
};
*/

// -*----------------------------------------------------------------*-
}//-*- end::namespace::alz                                          -*-
// -*----------------------------------------------------------------*-