#include "alize.hpp"


// -*----------------------------------------------------------------*-
// -*- begin::namespace::alz                                        -*-
// -*----------------------------------------------------------------*-
namespace alz{
// -*-
Tokenizer::Tokenizer(const Str& src)
: m_src{src}
, m_pos{usize{}}{}

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

Vec<Token> Tokenizer::tokenize(){}

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