#include "alize.hpp"
#include<iomanip>

// -*----------------------------------------------------------------*-
// -*- begin::namespace::alz                                        -*-
// -*----------------------------------------------------------------*-
namespace alz{
// -*-

// -*-----------*-
// -*- Closure -*-
// -*-----------*-
Closure::Closure(FunAst ast)
: m_kind{Closure::Kind::Fun}
, m_ast{std::make_shared<FunAst>(ast)}
{}

// -*-
Closure::Closure(LambdaAst ast)
: m_kind{Closure::Kind::Lambda}
, m_ast{std::make_shared<LambdaAst>(ast)}
{}

// -*-
Closure::Closure(MacroAst ast)
: m_kind{Closure::Kind::Macro}
, m_ast{std::make_shared<MacroAst>(ast)}
{}

// -*-
Closure::~Closure(){
    this->m_ast.reset();
}

// -*-
Object Closure::operator()(Vec<Object> argv, Env& env){
    Object result{};
    if(this->is_function()){
        auto ast = dynamic_cast<FunAst*>(this->m_ast.get());
        if(ast == nullptr){
            throw Error(
                Error::Kind::RuntimeError,
                "unexpected error occurred while creating a `function`"
            );
        }
        auto params = ast->params();
        if(params.size() != argv.size()){
            throw Error(Error::SyntaxError, "arguments count mismatch");
        }
        auto scope = ast->scope();
        scope.set_parent(&env);
        for(auto i=0; i < params.size(); i++){
            auto key = static_cast<Str>(params[i]);
            Object val(argv[i]);
            scope.update(key, val);
        }
        result = ast->eval(scope);
    }else if(this->is_lambda()){
        auto ast = dynamic_cast<LambdaAst*>(this->m_ast.get());
        if(ast == nullptr){
            throw Error(
                Error::Kind::RuntimeError, 
                "unexpected error occurred while creating a `lambda`"
            );
        }
        auto params = ast->params();
        if(params.size() != argv.size()){
            throw Error(Error::SyntaxError, "arguments count mismatch");
        }
        auto scope = ast->scope();
        scope.set_parent(&env);
        for(auto i=0; i < params.size(); i++){
            auto key = static_cast<Str>(params[i]);
            Object val(argv[i]);
            scope.update(key, val);
        }
        result = ast->eval(scope);
    }else if(this->is_macro()){
        auto ast = dynamic_cast<MacroAst*>(this->m_ast.get());
        if(ast == nullptr){
            throw Error(
                Error::Kind::RuntimeError,
                "unexpected error occurred while creating a `macro`"
            );
        }
        auto params = ast->params();
        if(params.size() != argv.size()){
            throw Error(Error::SyntaxError, "arguments count mismatch");
        }
        auto scope = ast->scope();
        scope.set_parent(&env);
        for(auto i=0; i < params.size(); i++){
            auto key = static_cast<Str>(params[i]);
            Object val(argv[i]);
            scope.update(key, val);
        }
        result = ast->eval(scope);
    }

    return result;
}

// -*-
bool Closure::is_function(void) const{
    return this->m_kind == Closure::Kind::Fun;
}

// -*-
bool Closure::is_lambda(void) const{
    return this->m_kind == Closure::Kind::Lambda;
}

// -*-
bool Closure::is_macro(void) const{
    return this->m_kind == Closure::Kind::Macro;
}

// -*-
Str Closure::str(void) const{
    std::stringstream stream;
    if(this->is_function()){
        auto ast = dynamic_cast<FunAst*>(this->m_ast.get());
        if(ast == nullptr){
            throw Error(
                Error::Kind::RuntimeError,
                "unexpected error occurred while creating a `function`"
            );
        }
        stream << "Function `" << ast->name() << "` @ 0x" << std::hex << ast;
    }else if(this->is_lambda()){
        auto ast = dynamic_cast<LambdaAst*>(this->m_ast.get());
        if(ast == nullptr){
            throw Error(
                Error::Kind::RuntimeError,
                "unexpected error occurred while creating a `function`"
            );
        }
        stream << "Lambda @ 0x" << std::hex << ast;
    }
    else if(this->is_macro()){
        auto ast = dynamic_cast<MacroAst*>(this->m_ast.get());
        if(ast == nullptr){
            throw Error(
                Error::Kind::RuntimeError,
                "unexpected error occurred while creating a `function`"
            );
        }
        stream << "Function: `" << ast->name() << "` @ 0x" << std::hex << ast;
    }

    return stream.str();
}

// -*-
Str Closure::repr(void) const{
    std::stringstream stream;
    if(this->is_function()){
        auto ast = dynamic_cast<FunAst*>(this->m_ast.get());
        if(ast == nullptr){
            throw Error(
                Error::Kind::RuntimeError,
                "unexpected error occurred while creating a `function`"
            );
        }
        stream << "(fun " << ast->name() << "(";
        auto params = ast->params();
        if(params.size()==0){
            stream << ")";
        }else{
            for(auto i=0; i < params.size(); i++){
                stream << static_cast<Str>(params[i]);
                if(i < params.size()-1){ stream << " "; }
            }
            stream << ")\n    ";
        }
        auto body = ast->body();
        for(const auto& elem: body){
            stream << elem->repr() << "\n    ";
        }
        stream << "\n    )";
    }else if(this->is_lambda()){
        auto ast = dynamic_cast<LambdaAst*>(this->m_ast.get());
        if(ast == nullptr){
            throw Error(
                Error::Kind::RuntimeError,
                "unexpected error occurred while creating a `lambda`"
            );
        }
        stream << "(lambda (";
        auto params = ast->params();
        if(params.size()==0){
            stream << ")";
        }else{
            for(auto i=0; i < params.size(); i++){
                stream << static_cast<Str>(params[i]);
                if(i < params.size()-1){ stream << " "; }
            }
            stream << ")\n    ";
        }
        auto body = ast->body();
        for(const auto& elem: body){
            stream << elem->repr() << "\n    ";
        }
        stream << "\n    )";
    }
    else if(this->is_macro()){
        auto ast = dynamic_cast<MacroAst*>(this->m_ast.get());
        if(ast == nullptr){
            throw Error(
                Error::Kind::RuntimeError,
                "unexpected error occurred while creating a `macro`"
            );
        }
        stream << "(macro " << ast->name() << "(";
        auto params = ast->params();
        if(params.size()==0){
            stream << ")";
        }else{
            for(auto i=0; i < params.size(); i++){
                stream << static_cast<Str>(params[i]);
                if(i < params.size()-1){ stream << " "; }
            }
            stream << ")\n    ";
        }
        auto body = ast->body();
        for(const auto& elem: body){
            stream << elem->repr() << "\n    ";
        }
        stream << "\n    )";
    }

    return stream.str();
}

// -*-
Str Closure::name(void) const{
    Str result{};
    if(this->is_lambda()){ result = ""; }
    else if(this->is_function()){
        auto ast = dynamic_cast<FunAst*>(this->m_ast.get());
        if(ast != nullptr){ result = ast->name(); }
        else{
            throw Error(
                Error::Kind::RuntimeError,
                "unexpected error occurred while creating a `function`"
            );
        }
    }else if(this->is_macro()){
        auto ast = dynamic_cast<MacroAst*>(this->m_ast.get());
        if(ast != nullptr){ result = ast->name(); }
        else{
            throw Error(
                Error::Kind::RuntimeError,
                "unexpected error occurred while creating a `macro`"
            );
        }
    }

    return result;
}

// -*-
const Ast& Closure::ast(void) const{
    return this->m_ast;
}

// -*----------*-
// -*- Object -*-
// -*----------*-
Object::Object()
: m_typekind{TypeKind::Nil}
, m_value{}{}

// -*-
Object::Object(bool val)
: m_typekind{TypeKind::Bool}
, m_value{val}{}

// -*-
Object::Object(i64 val)
: m_typekind{TypeKind::Int}
, m_value{val}{}

// -*-
Object::Object(f64 val)
: m_typekind{TypeKind::Float}
, m_value{val}{}

// -*-
Object::Object(Str val)
: m_typekind{TypeKind::String}
, m_value{val}{}

// -*-
Object::Object(Symbol val)
: m_typekind{TypeKind::Sym}
, m_value{val}{}


// -*-
Object::Object(CFun cfun)
: m_typekind{TypeKind::Fn}
, m_value{cfun}{}


// -*-
Object::Object(Closure closure)
: m_typekind{TypeKind::Fun}
, m_value{closure}{
    if(closure.is_function()){ this->m_typekind = TypeKind::Fun; }
    else if(closure.is_lambda()){ this->m_typekind = TypeKind::Lambda; }
    else if(closure.is_macro()){ this->m_typekind = TypeKind::Macro; }
}

// -*-
Object::Object(ArrayList xs)
: m_typekind{TypeKind::List}
, m_value{xs}{}


// -*-
Object::Object(const Object& other)
: m_typekind{other.m_typekind}
, m_value{other.m_value}{}

/*
Object::Object& operator=(const Object& other){}
Object::Object& operator=(Object&& other){}
Object::~Object(){}

// -*- type-cast -*-
Object::operator bool(){}
Object::operator i64(){}
Object::operator f64(){}
Object::operator Str(){}
Object::operator Symbol(){}
Object::operator CFun(){}
Object::operator Closure(){}
Object::operator ArrayList(){}

// -*- Predicates -*-
bool Object::is_nil(void) const{}
bool Object::is_bool(void) const{}
bool Object::is_integer(void) const{}
bool Object::is_float(void) const{}
bool Object::is_symbol(void) const{}
bool Object::is_string(void) const{}
bool Object::is_builtin_function(void) const{}
bool Object::is_closure(void) const{}
bool Object::is_list(void) const{}

// -*- stringifiers -*-
Str Object::str(void) const{}
Str Object::repr(void) const{}
    
// -*- unary-operator: {-, +, ~, } -*-
Object& Object::operator-(){}
Object& Object::operator+(){}
Object& Object::operator~(){}

// -*- binary-operator: {-, +, *, /, %, and, or,} -*-
Object operator+(const Object& lhs, const Object& rhs){}
Object operator-(const Object& lhs, const Object& rhs){}
Object operator*(const Object& lhs, const Object& rhs){}
Object operator/(const Object& lhs, const Object& rhs){}
Object operator%(const Object& lhs, const Object& rhs){}
Object operator||(const Object& lhs, const Object& rhs){}
Object operator&&(const Object& lhs, const Object& rhs){}

// -*- miscelaneous methods -*-
Symbol Object::type(void) const{}


// -*-------*-
// -*- Env -*-
// -*-------*-
Env::Env(){}
Env::Env(Env* parent){}
Env::Env(const Env& other){}
Env::~Env(){}
void Env::put(Str key, const Object& val){}
void Env::update(Str key, const Object& val){}
Object Env::get(Str key){}
bool Env::contains(const Str key){}
bool Env::contains(const Str key){}

*/

// -*----------------------------------------------------------------*-
}//-*- end::namespace::alz                                          -*-
// -*----------------------------------------------------------------*-