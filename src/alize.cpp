#include "alize.hpp"
#include<iomanip>
#include<cmath>
#include<limits>

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
Object::Object(Str name, CFun cfun)
: m_typekind{TypeKind::Fn}
, m_name{name}
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
, m_name{other.m_name}
, m_value{other.m_value}{}

// -*-
Object::Object(Object&& other)
: m_typekind{std::move(other.m_typekind)}
, m_name{std::move(other.m_name)}
, m_value{std::move(other.m_value)}{}

// -*-
Object& Object::operator=(const Object& other){
    if(this != &other){
        this->m_typekind = other.m_typekind;
        this->m_name = other.m_name;
        this->m_value = other.m_value;
    }

    return *this;
}

// -*-
Object& Object::operator=(Object&& other){
    if(this != &other){
        this->m_typekind = std::move(other.m_typekind);
        this->m_name = std::move(other.m_name);
        this->m_value = std::move(other.m_value);
    }
    return *this;
}

// -*- type-cast -*-
Object::operator bool(){
    bool result{};
    switch(this->m_typekind){
    case TypeKind::Nil:
        result = false;
        break;
    case TypeKind::Bool:
        result = std::get<bool>(this->m_value);
        break;
    case TypeKind::Int:{
            auto num = std::get<i64>(this->m_value);
            result = (num == 0) ? false : true;
        }
        break;
    case TypeKind::Float:{
            auto num = std::get<f64>(this->m_value);
            result = Alize::almostEqual(num, 0.0);
        }
        break;
    default:
        result = true;
        break;
    }

    return result;
}

// -*-
Object::operator i64(){
    if(this->is_integer()){
        auto num = std::get<i64>(this->m_value);
        return num;
    }
    if(this->is_float()){
        auto num = std::get<f64>(this->m_value);
        return static_cast<i64>(num);
    }
    if(this->is_bool()){
        auto val = std::get<bool>(this->m_value);
        return (val ? 1 : 0);
    }
    std::stringstream stream;
    stream << "cannot convert `" << this->type().data << "' into `integer'";
    throw Error(Error::TypeError, stream.str());
}

// -*-
Object::operator f64(){
    if(this->is_float()){
        auto num = std::get<f64>(this->m_value);
        return num;
    }
    if(this->is_integer()){
        auto num = std::get<i64>(this->m_value);
        return static_cast<f64>(num);
    }
    if(this->is_bool()){
        auto val = std::get<bool>(this->m_value);
        return (val ? 1.0 : 0.0);
    }
    std::stringstream stream;
    stream << "cannot convert `" << this->type().data << "' into `float'";
    throw Error(Error::TypeError, stream.str());
}

// -*-
Object::operator Str(){
    if(this->is_string()){
        auto str = std::get<Str>(this->m_value);
        return str;
    }
    std::stringstream stream;
    stream << "cannot convert `" << this->type().data << "' into `string'";
    throw Error(Error::TypeError, stream.str());
}

// -*-
Object::operator Symbol(){
    if(this->is_string()){
        auto str = std::get<Symbol>(this->m_value);
        return str;
    }
    std::stringstream stream;
    stream << "cannot convert `" << this->type().data << "' into `symbol'";
    throw Error(Error::TypeError, stream.str());
}

// -*-
Object::operator CFun(){
    if(this->is_builtin_function()){
        auto cfun = std::get<CFun>(this->m_value);
        return cfun;
    }
    std::stringstream stream;
    stream << "cannot convert `" << this->type().data << "' into `builtin function'";
    throw Error(Error::TypeError, stream.str());
}

// -*-
Object::operator Closure(){
    if(this->is_closure()){
        auto closure = std::get<Closure>(this->m_value);
        return closure;
    }
    std::stringstream stream;
    stream << "cannot convert `" << this->type().data << "' into `closure'";
    throw Error(Error::TypeError, stream.str());
}

// -*-
Object::operator ArrayList(){
    if(this->is_list()){
        auto vec = std::get<ArrayList>(this->m_value);
        return vec;
    }
    std::stringstream stream;
    stream << "cannot convert `" << this->type().data << "' into `closure'";
    throw Error(Error::TypeError, stream.str());
}

// -*- Predicates -*-
bool Object::is_nil(void) const{
    return this->m_typekind == TypeKind::Nil;
}

// -*-
bool Object::is_bool(void) const{
    return this->m_typekind == TypeKind::Bool;
}

// -*-
bool Object::is_integer(void) const{
    return this->m_typekind == TypeKind::Int;
}

// -*-
bool Object::is_float(void) const{
    return this->m_typekind == TypeKind::Float;
}

// -*-
bool Object::is_symbol(void) const{
    return this->m_typekind == TypeKind::Sym;
}

// -*-
bool Object::is_string(void) const{
    return this->m_typekind == TypeKind::String;
}

// -*-
bool Object::is_builtin_function(void) const{
    return this->m_typekind == TypeKind::Fn;
}

// -*-
bool Object::is_closure(void) const{
    return (
        this->m_typekind == TypeKind::Lambda ||
        this->m_typekind == TypeKind::Fun ||
        this->m_typekind == TypeKind::Macro
    );
}

// -*-
bool Object::is_list(void) const{
    return this->m_typekind == TypeKind::List;
}

// -*- stringifiers -*-
Str Object::str(void) const{
    std::stringstream stream;
    switch(this->m_typekind){
    case TypeKind::Nil:
        stream << "nil";
        break;
    case TypeKind::Bool:{
            auto val = std::get<bool>(this->m_value);
            stream << (val ? "true" : "false");
        }
        break;
    case TypeKind::Int:{
            auto num = std::get<i64>(this->m_value);
            stream << num;
        }
        break;
    case TypeKind::Float:{
            auto num = std::get<f64>(this->m_value);
            stream << num;
        }
        break;
    case TypeKind::Sym:{
            auto val = std::get<Symbol>(this->m_value);
            stream << val.data;
        }
        break;
    case TypeKind::String:{
            auto val = std::get<Str>(this->m_value);
            stream << val;
        }
        break;
    case TypeKind::Fn:{
            auto val = std::get<Symbol>(this->m_value);
            auto name = this->m_name.value_or("");
            auto cfun = std::get<CFun>(this->m_value);
            stream << "Builtin function `" << name << "` @ 0x";
            stream << std::hex << cfun;
        }
        break;
    case TypeKind::Fun:{
            auto closure = std::get<Closure>(this->m_value);
            auto ast = closure.ast();
            stream << "Function `" << closure.name() << "` @ 0x";
            stream << std::hex << ast.get();
        }
        break;
    case TypeKind::Lambda:{
            auto closure = std::get<Closure>(this->m_value);
            auto ast = closure.ast();
            stream << "Lambda  @ 0x" << std::hex << ast.get();
        }
        break;
    case TypeKind::Macro:{
            auto closure = std::get<Closure>(this->m_value);
            auto ast = closure.ast();
            stream << "Macro `" << closure.name() << "` @ 0x";
            stream << std::hex << ast.get();
        }
        break;
    case TypeKind::List:{
            auto vec = std::get<ArrayList>(this->m_value);
            if(vec.size()==0){ stream << "()";}
            else{
                stream << "(";
                auto len = vec.size();
                for(decltype(len) i=0; i < len; i++){
                    stream << vec[i].str();
                    if(i < (len-1)){ stream << " "; }
                }
                stream << ")";
            }
        }
        break;
    }

    return stream.str();
}

// -*-
Str Object::repr(void) const{
    std::stringstream stream;
    switch(this->m_typekind){
    case TypeKind::Nil:
        stream << "\"nil\"";
        break;
    case TypeKind::Bool:{
            auto val = std::get<bool>(this->m_value);
            stream << (val ? "\"true\"" : "\"false\"");
        }
        break;
    case TypeKind::Int:{
            auto num = std::get<i64>(this->m_value);
            stream << std::quoted(std::to_string(num));
        }
        break;
    case TypeKind::Float:{
            auto num = std::get<f64>(this->m_value);
            stream << std::quoted(std::to_string(num));
        }
        break;
    case TypeKind::Sym:{
            auto val = std::get<Symbol>(this->m_value);
            stream << std::quoted(val.data);
        }
        break;
    case TypeKind::String:{
            auto val = std::get<Str>(this->m_value);
            stream << std::quoted(val);
        }
        break;
    case TypeKind::Fn:{
            auto val = std::get<Symbol>(this->m_value);
            auto name = this->m_name.value_or("");
            auto cfun = std::get<CFun>(this->m_value);
            stream << "Builtin function `" << name << "` @ 0x";
            stream << std::hex << cfun;
        }
        break;
    case TypeKind::Fun:
    case TypeKind::Lambda:
    case TypeKind::Macro:{
            auto closure = std::get<Closure>(this->m_value);
            auto ast = closure.ast();
            stream << ast->repr();
        }
        break;
    case TypeKind::List:{
            auto vec = std::get<ArrayList>(this->m_value);
            if(vec.size()==0){ stream << "\"'()\"";}
            else{
                stream << "'(";
                auto len = vec.size();
                for(decltype(len) i=0; i < len; i++){
                    stream << vec[i].repr();
                    if(i < (len-1)){ stream << " "; }
                }
                stream << ")";
            }
        }
        break;
    }

    return stream.str();
}

// -*- unary-operator: {-, +, ~, } -*-
Object& Object::operator-(){
    if(!this->is_number()){
        std::stringstream stream;
        stream << "cannot negate object of type `" << this->type().data << "'";
        throw Error(Error::Kind::TypeError, stream.str());
    }
    if(this->is_integer()){
        auto num = std::get<i64>(this->m_value);
        this->m_value = -num;
    }else{
        auto num = std::get<f64>(this->m_value);
        this->m_value = -num;
    }
    return *this;
}

// -*-
Object& Object::operator+(){
    if(!this->is_number()){
        std::stringstream stream;
        stream << "cannot negate object of type `" << this->type().data << "'";
        throw Error(Error::Kind::TypeError, stream.str());
    }
    return *this;
}

/*
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