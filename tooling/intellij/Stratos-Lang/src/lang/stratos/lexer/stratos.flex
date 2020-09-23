package lang.stratos.lexer; // Package which will be the lexer generation location.
//
import java.util.*;
import com.intellij.lexer.FlexLexer;
import com.intellij.psi.tree.IElementType;
import lang.stratos.language.StratosTokenType;
import static com.intellij.psi.TokenType.BAD_CHARACTER; // Pre-defined bad character token.
import static com.intellij.psi.TokenType.WHITE_SPACE; // Pre-defined whitespace character token.
import static lang.stratos.psi.StratosTypes.*; // Note that is the class which is specified as `elementTypeHolderClass`
// in bnf grammar file. This will contain all other tokens which we will use.


@SuppressWarnings("DuplicateBranchesInSwitch")
%%
%{
  private static final class State {
    final int lBraceCount;
    final int state;

    private State(int state, int lBraceCount) {
      this.state = state;
      this.lBraceCount = lBraceCount;
    }

    @Override
    public String toString() {
      return "yystate = " + state + (lBraceCount == 0 ? "" : "lBraceCount = " + lBraceCount);
    }
  }

  protected final Stack<State> myStateStack = new Stack<>();
  protected int myLeftBraceCount;

  private void pushState(int state) {
    myStateStack.push(new State(yystate(), myLeftBraceCount));
    myLeftBraceCount = 0;
    yybegin(state);
  }

  private void popState() {
    State state = myStateStack.pop();
    myLeftBraceCount = state.lBraceCount;
    yybegin(state.state);
  }

  _StratosLexer() {
    this(null);
  }
%}

%class _StratosLexer
%implements FlexLexer
%unicode
%function advance
%type IElementType
%eof{
  myLeftBraceCount = 0;
  myStateStack.clear();
%eof}

%xstate MULTI_LINE_COMMENT_STATE QUO_STRING THREE_QUO_STRING APOS_STRING THREE_APOS_STRING SHORT_TEMPLATE_ENTRY LONG_TEMPLATE_ENTRY

DIGIT=[0-9]
HEX_DIGIT=[0-9a-fA-F]
LETTER=[a-z]|[A-Z]
WHITE_SPACE=[ \n\t\f]+
PROGRAM_COMMENT="#""!"[^\n]*
SINGLE_LINE_COMMENT="/""/"[^\n]*
SINGLE_LINE_DOC_COMMENT="/""/""/"[^\n]*
SINGLE_LINE_COMMENTED_COMMENT="/""/""/""/"[^\n]*

MULTI_LINE_DEGENERATE_COMMENT = "/*" "*"+ "/"
MULTI_LINE_COMMENT_START      = "/*"
MULTI_LINE_DOC_COMMENT_START  = "/**"
MULTI_LINE_COMMENT_END        = "*/"

RAW_SINGLE_QUOTED_STRING= "r" ((\" ([^\"\n])* \"?) | ("'" ([^\'\n])* \'?))
RAW_TRIPLE_QUOTED_STRING= "r" ({RAW_TRIPLE_QUOTED_LITERAL} | {RAW_TRIPLE_APOS_LITERAL})

RAW_TRIPLE_QUOTED_LITERAL = {THREE_QUO}  ([^\"] | \"[^\"] | \"\"[^\"])* {THREE_QUO}?
RAW_TRIPLE_APOS_LITERAL   = {THREE_APOS} ([^\'] | \'[^\'] | \'\'[^\'])* {THREE_APOS}?

THREE_QUO =  (\"\"\")
THREE_APOS = (\'\'\')

SHORT_TEMPLATE_ENTRY=\${IDENTIFIER_NO_DOLLAR}
LONG_TEMPLATE_ENTRY_START=\$\{

IDENTIFIER_START_NO_DOLLAR={LETTER}|"_"
IDENTIFIER_START={IDENTIFIER_START_NO_DOLLAR}|"$"
IDENTIFIER_PART_NO_DOLLAR={IDENTIFIER_START_NO_DOLLAR}|{DIGIT}
IDENTIFIER_PART={IDENTIFIER_START}|{DIGIT}
IDENTIFIER={IDENTIFIER_START}{IDENTIFIER_PART}*
IDENTIFIER_NO_DOLLAR={IDENTIFIER_START_NO_DOLLAR}{IDENTIFIER_PART_NO_DOLLAR}*

NUMERIC_LITERAL = {NUMBER} | {HEX_NUMBER}
NUMBER = ({DIGIT}+ ("." {DIGIT}+)? {EXPONENT}?) | ("." {DIGIT}+ {EXPONENT}?)
EXPONENT = [Ee] ["+""-"]? {DIGIT}*
HEX_NUMBER = 0 [Xx] {HEX_DIGIT}*

%%

<YYINITIAL> "{"                { return OPEN_BRACKETS; }
<YYINITIAL> "}"                { return CLOSE_BRACKETS; }
<LONG_TEMPLATE_ENTRY> "{"      { myLeftBraceCount++; return CLOSE_BRACKETS; }
<LONG_TEMPLATE_ENTRY> "}"      {
                                   if (myLeftBraceCount == 0) {
                                     popState();
                                     return LONG_TEMPLATE_ENTRY_END;
                                   }
                                   myLeftBraceCount--;
                                   return OPEN_BRACKETS;
                               }

<YYINITIAL, LONG_TEMPLATE_ENTRY> {WHITE_SPACE}                   { return WHITE_SPACE;             }

// single-line comments
<YYINITIAL, LONG_TEMPLATE_ENTRY> {SINGLE_LINE_COMMENTED_COMMENT} { return SINGLE_LINE_COMMENT; }
<YYINITIAL, LONG_TEMPLATE_ENTRY> {SINGLE_LINE_DOC_COMMENT}       { return SINGLE_LINE_DOC_COMMENT; }
<YYINITIAL, LONG_TEMPLATE_ENTRY> {SINGLE_LINE_COMMENT}           { return SINGLE_LINE_COMMENT;     }
<YYINITIAL>                      {PROGRAM_COMMENT}               { return SINGLE_LINE_COMMENT;     }

// multi-line comments
<YYINITIAL, LONG_TEMPLATE_ENTRY> {MULTI_LINE_DEGENERATE_COMMENT} { return MULTI_LINE_COMMENT;      } // without this rule /*****/ is parsed as doc comment and /**/ is parsed as not closed doc comment

// next rules return temporary IElementType's that are rplaced with TokenTypesSets#MULTI_LINE_COMMENT or DartTokenTypesSets#MULTI_LINE_DOC_COMMENT in com.jetbrains.lang.dart.lexer.DartLexer
<YYINITIAL, LONG_TEMPLATE_ENTRY> {MULTI_LINE_DOC_COMMENT_START}  { pushState(MULTI_LINE_COMMENT_STATE); return MULTI_LINE_DOC_COMMENT_START;                                                             }
<YYINITIAL, LONG_TEMPLATE_ENTRY> {MULTI_LINE_COMMENT_START}      { pushState(MULTI_LINE_COMMENT_STATE); return MULTI_LINE_COMMENT_START;                                                                 }

<MULTI_LINE_COMMENT_STATE>       {MULTI_LINE_COMMENT_START}      { pushState(MULTI_LINE_COMMENT_STATE); return MULTI_LINE_COMMENT_BODY;                                                                  }
<MULTI_LINE_COMMENT_STATE>       [^]                             {                                      return MULTI_LINE_COMMENT_BODY;                                                                  }
<MULTI_LINE_COMMENT_STATE>       {MULTI_LINE_COMMENT_END}        { popState();                          return yystate() == MULTI_LINE_COMMENT_STATE
                                                                                                               ? MULTI_LINE_COMMENT_BODY // inner comment closed
                                                                                                               : MULTI_LINE_COMMENT_END; }

// reserved words
<YYINITIAL, LONG_TEMPLATE_ENTRY> "assert"               { return ASSERT; }
<YYINITIAL, LONG_TEMPLATE_ENTRY> "break"                { return BREAK; }
<YYINITIAL, LONG_TEMPLATE_ENTRY> "function"                { return FUNCTION; }
<YYINITIAL, LONG_TEMPLATE_ENTRY> "yeild"                { return YIELD; }
<YYINITIAL, LONG_TEMPLATE_ENTRY> "returns"                { return RETURNS; }
<YYINITIAL, LONG_TEMPLATE_ENTRY> "package"                { return PACKAGE; }
<YYINITIAL, LONG_TEMPLATE_ENTRY> "import"                { return IMPORT; }
<YYINITIAL, LONG_TEMPLATE_ENTRY> "if"                { return IF; }
<YYINITIAL, LONG_TEMPLATE_ENTRY> "else"                { return ELSE; }
<YYINITIAL, LONG_TEMPLATE_ENTRY> "when"                { return WHEN; }

<YYINITIAL, LONG_TEMPLATE_ENTRY> "%"                { return MODULAS; }
<YYINITIAL, LONG_TEMPLATE_ENTRY> "%"                { return MODULAS; }
<YYINITIAL, LONG_TEMPLATE_ENTRY> "/"                { return DIVIDE; }
<YYINITIAL, LONG_TEMPLATE_ENTRY> "*"                { return MULTIPLY; }
<YYINITIAL, LONG_TEMPLATE_ENTRY> "+"                { return ADD; }
<YYINITIAL, LONG_TEMPLATE_ENTRY> "-"                { return MINUS; }
<YYINITIAL, LONG_TEMPLATE_ENTRY> ">"                { return GREATER_THAN; }
<YYINITIAL, LONG_TEMPLATE_ENTRY> "<"                { return LESS_THAN; }
<YYINITIAL, LONG_TEMPLATE_ENTRY> "="                { return ASSIGN; }
<YYINITIAL, LONG_TEMPLATE_ENTRY> "=="                { return EQUAL; }
<YYINITIAL, LONG_TEMPLATE_ENTRY> ":"                { return COLON; }
<YYINITIAL, LONG_TEMPLATE_ENTRY> "!"                { return NOT; }
<YYINITIAL, LONG_TEMPLATE_ENTRY> ";"                { return SEMI_COLON; }

<YYINITIAL, LONG_TEMPLATE_ENTRY> "#"                { return HASH; }
<YYINITIAL, LONG_TEMPLATE_ENTRY> "@"                { return AT; }


<YYINITIAL, LONG_TEMPLATE_ENTRY> "("                { return OPEN_BRACE; }
<YYINITIAL, LONG_TEMPLATE_ENTRY> ")"                { return CLOSE_BRACE; }

<YYINITIAL, LONG_TEMPLATE_ENTRY> "let"                { return LET; }
<YYINITIAL, LONG_TEMPLATE_ENTRY> "var"                { return VARIABLE; }
<YYINITIAL, LONG_TEMPLATE_ENTRY> "val"                { return VALUE; }
<YYINITIAL, LONG_TEMPLATE_ENTRY> "mut"                { return MUTABLE; }

<YYINITIAL, LONG_TEMPLATE_ENTRY> "as"                { return AS; }
<YYINITIAL, LONG_TEMPLATE_ENTRY> "is"                { return IS; }

<YYINITIAL, LONG_TEMPLATE_ENTRY> "int"                { return INT; }
<YYINITIAL, LONG_TEMPLATE_ENTRY> "string"                { return STRING; }
<YYINITIAL, LONG_TEMPLATE_ENTRY> "double"                { return DOUBLE; }
<YYINITIAL, LONG_TEMPLATE_ENTRY> "unit"                { return UNIT; }
<YYINITIAL, LONG_TEMPLATE_ENTRY> "bool"                { return BOOL; }

<YYINITIAL, LONG_TEMPLATE_ENTRY> "true"                { return TRUE; }
<YYINITIAL, LONG_TEMPLATE_ENTRY> "false"                { return FALSE; }

<YYINITIAL, LONG_TEMPLATE_ENTRY> "alias"                { return ALIAS; }
<YYINITIAL, LONG_TEMPLATE_ENTRY> "override"                { return OVERRIDE; }


<YYINITIAL, LONG_TEMPLATE_ENTRY> "struct"                { return STRUCT; }
<YYINITIAL, LONG_TEMPLATE_ENTRY> "enum"                { return ENUM; }

<YYINITIAL, LONG_TEMPLATE_ENTRY> "implement"                { return IMPLEMENTS; }
<YYINITIAL, LONG_TEMPLATE_ENTRY> "extends"                { return EXTENDS; }






<YYINITIAL, LONG_TEMPLATE_ENTRY> {NUMERIC_LITERAL} { return NUMBER; }

// raw strings
<YYINITIAL, LONG_TEMPLATE_ENTRY> {RAW_TRIPLE_QUOTED_STRING} { return RAW_TRIPLE_QUOTED_STRING; }
<YYINITIAL, LONG_TEMPLATE_ENTRY> {RAW_SINGLE_QUOTED_STRING} { return RAW_SINGLE_QUOTED_STRING; }

// string start
<YYINITIAL, LONG_TEMPLATE_ENTRY>      \"                  { pushState(QUO_STRING);        return OPEN_QUOTE;    }
<YYINITIAL, LONG_TEMPLATE_ENTRY>      \'                  { pushState(APOS_STRING);       return OPEN_QUOTE;    }
<YYINITIAL, LONG_TEMPLATE_ENTRY>      {THREE_QUO}         { pushState(THREE_QUO_STRING);  return OPEN_QUOTE;    }
<YYINITIAL, LONG_TEMPLATE_ENTRY>      {THREE_APOS}        { pushState(THREE_APOS_STRING); return OPEN_QUOTE;    }
// correct string end
<QUO_STRING>                          \"                  { popState();                   return CLOSING_QUOTE; }
<APOS_STRING>                         \'                  { popState();                   return CLOSING_QUOTE; }
<THREE_QUO_STRING>                    {THREE_QUO}         { popState();                   return CLOSING_QUOTE; }
<THREE_APOS_STRING>                   {THREE_APOS}        { popState();                   return CLOSING_QUOTE; }
<QUO_STRING, APOS_STRING>             \n                  { popState();                   return WHITE_SPACE;   } // not closed single-line string literal. Do not return BAD_CHARACTER here because red highlighting of bad \n looks awful
// string content
<QUO_STRING>                          ([^\\\"\n\$] | (\\ [^\n]))*   {                return REGULAR_STRING_PART; }
<APOS_STRING>                         ([^\\\'\n\$] | (\\ [^\n]))*   {                return REGULAR_STRING_PART; }
<THREE_QUO_STRING>                    ([^\\\"\$])*                  {                return REGULAR_STRING_PART; }
<THREE_QUO_STRING>                    (\"[^\"]) | (\"\"[^\"])       { yypushback(1); return REGULAR_STRING_PART; } // pushback because we could capture '\' that escapes something
<THREE_APOS_STRING>                   ([^\\\'\$])*                  {                return REGULAR_STRING_PART; }
<THREE_APOS_STRING>                   (\'[^\']) | (\'\'[^\'])       { yypushback(1); return REGULAR_STRING_PART; } // pushback because we could capture '\' that escapes something
<THREE_QUO_STRING, THREE_APOS_STRING> (\\[^])                       {                return REGULAR_STRING_PART; } // escape sequence
// bad string interpolation (no identifier after '$')
<QUO_STRING, APOS_STRING, THREE_QUO_STRING, THREE_APOS_STRING> \$   { return SHORT_TEMPLATE_ENTRY_START; }
// short string interpolation
<QUO_STRING, APOS_STRING, THREE_QUO_STRING, THREE_APOS_STRING> {SHORT_TEMPLATE_ENTRY}      { pushState(SHORT_TEMPLATE_ENTRY);
                                                                                             yypushback(yylength() - 1);
                                                                                             return SHORT_TEMPLATE_ENTRY_START;}
// long string interpolation
<QUO_STRING, APOS_STRING, THREE_QUO_STRING, THREE_APOS_STRING> {LONG_TEMPLATE_ENTRY_START} { pushState(LONG_TEMPLATE_ENTRY);
                                                                                             return LONG_TEMPLATE_ENTRY_START; }
// Only *this* keyword is itself an expression valid in this position
// *null*, *true* and *false* are also keywords and expression, but it does not make sense to put them
// in a string template for it'd be easier to just type them in without a dollar
<SHORT_TEMPLATE_ENTRY> "this"          { popState(); return THIS; }
<SHORT_TEMPLATE_ENTRY> {IDENTIFIER_NO_DOLLAR}    { popState(); return IDENTIFIER; }

<YYINITIAL, MULTI_LINE_COMMENT_STATE, QUO_STRING, THREE_QUO_STRING, APOS_STRING, THREE_APOS_STRING, SHORT_TEMPLATE_ENTRY, LONG_TEMPLATE_ENTRY> [^] { return BAD_CHARACTER; }
