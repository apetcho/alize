#include "alize.hpp"


// -*----------------------------------------------------------------*-
// -*- begin::namespace::alz                                        -*-
// -*----------------------------------------------------------------*-
namespace alz{
// -*-
Parser::Parser(const Str& src){
    //! @todo
}

Parser::Parser(std::stringstream* stream){
    //! @todo
}

Object Parser::parse(void){
    //! @todo
    return Object();
}

 // nil, true, false, integer, float, string
Object Parser::parse_atom(void){
    //! @todo
    return Object();
}

Object Parser::parse_list(void){
    //! @todo
    return Object();
}

Object Parser::parse_fun(void){
    //! @todo
    return Object();
}

Object Parser::parse_macro(void){
    //! @todo
    return Object();
}

Object Parser::parse_var(void){
    //! @todo
    return Object();
}

Object Parser::parse_lambda(void){
    //! @todo
    return Object();
}

Object Parser::parse_progn(void){
    //! @todo
    return Object();
}

Object Parser::parse_if(void){
    //! @todo
    return Object();
}

Object Parser::parse_cond(void){
    //! @todo
    return Object();
}

Object Parser::parse_import(void){
    //! @todo
    return Object();
}

bool Parser::expect(AstKind kind){
    //! @todo
    return false;
}

/*
// -*- Parser -*-
class Parser final{
private:
    Tokenizer m_tokenizer;

public:
    Parser() = default;

private:

*/


// -*----------------------------------------------------------------*-
}//-*- end::namespace::alz                                          -*-
// -*----------------------------------------------------------------*-