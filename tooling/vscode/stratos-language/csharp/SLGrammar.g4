/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

grammar SLGrammar;

// Базовые символы и имена
Colon: ':';
Semicolon: ';';
Comma: ',';
Point: '.';

Variable: 'variable';
Const: 'const';
Length: 'length';
Let: 'let';
Return: 'return';
Input: 'input';
Output: 'output';
Call: 'call';

If: 'if';
Then: 'then';
Else: 'else';
While: 'while';
Repeat: 'repeat';
Elseif: 'elseif';
Do: 'do';

Raw: 'raw';

// Арифметика и булевы токены
AddOp: '+';
SubOp: '-';
MulOp: '*';
DivOp: '/';
ModOp: '%';

BoolOr: '||';
BoolAnd: '&&';

BoolEq: '==';
BoolNeq: '!=';
BoolG: '>';
BoolL: '<';
BoolGeq: '>=';
BoolLeq: '<=';
BoolNot: '!';


// Объявление и импорт модуля
ModuleToken: 'module';
ImportToken: 'import';

// Начало и конец (конец используется ещё и в условиях/циклах/функциях)
Start: 'start';
End: 'end';

Function: 'function';
Procedure: 'procedure';
Using: 'using';

// круглые и квадратные скобки
LBrace: '(';
RBrace: ')';

LSBrace: '[';
RSBrace: ']';

LABrace: '{';
RABrace: '}';

AssignToken: ':='; // Присваивание

// ТИПЫ
fragment Integer: 'integer'; // | 'int'
fragment Real: 'real';
fragment Character: 'character'; // | 'char'
fragment Boolean: 'boolean'; // | 'bool'

// Токены для указателей
New: 'new'; // выделение памяти
Nil: 'nil'; // значение null
Pointer: 'pointer'; // указатель

typeName: scalarType | arrayType;
ptrType: Pointer (LBrace id RBrace)?; // id -- тип
scalarType: simpleType | functionalType | id | ptrType;
functionalType: procedureType | functionType; // Функциональный тип = процедуры и функции
simpleType : SimpleType; // Встроенные типы 

functionType: Procedure functionalArgList Colon typeName; // Тип функция
procedureType: Function functionalArgList;                // Тип процедура

functionalArgList: LBrace (functionalArg(Comma functionalArg) | /* nothing */) RBrace;
functionalArg: FunctionArgModifier typeName;

ArrayToken: 'array'; // Массивный тип
arrayType: ArrayToken (arrayDimention)+ scalarType;
arrayDimention : LSBrace RSBrace;

FunctionArgModifier : ArgValModifier | ArgRefModifier ; // Передача аргументов в функцию по значению и ссылке
fragment ArgValModifier: 'val';
fragment ArgRefModifier: 'ref';

AccessModifier: PublicModifier | PrivateModifier; // Модификаторы доступа
fragment PublicModifier: 'public';
fragment PrivateModifier: 'private';

// Работа с модулями тут =
start: moduleImportList module;
moduleImportList: (moduleImport | raw)*;
moduleImport: ImportToken Id;
module: ModuleToken Id moduleDeclare moduleEntry;

moduleDeclare: (functionDeclare | procedureDeclare | raw | varModuleDeclare | classDeclare)*; // Определение модуля 


/// Object-Oriented part!
// Lexer rules
Class: 'class';

Inherit: 'inherit'; // Класс наследует что-либо
Base: 'base'; // От класса можно наследоваться 

Abstract: 'abstract';

Override: 'override'; // Метод переопределяется

// Parser rules
base_head: (Base)?;
inherit_head: (Inherit LBrace id RBrace)?;

classDeclare: AccessModifier base_head Class Id inherit_head classStatements End;
//classDeclare: AccessModifier Id Class;
classStatements: (methodDeclare | raw | fieldDeclare)*;
methodDeclare: methodFuncDeclare | methodProcDeclare | methodFuncAbstract | methodProcAbstract;

thisHeader: LBrace functionalDeclareArg  RBrace;

methodFuncAbstract: AccessModifier Abstract thisHeader Function functionalDeclareArgList Colon typeName Id Semicolon;
methodProcAbstract: AccessModifier Abstract thisHeader Procedure functionalDeclareArgList Id Semicolon;

methodFuncDeclare: AccessModifier (Override)? thisHeader Function functionalDeclareArgList Colon typeName Id statementSeq End;
methodProcDeclare: AccessModifier (Override)? thisHeader Procedure functionalDeclareArgList Id statementSeq End;

fieldDeclare: AccessModifier varDeclare Semicolon;
// end OOP

functionDeclare: AccessModifier Function functionalDeclareArgList Colon typeName Id statementSeq End; // Функции
procedureDeclare: AccessModifier Procedure functionalDeclareArgList Id statementSeq End; // Процедура
varModuleDeclare: AccessModifier declare Semicolon;

functionalDeclareArgList : LBrace (functionalDeclareArg (Comma functionalDeclareArg)* | /* нет аргументов */ )  RBrace; 

functionalDeclareArg : FunctionArgModifier typeName Id;

moduleEntry: Start statementSeq End Id Point;

statementSeq: (statement | raw)*;

statement: simpleStatement | complexStatement;

simpleStatement: (declare | let | input | output | return_val | call | using) Semicolon; // Операторы
complexStatement: if_cond | while_cond | repeat;

declare: constDeclare | varDeclare; // Определение констант и переменных


// Определение констант и переменных
// Константы -- простые типы языка (на текущий момент)

constDeclare: Const typeName Id AssignToken (mathExpression | boolExpression);
varDeclare: scalarDeclare | arrayDeclare | ptrDeclare;

scalarDeclare: Variable scalarType Id (AssignToken mathExpression | AssignToken boolExpression)?;
arrayDeclare: arrayDeclareType Id (AssignToken mathExpression)?; // выражение
ptrDeclare: ptrType Id (AssignToken mathExpression)?; // всего скорее, без адресной арифметики -- нужен только expAtom для указателей -- new, nil

arrayDeclareType: ArrayToken (arrayDeclareDimention)+ scalarType;
arrayDeclareDimention: LSBrace mathExpression RSBrace;
arrayElement: id (arrayDeclareDimention)+;
arrayLenProperty: id Point Length LBrace IntValue RBrace;

let: Let (simpleLet | arrayLet);
simpleLet : id AssignToken mathExpression | id AssignToken boolExpression | id AssignToken let;
arrayLet: arrayElement AssignToken mathExpression | arrayElement AssignToken boolExpression | arrayElement AssignToken let;
using: Using id AssignToken id;

return_val: Return (exp)?;
input: Input id (Comma id)*;
output: Output outputArgument (Comma outputArgument)*;
outputArgument: StringLiteral | exp;

call: Call id LBrace callArgList RBrace;
callArgList: (callArg (Comma callArg)*) | /*nothing*/;
callArg: exp; // Некое выражение в аргументе

call_func: id LBrace callArgList RBrace;

// Условия
if_cond : If LBrace boolExpression RBrace Then statementSeq End #IfSingle
   | If LBrace boolExpression RBrace Then statementSeq (Elseif LBrace boolExpression RBrace Then statementSeq)* Else statementSeq End #IfElseIfElse
   ;

while_cond: While LBrace boolExpression RBrace Do statementSeq End;
repeat: Repeat statementSeq While LBrace boolExpression RBrace;

// Разбор математических правил

mathExpression
	: mathTerm #MathExpEmpty
	| mathTerm AddOp mathExpression #MathExpSum 
	| mathTerm SubOp mathExpression #MathExpSub
	;

mathTerm
	: mathFactor #MathTermEmpty
	| mathFactor MulOp mathTerm #MathTermMul 
	| mathFactor DivOp mathTerm #MathTermDiv 
	| mathFactor ModOp mathTerm #MathTermMod
	;

mathFactor
	: expAtom #MathFactorEmpty
	| LBrace mathExpression RBrace #MathFactorBrackets
	| AddOp mathFactor #MathFactorUnaryPlus
	| SubOp mathFactor #MathFactorUnaryMinus
	;

// Разбор булевых выражений

boolExpression
	: boolAnd #BoolOrEmpty
	| boolAnd BoolOr boolExpression #LogicOr
	;

boolAnd
	: boolEquality #BoolAndEmpty
	| boolEquality BoolAnd boolAnd #LogicAnd
	;

boolEquality
	: boolInequality #BoolEqualityEmpty
	| mathExpression BoolEq mathExpression #MathEqual
	| boolInequality BoolEq boolEquality #BoolEqual
	| mathExpression BoolNeq mathExpression #MathNotEqual
	| boolInequality BoolNeq boolEquality #BoolNotEqual 
	;

boolInequality
	: boolFactor #BoolInequalityEmpty
	| mathExpression BoolG mathExpression #Bigger
	| mathExpression BoolL mathExpression #Lesser
	| mathExpression BoolGeq mathExpression #BiggerOrEqual
	| mathExpression BoolLeq mathExpression #LesserOrEqual
	;

boolFactor
	: expAtom #BoolAtomEmpty 
	| BoolNot expAtom #Not 
	| LBrace boolExpression RBrace #BoolAtomBrackets 
	| BoolNot LBrace boolExpression RBrace #BoolAtomBracketsNot
	;

newExp: New LBrace id RBrace;
ptrExpAtom: newExp | Nil;
expAtom: call | arrayLenProperty | arrayElement | id | (IntValue | RealValue | BoolValue) | call_func | StringLiteral | ptrExpAtom | array;
// Точки -- для указания связт модуль-функция
id: (Id Point)? Id;
SimpleType: Real | Integer | Boolean | Character;

array: LABrace (expAtom (Comma expAtom)*)?  RABrace;

exp: mathExpression | boolExpression;

raw: 'raw' any End; // Вставка целевого кода -- мы же не будем писать стандартные модули на чистом семантике!
any: (.)*?;

// Многие простые типы-константы

fragment Digit: [0-9]; // цифра

IntValue: Digit+; // Целое число
RealValue: Digit*Point?Digit+([eE][-+]?Digit+)?; // Вещественное значение
BoolValue: 'true' | 'false'; // Булево значение

Id: [_a-zA-Z][_a-zA-Z0-9]*; // Идентификатор

StringLiteral:	'"' StringCharacter* '"'; // Строковый литерал
fragment StringCharacter: ~["] | EscapeSequence; // Символ
fragment EscapeSequence : '\\' [btnfr"'\\]; // escape-символы типа \n \t ... либо группа 1 символа

Comment: ('//' ~[\r\n]* | '/*' .*? '*/') -> skip;
Ws: [ \t\r\n] -> skip;