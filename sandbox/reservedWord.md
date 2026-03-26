## Reserved words in Alize Programming Language

BoolExprAst           : true, false
BytesExprAst          : b'c'
CharExprAst           : 'a'
IntegerExprAst        : 12345 | 0x1Aef, 0o0117 | 0b010111
FloatExprAst          : 3.14 | 9.01e-31, 6.22E30
StringExprAst         : "text"
TupleExprAst          : (expr1, expr2, ..., exprN)
ArrayExprAst          : [expr1, expr2, ..., exprN]
HashSetExprAst        : {key1, key2, ..., keyN }
HashMapExprAst        : {key1: val1, ..., keyN: valN }

UndefinedExprAst      : undefined
SelfExprAst           : self
IdentifierExprAst     : ident
OkExprAst             : Ok(expr)
ErrExprAst            : Err(expr)
SomeExprAst           : Some(expr)
NoneExprAst           : None
UnaryExprAst          : op rhsExpr
BinaryExprAst         : lhsExpr op rhsExpr
TernaryExprAst        : whenExpr
DeleteExprAst         : "delete ident"
WhenExprAst           : "when(boolExpr){okBody}{noBody}"
CompoundExprAst       : expr1, expr2, ..., exprn
LambdaExprAst         : "lambda(params){body}"
CallExprAst           : ident(...)
ExprStmtAst           : let id=expr | ver id=expr | when(expr){okBody}{noBody}
FromExprStmtAst       : import {id1, id2, ...} from moduleId
AliasExprStmtAst      : alias newId = oldId
ReturnExprStmtAst     : return [expr]
NewExprStmtAst        : new Constructor
TypeStmtAst           : type ident = new Object;
StructStmtAst         : struct ident [implements trait1, trait2, ...]{ body }
FunStmtAst            : fun ident(params){ body }
MacroStmtAst          : macro ident(params){ body }
LetStmtAst            : let ident = expr
VarStmtAst            : var ident [= expr]
AtDefineStmtAst       : @define(ident, expr)
AtOperatorStmtAst     : @operator ident(params){ body }
AtPropertiesStmtAst   : @properties{ property1, property2, ..., propertyN}
AtStaticStmtAst       : @static ident(params){ body }
AtTestingStmtAst      : @testing{ body }
AtTestStmtAst         : @test ident{ body }
AtSetupStmtAst        : @setup{ body }
AtCleanupStmtAst      : @cleanup{ body }
ImportStmtAst         : import moduleId | moduleId::ident
TraitStmtAst          : trait ident{ body }
ImplementsStmtAst     : implements traitId1 [, ..., traitIdN]
AtImplStmtAst         : @impl ident(params){ body }
IfStmtAst             : if(testExpr1){body1}elif(testExpr2){body2}[...]else{body}
ForStmtAst            : for(ident : iteratorExpr){ body }
LoopStmtAst           : loop{ body }
MatchStmtAst          : match expr { body }
CaseStmtAst           : case(expr){ body }
AtScopeStmtAst        : @scope{ body }
BreakStmtAst          : break
ContinueStmtAst       : continue
