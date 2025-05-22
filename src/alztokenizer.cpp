#include "alize.hpp"
#include<cctype>

// -*----------------------------------------------------------------*-
// -*- begin::namespace::alz                                        -*-
// -*----------------------------------------------------------------*-
namespace alz{
// -*-
Tokenizer::Tokenizer(const Str& src)
: m_src{src}, m_pos{usize{}}
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
Token Tokenizer::read_ident(void){}
Token Tokenizer::read_bool(void){}
Token Tokenizer::read_integer(void){}
Token Tokenizer::read_float(void){}
Token Tokenizer::read_str(void){}
    //Token read_list(void);
char Tokenizer::peek(){}
void Tokenizer::skip_whitespace(void){}
void Tokenizer::skip_comment(void){}
bool Tokenizer::match(const Str& ident){}
};

*/


// -*----------------------------------------------------------------*-
}//-*- end::namespace::alz                                          -*-
// -*----------------------------------------------------------------*-