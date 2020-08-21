
package lang.stratos.grammer;

import com.intellij.lexer.FlexLexer;
import com.intellij.psi.tree.IElementType;

import static com.intellij.psi.TokenType.BAD_CHARACTER; // Pre-defined bad character token.
import static com.intellij.psi.TokenType.WHITE_SPACE; // Pre-defined whitespace character token.
import static lang.stratos.grammer.StratosTypes; // Note that is the class which is specified as `elementTypeHolderClass`
// in bnf grammar file. This will contain all other tokens which we will use.

%%

%public
%class StratosLexer // Name of the lexer class which will be generated.
%implements FlexLexer // Name of the super class.
%function advance
%type IElementType
%unicode

// We define various Lexer rules as regular expressions first.

// If some character sequence is matched to this regex, it will be treated as an IDENTIFIER.
IDENTIFIER=[a-zA-Z_][a-zA-Z0-9_]*

// If some character sequence is matched to this regex, it will be treated as a WHITE_SPACE.
WHITE_SPACE=[ \t\n\x0B\f\r]+

// Initial state. We can specify mutiple states for more complex grammars. This corresponds to `modes` in ANTLR grammar.
%%
<YYINITIAL> {
  // In here, we match keywords. So if a keyword is found, this returns a token which corresponds to that keyword.
  // These tokens are generated using the `Ballerina.bnf` file and located in the StratosTypes `class`.
  // These tokens are Parser uses these return values to match token squence to a parser rule.
  "function"         { return FUNCTION; } // This indicates that the `function` keyword is encountered.
  "returns"          { return RETURNS; }  // This indicates that the `returns` keyword is encountered.
  "int"              { return INT; }      // This indicates that the `int` keyword is encountered.
  "string"           { return STRING; }   // This indicates that the `string` keyword is encountered.

 "{"                { return LBRACE; }
 "}"                { return RBRACE; }

  "assert"               { return ASSERT; }
  "break"                { return BREAK; }
  "case"                 { return CASE; }
  "catch"                { return CATCH; }
  "class"                { return CLASS; }
  "const"                { return CONST; }
  "continue"             { return CONTINUE; }
  "default"              { return DEFAULT; }
  "do"                   { return DO; }
  "else"                 { return ELSE; }
  "enum"                 { return ENUM; }
  "extends"              { return EXTENDS; }
  "false"                { return FALSE; }
  "final"                { return FINAL; }
  "finally"              { return FINALLY; }
  "for"                  { return FOR; }
  "if"                   { return IF; }
  "in"                   { return IN; }
  "is"                   { return IS; }
  "new"                  { return NEW; }
  "null"                 { return NULL; }
  "rethrow"              { return RETHROW; }
  "return"               { return RETURN; }
  "super"                { return SUPER; }
  "switch"               { return SWITCH; }
  "this"                 { return THIS; }
  "throw"                { return THROW; }
  "true"                 { return TRUE; }
  "try"                  { return TRY; }
  "var"                  { return VAR; }
  "void"                 { return VOID; }
  "while"                { return WHILE; }
  "with"                 { return WITH; }


// BUILT_IN_IDENTIFIER (can be used as normal identifiers)
  "abstract"             { return ABSTRACT; }
  "as"                   { return AS; }
  "covariant"            { return COVARIANT; }
  "deferred"             { return DEFERRED; }
  "export"               { return EXPORT; }
  "extension"            { return EXTENSION; }
  "external"             { return EXTERNAL; }
  "factory"              { return FACTORY; }
  "get"                  { return GET; }
  "implements"           { return IMPLEMENTS; }
  "import"               { return IMPORT; }
  "library"              { return LIBRARY; }
  "mixin"                { return MIXIN; }
  "operator"             { return OPERATOR; }
  "part"                 { return PART; }
  "set"                  { return SET; }
  "static"               { return STATIC; }
  "typedef"              { return TYPEDEF; }
  "sync"                 { return SYNC; }
  "async"                { return ASYNC; }
  "await"                { return AWAIT; }
  "yield"                { return YIELD; }

// next are not listed in spec, but they seem to have the same sense as BUILT_IN_IDENTIFIER: somewhere treated as keywords, but can be used as normal identifiers
  "on"                   { return ON; }
  "of"                   { return OF; }
  "native"               { return NATIVE; }
  "show"                 { return SHOW; }
  "hide"                 { return HIDE; }
  "late"                 { return LATE; }
  "required"             { return REQUIRED; }

  {IDENTIFIER}           { return IDENTIFIER; }
  "["                { return LBRACKET; }
  "]"                { return RBRACKET; }
  "("                { return LPAREN; }
  ")"                { return RPAREN; }
  ";"                { return SEMICOLON; }
  "-"                { return MINUS; }
  "-="               { return MINUS_EQ; }
  "--"               { return MINUS_MINUS; }
  "+"                { return PLUS; }
  "++"               { return PLUS_PLUS; }
  "+="               { return PLUS_EQ; }
  "/"                { return DIV; }
  "/="               { return DIV_EQ; }
  "*"                { return MUL; }
  "*="               { return MUL_EQ; }
  "~/"               { return INT_DIV; }
  "~/="              { return INT_DIV_EQ; }
  "%="               { return REM_EQ; }
  "%"                { return REM; }
  "~"                { return BIN_NOT; }
  "!"                { return NOT; }

  "=>"               { return EXPRESSION_BODY_DEF; }
  "="                { return EQ; }
  "=="               { return EQ_EQ; }
  "!="               { return NEQ; }
  "."                { return DOT; }
  ".."               { return DOT_DOT; }
  "?.."              { return QUEST_DOT_DOT; }
  "..."              { return DOT_DOT_DOT; }
  "...?"             { return DOT_DOT_DOT_QUEST; }
  ","                { return COMMA; }
  ":"                { return COLON; }
  ">"                { return GT; }
//  ">="               { return GT_EQ;    } breaks mixin app parsing
//  ">>"               { return GT_GT;    } breaks generics parsing
//  ">>="              { return GT_GT_EQ; } breaks mixin app parsing
  "<"                { return LT; }
  "<="               { return LT_EQ; }
  "<<"               { return LT_LT; }
  "<<="              { return LT_LT_EQ; }
  "?"                { return QUEST; }
  "?."               { return QUEST_DOT; }
  "??"               { return QUEST_QUEST; }
  "??="              { return QUEST_QUEST_EQ; }
  "|"                { return OR; }
  "|="               { return OR_EQ; }
  "||"               { return OR_OR; }
  "||="              { return OR_OR_EQ; }
  "^"                { return XOR; }
  "^="               { return XOR_EQ; }
  "&"                { return AND; }
  "&="               { return AND_EQ; }
  "&&"               { return AND_AND; }
  "&&="              { return AND_AND_EQ; }
  "@"                { return AT; }
  "#"                { return HASH; }


  // In here, we check for character sequences which matches regular expressions which we defined above.
  {IDENTIFIER}       { return IDENTIFIER; } // This indicates that a character sequence which matches to the rule
                                            // identifier is encountered.
  {WHITE_SPACE}      { return WHITE_SPACE; } // This indicates that a character sequence which matches to the rule
                                             // whitespace is encountered.
}

// If the character sequence does not match any of the above rules, we return BAD_CHARACTER which indicates that
// there is an error in the character sequence. This is used to highlight errors.
[^] { return BAD_CHARACTER; }