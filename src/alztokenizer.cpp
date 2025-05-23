#include "alize.hpp"
#include<cctype>

// -*----------------------------------------------------------------*-
// -*- begin::namespace::alz                                        -*-
// -*----------------------------------------------------------------*-
namespace alz{
// -*-
Tokenizer::Tokenizer(const Str& src)
: m_src{src}
, m_cur{'\0'}
, m_pos{usize{}}
, m_row{1}, m_col{1}{}

// -*-
Vec<Token> Tokenizer::tokenize(void){
    Vec<Token> tokens{};
    auto token = this->next_token();
    while(token.kind != TokenKind::Eof){
        tokens.push_back(token);
        token = this->next_token();
    }
    return tokens;
}

// -*-
Token Tokenizer::next_token(void){
    auto c = this->next();
    while(std::isspace(c) || !this->is_eof()){
        c = this->next();
    }
    if(c == ';'){this->skip_comment(); }
    if(this->is_eof()){ return Token(TokenKind::Eof, ""); }
    Token token{};
    switch(c){
    case ',':   // unquote, unquote-splicing ',@'
        if(this->m_src[this->m_pos+1] == '@'){
            [[maybe_unused]] auto _ = this->next();
            token.kind = TokenKind::UnquoteSplicing;
            token.lexeme = ",@";
            token.row = this->m_row;
            token.col = this->m_col;
        }else{
            token.kind = TokenKind::Unquote;
            token.lexeme = ",";
            token.row = this->m_row;
            token.col = this->m_col;
        }
        break;
    case '\'':  // quote
        token.kind = TokenKind::Quote;
        token.lexeme = "'";
        token.row = this->m_row;
        token.col = this->m_col;
        break;
    case '`':   // quasiquote
        token.kind = TokenKind::Quasiquote;
        token.lexeme = "`";
        token.row = this->m_row;
        token.col = this->m_col;
        break;
    case '"':{   // string-literal, processing escape-characters
            auto self = this->read_str();
            token.kind = TokenKind::String;
            token.lexeme = self.lexeme;
            token.row = self.row;
            token.col = self.col;
        }
        break;
    case '.':   // dot
        token.kind = TokenKind::Dot;
        token.lexeme = ",";
        token.row = this->m_row;
        token.col = this->m_col;
        break;
    case '\\':{  // escape-character
            auto _c = this->peek();
            switch(_c){
            case 'n':
                this->advance();
                token.kind = TokenKind::String;
                token.lexeme = "\n";
                token.row = this->m_row;
                token.col = this->m_col;
                break;
            case 't':
                this->advance();
                token.kind = TokenKind::String;
                token.lexeme = "\t";
                token.row = this->m_row;
                token.col = this->m_col;
                break;
            case 'f':
                this->advance();
                token.kind = TokenKind::String;
                token.lexeme = "\f";
                token.row = this->m_row;
                token.col = this->m_col;
                break;
            case 'r':
                this->advance();
                token.kind = TokenKind::String;
                token.lexeme = "\r";
                token.row = this->m_row;
                token.col = this->m_col;
                break;
            case '\\':
                this->advance();
                token.kind = TokenKind::String;
                token.lexeme = "\\";
                token.row = this->m_row;
                token.col = this->m_col;
                break;
            default:{
                    std::stringstream stream;
                    stream << "invalid escape-character at Line ";
                    stream << this->m_row << " and column " << this->m_col;
                    throw Error(Error::Kind::SyntaxError, stream.str());
                }
                break;
            }
        }
        break;
    case ':':   // Colon, DColon
        if(this->peek()==':'){
            this->advance();
            token.kind = TokenKind::DColon;
            token.lexeme = "::";
            token.row = this->m_row;
            token.col = this->m_col;
        }else{
            auto self = this->read_identifier();
            token.kind = self.kind;
            token.lexeme = self.lexeme;
            token.row = self.row;
            token.col = self.col;
        }
        break;
    case '+':{   // Plus
        if(std::isdigit(this->peek())){
                auto self = this->read_number();
                token.kind = self.kind;
                token.lexeme = self.lexeme;
                token.row = self.row;
                token.col = self.col;
            }
            else{
                token.kind = TokenKind::Minus;
                token.lexeme = "+";
                token.row = this->m_row;
                token.col = this->m_col;
            }
        }
        break;
    case '-':{   // Minus
            if(std::isdigit(this->peek())){
                auto self = this->read_number();
                token.kind = self.kind;
                token.lexeme = Str{"-"} + self.lexeme;
                token.row = self.row;
                token.col = self.col;
            }
            else{
                token.kind = TokenKind::Minus;
                token.lexeme = "-";
                token.row = this->m_row;
                token.col = this->m_col;
            }
        }
        break;
    case '/':   // Div
        token.kind = TokenKind::Div;
        token.lexeme = "/";
        token.row = this->m_row;
        token.col = this->m_col;
        break;
    case '%':   // Mod
        token.kind = TokenKind::Mod;
        token.lexeme = "%";
        token.row = this->m_row;
        token.col = this->m_col;
        break;
    case '*':   // Mul
        token.kind = TokenKind::Mul;
        token.lexeme = "*";
        token.row = this->m_row;
        token.col = this->m_col;
        break;
    default:
        // numbers
        // identifiers:
        //      true, false, nil,
        //      fun, var, macro, lambda, if, progn, cond, import
        //      for, let, <<BuiltinFunctionName>>, <<BuiltinVariableName>>,
        //      <<UserDefinedFunctionOrMacroName>>, <<UserDefinedVaraibaleName>>
        if(std::isdigit(c)){
            auto self = this->read_number();
            token.kind = self.kind;
            token.lexeme = self.lexeme;
            token.row = self.row;
            token.col = self.col;
        }else if(this->is_valid_identifier_start_char(c)){
            auto self = this->read_identifier();
            token.kind = self.kind;
            token.lexeme = self.lexeme;
            token.row = self.row;
            token.col = self.col;
        }else{
            std::stringstream stream;
            stream << "error while tokenizing at Line " << this->m_row;
            stream << " and column " << this->m_col;
            throw Error(Error::Kind::SyntaxError, stream.str());
        }
        break;
    }

    return token;
}

// -*-
Token Tokenizer::read_identifier(void){
    static std::map<Str, TokenKind> _builtinIdentifiers = {
        {"true", TokenKind::True},
        {"false", TokenKind::False},
        {"nil", TokenKind::Nil},
        {"fun", TokenKind::Fun},
        {"macro", TokenKind::Macro},
        {"lambda", TokenKind::Lambda},
        {"var", TokenKind::Var},
        {"if", TokenKind::If},
        {"for", TokenKind::For},
        {"cond", TokenKind::Cond},
        {"progn", TokenKind::Progn},
        {"let", TokenKind::Let},
    };
    Str lexeme{};
    auto c = static_cast<char>(this->m_cur);
    if(!this->is_valid_identifier_start_char(c)){
        std::stringstream stream;
        stream << "invalid first identifier character '" << c << "'";
        throw Error(Error::Kind::Default, stream.str());
    }
    lexeme = c;
    if(this->is_eof()){
        auto entry = _builtinIdentifiers.find(lexeme);
        Token token{};
        if(entry != _builtinIdentifiers.end()){
            token.kind = entry->second;
        }else{
            token.kind = TokenKind::Ident;
        }
        token.lexeme = lexeme;
        token.row = this->m_row;
        token.col = this->m_col;
        return token;
    }
    c = this->next();
    while(this->is_valid_identifier_char(c)){
        lexeme += c;
        if(this->is_eof()){ break;}
        c = this->next();
    }
    auto entry = _builtinIdentifiers.find(lexeme);
    Token token{};
    if(entry != _builtinIdentifiers.end()){
        token.kind = entry->second;
    }else{
        token.kind = TokenKind::Ident;
    }
    token.lexeme = lexeme;
    token.row = this->m_row;
    token.col = this->m_col;

    return token;
}

// -*-
static inline char _to_char(i32 n){
    return static_cast<char>(n);
}

// -*-
Token Tokenizer::read_number(void){
    bool isFloating = false;
    auto c = _to_char(this->m_cur);
    auto row = this->m_row;
    auto col = this->m_col;
    Token token{};
    token.row = row;
    token.col = col;
    // -*- number-format:
    //  <mantissa>[.]<fractional>[eE]<exponent>
    Str lexeme{};
    // (01) mantissa
    while(std::isdigit(c) && !this->is_eof()){
        lexeme += c;
        c = _to_char(this->next());
    }
    if(this->is_eof()){
        // Token is an integer
        token.kind = TokenKind::Integer;
        token.lexeme = lexeme;
        return token;
    }
    // or maybe integer in binary|octal|hexadecimal format
    if(c == 'b'){ // binary format
        lexeme += 'b';
        this->advance();
        c = _to_char(this->next());
        if(!(c=='0' || c=='1')){
            throw Error(Error::Kind::SyntaxError, "malfomed integer in binary format");
        }
        while((Str{"01"}.find(c) != Str::npos) && !this->is_eof()){
            lexeme += c;
            c = _to_char(this->next());
        }
        token.kind = TokenKind::Integer;
        token.lexeme = lexeme;
        return token;
    }
    if(c == 'o'){ // octal format
        lexeme += 'o';
        this->advance();
        Str octals = "01234567";
        c = _to_char(this->next());
        if(octals.find(c) == Str::npos){
            throw Error(Error::Kind::SyntaxError, "malfomed integer in octal format");
        }
        while((octals.find(c) != Str::npos) && !this->is_eof()){
            lexeme += c;
            c = _to_char(this->next());
        }
        token.kind = TokenKind::Integer;
        token.lexeme = lexeme;
        return token;
    }
    if(c == 'x'){ // hexadecimal format
        lexeme += 'b';
        this->advance();
        Str hexadecs = "0123456789abcdefABCDEF";
        c = _to_char(this->next());
        if(hexadecs.find(c) == Str::npos){
            throw Error(Error::Kind::SyntaxError, "malfomed integer in hexadecimal format");
        }
        while((hexadecs.find(c)!= Str::npos) && !this->is_eof()){
            lexeme += c;
            c = _to_char(this->next());
        }
        token.kind = TokenKind::Integer;
        token.lexeme = lexeme;
        return token;
    }
    // (02) potential decimal-point
    if(c == '.'){
        lexeme += '.';
        isFloating = true;
        this->advance();
    }
    if(this->is_eof()){
        token.kind = isFloating? TokenKind::Float : TokenKind::Integer;
        token.lexeme = lexeme;
        return token;
    }
    // (03) fractional part
    c = _to_char(this->next());
    while(std::isdigit(c) && !this->is_eof()){
        lexeme += c;
        c = _to_char(this->next());
    }
    // (04) potential 'e' or 'E'
    if(c == 'e' || c == 'E'){
        lexeme += c;
        isFloating = true;
        this->advance();
        if(this->is_eof()){
            token.kind = TokenKind::Float;
            token.lexeme = lexeme;
            return token;
        }
        c = _to_char(this->next());
    }
    // possibly '+' or '-'
    if(c == '+' || c == '-'){
        lexeme += c;
        this->advance();
        if(this->is_eof()){
            throw Error(Error::Kind::SyntaxError, "malformed floating-point number");
        }
        c = _to_char(this->next());
    }
    if(std::isdigit(c)){
        while(std::isdigit(c) && !this->is_eof()){
            lexeme += c;
            c = _to_char(this->next());
        }
    }else{
        throw Error(Error::Kind::SyntaxError, "malformed floating-point number");
    }
    
    token.kind = isFloating? TokenKind::Float : TokenKind::Integer;
    token.lexeme = lexeme;

    return token;
}

// -*-
Token Tokenizer::read_str(void){
    Str lexeme{};
    auto row = this->m_row;
    auto col = this->m_col;
    auto c = _to_char(this->next());
    while(c != '"' && !this->is_eof()){
        if(c == '\\'){
            if(this->m_cur == EOF){
                throw Error(Error::Kind::Default, "unexpected end-of-file");
            }
            auto cur = _to_char(this->m_cur);
            switch(cur){
            case '\\':
                lexeme += "\\";
                this->advance();
                break;
            case 'n':
                lexeme += "\n";
                this->advance();
                break;
            case 'r':
                lexeme += "\r";
                this->advance();
                break;
            case 'f':
                lexeme += "\f";
                this->advance();
                break;
            case 't':
                lexeme += "\t";
                this->advance();
                break;
            default:
                break;
            }
        }else{
            lexeme += c;
        }
        c = _to_char(this->next());
    }
    if(this->is_eof()){
        throw Error(Error::Kind::Default, "unexpected end-of-file");
    }
    // skip the closing double-quote
    //this->advance();
    Token token{};
    token.kind = TokenKind::String;
    token.lexeme = lexeme;
    token.row = row;
    token.col = col;
    return token;
}

// -*-
i32 Tokenizer::next(void){
    auto result = this->m_cur;
    ++this->m_pos;
    if(this->is_eof()){
        this->m_cur = EOF;
    }else{
        this->m_cur = this->m_src[this->m_pos];
    }
    return result;
}

// -*-
void Tokenizer::advance(void){
    if(!this->is_eof()){
        ++this->m_pos;
    }
}

// -*-
char Tokenizer::peek(){
    if(this->m_cur==EOF){ return EOF; }
    return _to_char(this->m_cur);
}

// -*-
void Tokenizer::skip_whitespace(void){
    if(this->m_cur == EOF){ return; }
    if(std::isspace(this->m_cur)){
        while(std::isspace(this->next() && !this->is_eof())){}
    }
}

// -*-
void Tokenizer::skip_comment(void){
    //! @todo
}

// -*-
bool Tokenizer::match(const Str& ident){
    //! @todo
    return false;
}

// -*-
bool Tokenizer::is_eof(void){
    //! @todo
    return false;
}

/*
class Tokenizer{
private:
    Str m_src;
    Str::const_iterator m_beg;
    Str::const_iterator m_end;
    Str::iterator m_ptr;

public:
    Tokenizer() = default;
    ~Tokenizer() = default;

private:
// Token Tokenizer::read_bool(void){}
// Token Tokenizer::read_integer(void){}
// Token Tokenizer::read_float(void){}

    //Token read_list(void);

};

*/


// -*----------------------------------------------------------------*-
}//-*- end::namespace::alz                                          -*-
// -*----------------------------------------------------------------*-