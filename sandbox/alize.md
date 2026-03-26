# Alize Programming Language

## Core Types

- Object
- Bool
- Byte
- Char
- Number
  - Integer
  - Float
  - Complex
- String
- Array
- ArrayList
- ByteArray
- Tuple
- List
- HashSet
- HashMap
- Structure
- Callable
  - Function
  - Lambda
  - Macro
- Iterable
  - StringIterator
  - ArrayIterator
  - ArrayListIterator
  - TupleIterator
  - ListIterator
  - HashSetIterator
  - HashMapIterator
- Trait


## Object class

```c++
class Alize;
class Module;
class Parameter;

class Formatter;
class Error;
class Env;

class Object;
class Byte;
class Char;
class Bool;
class Number;
class String;
class Tuple;
class Array;
class Arraylist;
class List;
class HashSet;
class HashMap;
class Callable;
class Iterable;
class Structure;
class Trait;
class None;
class Option;
class Result;


using f32 = float;
using f64 = double;
using u64 = std::uint64_t;
using i64 = std::int64_t;
using u32 = std::uint32_t;
using i32 = std::int32_t;
using usize = std::uint64_t;
using isize = std::int64_t;
using cmplx32 = std::complex<f32>;
using cmplx64 = std::complex<f64>;
using Self = std::shared_ptr<Object>;
using Iterator = std::shared_ptr<Iterable>;
using CFun = Self (*)(const std::vector<Self>&);
using Str = std::string;

template<typename T>
using Vec = std::vector<T>;
template<typename K, typename V>
using Dict = std::map<K, V>;

using Params = Vec<Parameter>;
using Args = Vec<Self>;
using Body = Vec<Self>;

using Ast = std::shared_ptr<AstBase>;
using Signature = std::pair<Str, Params>;


class Object{
public:
    explicit Object();
    virtual ~Object();
    virtual Str type(void) const;
    virtual bool is_raw_object(void) const;
    virtual bool is_bool(void) const;
    virtual bool is_byte(void) const;
    virtual bool is_char(void) const;
    virtual bool is_number(void) const;
    virtual bool is_integer(void) const;
    virtual bool is_float(void) const;
    virtual bool is_complex(void) const;
    virtual bool is_string(void) const;
    virtual bool is_tuple(void) const;
    virtual bool is_array(void) const;
    virtual bool is_arraylist(void) const;
    virtual bool is_list(void) const;
    virtual bool is_hashset(void) const;
    virtual bool is_hashmap(void) const;
    virtual bool is_callable(void) const;
    virtual bool is_function(void) const;
    virtual bool is_lambda(void) const;
    virtual bool is_macro(void) const;
    virtual bool is_trait(void) const;
    virtual bool is_structure(void) const;
    virtual bool is_ok(void) const;
    virtual bool is_err(void) const;
    virtual bool is_none(void) const;
    virtual bool is_some(void) const;

    virtual Str str(void) const;
    virtual Self format(const Formatter& formatter) const;
    virtual Self repr(void);
};


// -*-
class Byte final: public Object{};
class Char final: public Object{};
class Bool final: public Object{};

// -*-
class Number final: public Object{
private:
    using Value = std::variant<i32, u32, f32, cmplx32, i64, u64, f64, cmplx64>;

    Value m_value;
    
public:
    explicit Number();
    // ...

    static Number I32_MIN;
    static Number I32_MAX;
    static Number U32_MIN;
    static Number U32_MAX;
    static Number F32_MIN;
    static Number F32_MAX;
    static Number I64_MIN;
    static Number I64_MAX;
    static Number U64_MIN;
    static Number U64_MAX;
    static Number F64_MIN;
    static Number F64_MAX;

    static Number EPSILON;
    static Number NaN;
    static Number PI;

    // ..methods, operators, etc.
};

class String final: public Object{};
class Tuple final: public Object{};
class Array final: public Object{};
class Arraylist final: public Object{};
class List final: public Object{};
class HashSet final: public Object{};
class HashMap final: public Object{};

// -*-
class Callable final: public Object{
private:
    enum class Kind {
        CFUN, FUN, MACRO,
    };

    Str m_name;
    CFun m_cfun;
    Ast m_ast; // Fun, Lambda, or Macro
    
public:
    explicit Callable(const Str& name, CFun cfun) noexcept;
    explicit Callable(const FunctionAst& ast) noexcept;
    explicit Callable(const LambdaAst& ast) noexcept;
    explicit Callable(const MacroAst& ast) noexcept;

    Self operator()(const Args& args);
    // -- Predicates, others methods 
};

// -*-
/** @brief: Meta methods

    __neg__                ==> -arg
    __add__                ==> lhs + rhs
    __sub__                ==> lhs - rhs
    __div__                ==> lhs / rhs
    __mul__                ==> lhs * rhs
    __mod__                ==> lhs % rhs
    __not__                ==> !arg
    __or__                 ==> lhs || rhs
    __and__                ==> lhs && rhs
    __bit_lsh__            ==> lhs << rhs
    __bit_rsh__            ==> lhs >> rhs
    __bit_or__             ==> lhs | rhs
    __bit_and__            ==> lhs & rhs
    __bit_not__            ==> lhs ~ rhs
    __bit_xor__            ==> lhs ^ rhs
    __get_property__       ==> lhs.rhs
    __set_property__       ==> lhs.rhs = val
    __del_property__       ==> delete lhs.rhs
    __get_item__           ==> lhs[rhs]
    __set_item__           ==> lhs[rhs] = val
    __del_item__           ==> delete lhs[rhs]
    __copy__               ==> .copy()
    __move__               ==> .move()

    [.iter() -> Iterator]
    __next__               ==>
    __has_next__           ==> 
 
*/

// -*-
class Structure final: public Object{
private:
    Str m_name;
    Dict<Str, Self> m_properties;
    Dict<Str, Self> m_methods;
    Dict<Str, Self> m_staticfields;
    Dict<Str, Self> m_impl_traits;

public:
    explicit Structure(/* ...  */) noexcept;
    // ...
};

class Trait final: public Object{
private:
    Str m_name;
    Vec<Signature> m_signatures;

public:
    explicit Trait(const Str& name) noexcept;
    void add_signature(const Signature& signature);
    Signature get_signature(const Str& name);
    // ...
    Self implement(/* ...  */);
};

class None final: public Object {};

class Option final: public Object{
private:
    using Value = std::variant<None, Self>;

    Value m_value;

public:
    explicit Option() noexcept;
    explicit Option(const Self& self) noexcept;
    // ...
};


class Result final: public Object{
private:
    using Value = std::variant<Error, Self>;
    Value m_value;

public:
    explicit Result(const Self& self);
    explicit Result(const Error& error);
    // ...
};

```

## Abstract Syntax Tree

```c++

#define ALIZE_TOKENS()                \
    ALIZE_DEF(Eof, "EOF")             \
    ALIZE_DEF(Wildcard, "_")          \
    ALIZE_DEF(Assign, "=")            \
    ALIZE_DEF(Dot, ".")               \
    ALIZE_DEF(Colon, ":")             \
    ALIZE_DEF(LBracket, "[")          \
    ALIZE_DEF(RBracket, "]")          \
    ALIZE_DEF(LBrace, "{")            \
    ALIZE_DEF(RBrace, "}")            \
    ALIZE_DEF(LParen, "(")            \
    ALIZE_DEF(RParen, ")")            \
    ALIZE_DEF(Self, "self")           \
    ALIZE_DEF(When, "when")           \
    ALIZE_DEF(If, "if")               \
    ALIZE_DEF(Elif, "elif")           \
    ALIZE_DEF(Else, "else")           \
    ALIZE_DEF(For, "for")             \
    ALIZE_DEF(Loop, "loop")           \
    ALIZE_DEF(Continue, "continue")   \
    ALIZE_DEF(Break, "break")         \
    ALIZE_DEF(Return, "return")       \
    ALIZE_DEF(Let, "let")             \
    ALIZE_DEF(Var, "var")             \
    ALIZE_DEF(New, "new")             \
    ALIZE_DEF(Define, "define")       \
    ALIZE_DEF(Fun, "fun")             \
    ALIZE_DEF(Lambda, "lambda")       \
    ALIZE_DEF(Macro, "macro")         \
    ALIZE_DEF(Structure, "struct")    \
    ALIZE_DEF(Trait, "trait")         \
    ALIZE_DEF(Delete, "delete")       \
    ALIZE_DEF(Import, "import")       \
    ALIZE_DEF(From, "from")           \
    ALIZE_DEF(Implement, "implements")\
    ALIZE_DEF(Plus, "+")              \
    ALIZE_DEF(Neg, "-")               \
    ALIZE_DEF(Slash, "/")             \
    ALIZE_DEF(Percent, "%")           \
    ALIZE_DEF(LNot, "not")            \
    ALIZE_DEF(LAnd, "and")            \
    ALIZE_DEF(LOr, "or")              \
    ALIZE_DEF(BitNot, "~")            \
    ALIZE_DEF(BitAnd, "&")            \
    ALIZE_DEF(BitOr, "|")             \
    ALIZE_DEF(Equal, "==")            \
    ALIZE_DEF(NotEq, "!=")            \
    ALIZE_DEF(Less, "<")              \
    ALIZE_DEF(LessEq, "<=")           \
    ALIZE_DEF(Greater, ">")           \
    ALIZE_DEF(GreaterEq, ">=")        \
    ALIZE_DEF(BitXor, "^")


#define ALIZE_AST_KINDS()                      \
    ALIZE_DEF(Ident, "IDENT")                  \
    ALIZE_DEF(LiteralExpr, "LITERAL_EXPR")     \
    ALIZE_DEF(AssignStmt, "ASSIGN_STMT")       \
    ALIZE_DEF(NegateExpr, "NEGATE_EXPR")       \
    ALIZE_DEF(NotExpr, "NOT_EXPR")             \
    ALIZE_DEF(BNotExpr, "BIT_NOT_EXPR")        \
    ALIZE_DEF(AddExpr, "ADD_EXPR")             \
    ALIZE_DEF(SubExpr, "SUB_EXPR")             \
    ALIZE_DEF(DivExpr, "DIV_EXPR")             \
    ALIZE_DEF(MulExpr, "MUL_EXPR")             \
    ALIZE_DEF(ModExpr, "MOD_EXPR")             \
    ALIZE_DEF(AndExpr, "AND_EXPR")             \
    ALIZE_DEF(BAndExpr, "BIT_AND_EXPR")        \
    ALIZE_DEF(OrExpr, "OR_EXPR")               \
    ALIZE_DEF(BLshExpr, "BIT_LSH_EXPR")        \
    ALIZE_DEF(BRshExpr, "BIT_RSH_EXPR")        \
    ALIZE_DEF(BXorExpr, "BIT_XOR_EXPR")        \
    ALIZE_DEF(GetPropExpr, "GET_PROP_EXPR")    \
    ALIZE_DEF(DelPropExpr, "DEL_PROP_EXPR")    \
    ALIZE_DEF(GetItemExpr, "GET_ITEM_EXPR")    \
    ALIZE_DEF(DelItemExpr, "DEL_ITEM_EXPR")    \
    ALIZE_DEF(WhenExpr, "WHEN_EXPR")           \
    ALIZE_DEF(SetPropExpr, "SET_PROP_EXPR")    \
    ALIZE_DEF(SetItemExpr, "SET_ITEM_EXPR")    \
    ALIZE_DEF(EqualExpr, "EQUAL_EXPR")         \
    ALIZE_DEF(NotEqExpr, "NOT_EQ_EXPR")        \
    ALIZE_DEF(LessExpr, "LESS_EXPR")           \
    ALIZE_DEF(LessEqExpr, "LESS_EQ_EXPR")      \
    ALIZE_DEF(GreaterExpr, "GREATER_EXPR")     \
    ALIZE_DEF(GreaterEqExpr, "GREATER_EQ_EXPR")\
    ALIZE_DEF(MacroStmt, "MACRO_STMT")         \
    ALIZE_DEF(FunStmt, "FUN_STMT")             \
    ALIZE_DEF(LambdaExpr, "LAMBDA_EXPR")       \
    ALIZE_DEF(StructStmt, "STRUCT_STMT")       \
    ALIZE_DEF(TraitStmt, "TRAIT_STMT")         \
    ALIZE_DEF(CallExpr, "CALL_EXPR")           \
    ALIZE_DEF(IfStmt, "IF_STMT")               \
    ALIZE_DEF(ForStmt, "FOR_STMT")             \
    ALIZE_DEF(LoopStmt, "LOOP_STMT")           \
    ALIZE_DEF(ReturnStmt, "RETURN_STMT")       \
    ALIZE_DEF(ContinueStmt, "CONTINUE_STMT")   \
    ALIZE_DEF(BreakStmt, "BREAK_STMT")         \
    ALIZE_DEF(WithStmt, "WITH_STMT")           \
    ALIZE_DEF(MatchStmt, "MATCH_STMT")         \
    ALIZE_DEF(CaseStmt, "CASE_STMT")           \
    ALIZE_DEF(Main, "MAIN")                    \
    ALIZE_DEF(LetStmt, "LET_STMT")             \
    ALIZE_DEF(VarStmt, "VAR_STMT")             \
    ALIZE_DEF(DefineStmt, "DEFINE_STMT")       \
    ALIZE_DEF(ImportStmt, "IMPORT_STMT")       \
    ALIZE_DEF(FromExpr, "FROM_EXPR")           \
    ALIZE_DEF(ImplementsStmt, "IMPLEMENTS_STMT")\
    ALIZE_DEF(NewStmt, "NEW_STMT")


class AstBase{
protected:
    // AstKind m_kind;
    
public:
    explicit AstBast(/* AstKind kind */) noexcept;
    virtual ~Ast();
    /* AstKind kind(void) const; */
    virtual Self eval(Alize* alize) = 0;
};

class IdentAst final: public AstBase{};
class LiteralExprAst final: public AstBase{}; // Bool, Integer, Float

class UnaryAst: public AstBase{};;
class NegateExprAst final: public UnaryAst{};;
class NotExprAst final: public UnaryAst{};
class BitNotExprAst final: public UnaryAst{};
class AssignStmtAst final: public UnaryAst{};

class BinaryAst: public AstBase{};
class AddExprAst final: public BinaryAst{};
class SubExprAst final: public BinaryAst{};
class DivExprAst final: public BinaryAst{};
class MulExprAst final: public BinaryAst{};
class ModExprAst final: public BinaryAst{};
class AndExprAst final: public BinaryAst{};
class OrExprAst final: public BinaryAst{};
class BitLshExprAst final: public BinaryAst{};
class BitRshExprAst final: public BinaryAst{};
class BitAndExprAst final: public BinaryAst{};
class BitOrExprAst final: public BinaryAst{};
class BitXorExprAst final: public BinaryAst{};
class GetPropExprAst final: public BinaryAst{};
class DelPropExprAst final: public BinaryAst{};
class GetItemExprAst final: public BinaryAst{};
class DelItemExprAst final: public BinaryAst{};
class EqualExprAst final: public BinaryAst{};
class NotEqExprAst final: public BinaryAst{};
class LessExprAst final: public BinaryAst{};
class LessEqExprAst final: public BinaryAst{};
class GreaterExprAst final: public BinaryAst{};
class GreaterEqExprAst final: public BinaryAst{};

class TernaryAst: public AstBase{};
class WhenExprAst final: public TernaryAst{};
class SetPropExprAst final: public TernaryAst{};
class SetItemExprAst final: public TernaryAst{};

class MacroStmtAst final: public AstBase{};
class FunStmtAst final: public AstBase{};
class LambdaExprAst final: public AstBase{};
class StructStmtAst final: public AstBase{};

class CallExprAst final: public AstBase{};

class IfStmtAst final: public AstBase{};
class ForStmtAst final: public AstBase{};
class LoopStmtAst final: public AstBase{};
class ReturnStmtAst final: public AstBase{};
class ContinueStmtAst final: public AstBase{};
class BreakStmtAst final: public AstBase{};
class WithStmtAst final: public AstBase{};
// - with { ...}  -*- with block -*- === progn

class MatchStmtAst final: public AstBase{};
class CaseStmtAst final: public AstBase{};
class MainAst final: public AstBase{};

class LetStmtAst final: public AstBase{};
class VarStmtAst final: public AstBase{};
class DefineStmtAst final: public AstBase{};
class ImportStmtAst final: public AstBase{};
class ImplementsStmtAst final: public AstBase{};
class NewStmtAst final: public AstBase{};
```

## Alize Interpreter

```c++

/* 
enum class Precedence {
    NONE,
    ASSIGN,      // =
    OR,          // or
    AND,         // and
    EQUALITY,    // == !=
    COMPARISON,  // < > <= >=
    TERM,        // + -
    FACTOR,      // * /
    UNARY,       // ! - +
    CALL,        // . () []
    PRIMARY
}; // Bitwise operators
*/

struct Token;
class Tokenizer{ /* ... */ };

// -*-
class Parser{
private:
    /* ...  */

public:
    /* ...  */
    Ast parse(void);


private:
    Ast parse_ident(void);
    Ast parse_literal(void);
    Ast parse_let(void);
    Ast parse_var(void);
    Ast parse_define(void);
    Ast parse_fun(void);
    Ast parse_lambda(void);
    Ast parse_macro(void);
    Ast parse_if(void);
    Ast parse_match(void);
    Ast parse_case(void);
    Ast parse_when(void);
    Ast parse_with(void);
    Ast parse_import(void);
    Ast parse_from(void);
    Ast parse_for(void);
    Ast parse_loop(void);
    Ast parse_continue(void);
    Ast parse_break(void);
    Ast parse_return(void);
    Ast parse_struct(void);
    Ast parse_negate(void);
    Ast parse_add(void);
    Ast parse_sub(void);
    Ast parse_mul(void);
    Ast parse_mod(void);
    Ast parse_div(void);
    Ast parse_not(void);
    Ast parse_and(void);
    Ast parse_or(void);
    Ast parse_bit_and(void);
    Ast parse_bit_or(void);
    Ast parse_bit_xor(void);
    Ast parse_bit_lsh(void);
    Ast parse_bit_rsh(void);
    Ast parse_assign(void);
    Ast parse_equal(void);
    Ast parse_not_equal(void);
    Ast parse_less(void);
    Ast parse_less_eq(void);
    Ast parse_greater(void);
    Ast parse_greater_eq(void);
    Ast parse_get_prop(void);
    Ast parse_set_prop(void);
    Ast parse_del_prop(void);
    Ast parse_get_item(void);
    Ast parse_set_item(void);
    Ast parse_del_item(void);
    Ast parse_call(void);
    Ast parse_new(void);
    Ast parse_implements(void);
};


// -*-
class Alize final{
private:
    /*
     struct Version{};
     m_runtime: Env
     m_version: Version
     
     */

public:
    /*
     ::builtinDocs: Dict<Str, Str>;
     ::userDocs: Dict<Str, Str>;
     ::license: Str;
     ::prelude: Env;
     */

    explicit Alize() noexcept;
    // void version(void);

    Self eval(void);

    static bool is_keyword(const Str& word);
    static bool is_reserved_word(const Str& word);
    static bool is_builtin_type(const Str& tname);
    static bool check_type(/* ...  */);
    static bool check_value(/* ... */);
    static void expect(/* ... */);

private:
    Self eval_ident(const IdentAst& ast);
    Self eval_literal_expr(const LiteralExprAst& ast);
    Self eval_negate_expr(const NegateExprAst& ast);
    Self eval_not_expr(const NotExprAst& ast);
    Self eval_bit_not_expr(const BitNotExprAst& ast);
    Self eval_assign_stmt(const AssignStmtAst& ast);
    Self eval_add_expr(const AddExprAst& ast);
    Self eval_sub_expr(const SubExprAst& ast);
    Self eval_div_expr(const DivExprAst& ast);
    Self eval_mul_expr(const MulExprAst& ast);
    Self eval_mod_expr(const ModExprAst& ast);
    Self eval_or_expr(const OrExprAst& ast);
    Self eval_bit_lsh_expr(const BitLshExprAst& ast);
    Self eval_bit_rhs_expr(const BitRshExprAst& ast);
    Self eval_bit_and_expr(const BitAndExprAst& ast);
    Self eval_bit_xor_expr(const BitXorExprAst& ast);
    Self eval_bit_or_expr(const BitOrExprAst& ast);
    Self eval_get_prop_expr(const GetPropExprAst& ast);
    Self eval_set_prop_expr(const SetPropExprAst& ast);
    Self eval_del_prop_expr(const DelPropExprAst& ast);
    Self eval_get_item_expr(const GetItemExprAst& ast);
    Self eval_set_item_expr(const SetItemExprAst& ast);
    Self eval_del_item_expr(const DelItemExprAst& ast);
    Self eval_equal_expr(const EqualExprAst& ast);
    Self eval_not_equal_expr(const NotEqExprAst& ast);
    Self eval_less_expr(const LessExprAst& ast);
    Self eval_less_eq_expr(const LessEqExprAst& ast);
    Self eval_greater_expr(const GreaterExprAst& ast);
    Self eval_greater_eq_expr(const GreaterEqExprAst& ast);
    Self eval_whene_expr(const WhenExprAst& ast);
    Self eval_macro_stmt(const MacroStmtAst& ast);
    Self eval_fun_stmt(const FunStmtAst& ast);
    Self eval_lambda_expr(const LambdaExprAst& ast);
    Self eval_struct_stmt(const StructStmtAst& ast);
    Self eval_call_expr(const CallExprAst& ast);
    Self eval_if_stmt(const IfStmtAst& ast);
    Self eval_for_stmt(const ForStmtAst& ast);
    Self eval_loop_stmt(const LoopStmtAst& ast);
    Self eval_return_stmt(const ReturnStmtAst& ast);
    Self eval_continue_stmt(const ContinueStmtAst& ast);
    Self eval_break_stmt(const BreakStmtAst& ast);
    Self eval_with_stmt(const WithStmtAst& ast);
    Self eval_match_stmt(const MatchStmtAst& ast);
    Self eval_case_stmt(const CaseStmtAst& ast);
    Self eval_main(const MainAst& ast);
    Self eval_let_stmt(const LetStmtAst& ast);
    Self eval_var_stmt(const VarStmtAst& ast);
    Self eval_define_stmt(const DefineStmtAst& ast);
    Self eval_import_stmt(const ImportStmtAst& ast);
    Self eval_from_expr(const FromExprAst& ast);
    Self eval_implements_stmt(const ImplementsStmtAst& ast);
    Self eval_new_stmt(const NewStmtAst& ast);
    // Self eval_(const _Ast& ast);
};

/*

SYMBOLS:
   - @operator, @impl, @private, @static, @property, @hidding

   struct Vec{
       @property{
           x, y, z
       }

       fun __init__(x, y, z){
           self.x = x
           self.y = y
           self.z = z
       }

       @operator +(othervec){
           let x = self.x + othervec.x
           let y = self.y + othervec.y
           let z = self.z + othervec.z
           return new Vec(x, y, z)
       }

       @overload __str__(){
           let text = format("({self.x}, {self.y}, {self.z})")
           return text
       }
       @overload __repr__(){
           let text = format("Vec({self.x}, {self.y}, {self.z})")
           return text
       }
   }

   let vec1 = Vec(1.0, 3.0, 0.5)
   let vec2 = Vec(1.0, 0.1, 2.0)
   let vec3 = vec1 + vec2
   println(vec1)                  // (1.0, 3.0, 0.5)
   println(vec2)                  // (1.0, 0.1, 2.0)
   println(vec3)                  // (2.0, 3.1, 2.5)
   println(format("{vec3:r}"))    // Vec(2.0, 3.1, 2.5)
   

RESERVED WORDS:
   - break, continue, define, delete, elif, else, from, fun, if,
     implements, import, lambda, let, loop, macro, match, new, return,
     struct, when, with,
     None, Ok, Err, Some, Unit, Nil
     and, or, not,
     true, false,


STATEMENTS:
   - new, fun, macro, if, match, loop, let, var, return, continue,
     break, define, import, lambda, 

OPERATORS:
   +, -, *, /, %,
   <<, >>, |, &, ^, ~,
   ==, !=, <=, >=, <, >,

BUILTIN FUNCTIONS:
   all, any,
   // not, and, or,
   print, println, eprint, eprintln, panic, format, input,
   range, linspace,
   Ok, Err,
   Some, None,

BUILTIN TYPES:
   Bool, Byte, Char,
   Number, Integer, Float, Complex,
   String,
   Array, Arraylist, ByteArray
   Tuple,
   List,
   HashSet,
   HashMap,


EXAMPLES:
---------
[1] Bool
false, true

[2] Byte
b'0'
b'c'

[3] Char
'a'
'u'

[4] Number <Integer, Float, Complex>
0b10100
0o0657
0x1cafe
123467801
3.14
9.1e-31
6.02e-23
Integer::from("0b0101")
Integer::from("123")
Float::from("3.141516")
Float::from("2.7882")
Complex::from(1.2, 4.1)

[5] String
"Hello World!"
String::from(...)

[6] Array
new Array(size)
new Array(size, init)
#[1, 2, 3, 4]
Array::from(...)

new BytesArray()
BytesArray::from()

[7] ArrayList
new ArrayList()
new ArrayList(size, init)
[1, 2, 3, 4]
ArrayList::from(...)

[8] Tuple
#(1, 1.2, "Alize", [1, 2, 3])
Tuple::from(...)

[9] List
new List()
(1, 2, 3, 5)
List::from(...)

[10] HashSet
{"C", "C++", "Rust", "Python", "JavaScript"}
new HashSet()
HashSet::from(...)

[11] HashMap
{
"lang": "Rust",
"developer": "...",
}

new HashMap()
HashMap::from(iterator)

[12] let & var
let lang = "Alize"
let num = 0x01cafe
var x = 3.14

[13] fun
fun ident(...){...}

[14] lambda
lambda(...){...}

[15] macro
macro name(...){...}

[16] struct
struct ident(...) [implements ...] {}

[17] trait
trait ident{...}

trait Animal{
    fun make_sound()
    fun get_name()
}

struct Dog(name) implements Animal{
   @property{
      [@private] self.name = name
      [@private] self.sound = "..."
   }

   @impl
   fun make_sound(){...}

   @impl
   fun get_name(){...}
}

*/

class Iterable{
public:
    virtual ~Iterable() = default;
    virtual Self next(void) = 0;
    virtual bool has_next(void) = 0;

    Self filter(Self predicate);
    Self map(Self fn);
    Self zip(Iterator other);
    Self reduce(Self fn, Self init);
    Self take(Self n);
    Self take_while(Self predicate);
    Self count(Self predicate);
    // ...
};

```
