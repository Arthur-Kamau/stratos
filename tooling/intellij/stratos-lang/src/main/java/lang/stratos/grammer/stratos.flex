
//package com.jetbrains.lang.dart.lexer;
//
//import java.util.*;
//import com.intellij.lexer.FlexLexer;
//import com.intellij.psi.tree.IElementType;
//import static com.jetbrains.lang.dart.DartTokenTypes.*;
//import static com.jetbrains.lang.dart.DartTokenTypesSets.*;
//import static com.jetbrains.lang.dart.lexer.DartLexer.*;

package lang.stratos.grammer;

import com.intellij.lexer.FlexLexer;
import com.intellij.psi.tree.IElementType;
import lang.stratos.grammer.types.StratosTypes; // org.intellij.sdk.language.psi.StratosTypes;
//import lang.stratos.grammer.types.StratosTokenType; //com.intellij.psi.TokenType;
import java.util.Stack;
import com.intellij.psi.TokenType;

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

  StratosLexer() {
    this(null);
  }
%}

%class StratosLexer
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

<YYINITIAL> "{"                { return StratosTypes.LBRACE; }
<YYINITIAL> "}"                { return StratosTypes.RBRACE; }
<LONG_TEMPLATE_ENTRY> "{"      { myLeftBraceCount++; return StratosTypes.LBRACE; }
<LONG_TEMPLATE_ENTRY> "}"      {
                                   if (myLeftBraceCount == 0) {
                                     popState();
                                     return StratosTypes.LONG_TEMPLATE_ENTRY_END;
                                   }
                                   myLeftBraceCount--;
                                   return StratosTypes.RBRACE;
                               }

<YYINITIAL, LONG_TEMPLATE_ENTRY> {WHITE_SPACE}                   { return StratosTypes.WHITE_SPACE;             }

// single-line comments
<YYINITIAL, LONG_TEMPLATE_ENTRY> {SINGLE_LINE_COMMENTED_COMMENT} { return  StratosTypes.SINGLE_LINE_COMMENT; }
<YYINITIAL, LONG_TEMPLATE_ENTRY> {SINGLE_LINE_DOC_COMMENT}       { return  StratosTypes.SINGLE_LINE_DOC_COMMENT; }
<YYINITIAL, LONG_TEMPLATE_ENTRY> {SINGLE_LINE_COMMENT}           { return  StratosTypes.SINGLE_LINE_COMMENT;     }
<YYINITIAL>                      {PROGRAM_COMMENT}               { return  StratosTypes.SINGLE_LINE_COMMENT;     }

// multi-line comments
<YYINITIAL, LONG_TEMPLATE_ENTRY> {MULTI_LINE_DEGENERATE_COMMENT} { return   StratosTypes.MULTI_LINE_COMMENT;      } // without this rule /*****/ is parsed as doc comment and /**/ is parsed as not closed doc comment

// next rules return  StratosTypes. temporary IElementType's that are rplaced with DartTokenTypesSets#MULTI_LINE_COMMENT or DartTokenTypesSets#MULTI_LINE_DOC_COMMENT in com.jetbrains.lang.dart.lexer.DartLexer
<YYINITIAL, LONG_TEMPLATE_ENTRY> {MULTI_LINE_DOC_COMMENT_START}  { pushState(MULTI_LINE_COMMENT_STATE); return   StratosTypes.MULTI_LINE_DOC_COMMENT_START;                                                             }
<YYINITIAL, LONG_TEMPLATE_ENTRY> {MULTI_LINE_COMMENT_START}      { pushState(MULTI_LINE_COMMENT_STATE); return   StratosTypes.MULTI_LINE_COMMENT_START;                                                                 }

<MULTI_LINE_COMMENT_STATE>       {MULTI_LINE_COMMENT_START}      { pushState(MULTI_LINE_COMMENT_STATE); return   StratosTypes.MULTI_LINE_COMMENT_BODY;                                                                  }
<MULTI_LINE_COMMENT_STATE>       [^]                             {                                      return   StratosTypes.MULTI_LINE_COMMENT_BODY;                                                                  }
<MULTI_LINE_COMMENT_STATE>       {MULTI_LINE_COMMENT_END}        { popState();                          return   yystate() == MULTI_LINE_COMMENT_STATE
                                                                                                               ? StratosTypes.MULTI_LINE_COMMENT_BODY // inner comment closed
                                                                                                               : StratosTypes.MULTI_LINE_COMMENT_END; }

// reserved words
<YYINITIAL, LONG_TEMPLATE_ENTRY> "assert"               { return  StratosTypes.ASSERT; }
<YYINITIAL, LONG_TEMPLATE_ENTRY> "break"                { return  StratosTypes.BREAK; }
<YYINITIAL, LONG_TEMPLATE_ENTRY> "case"                 { return  StratosTypes.CASE; }
<YYINITIAL, LONG_TEMPLATE_ENTRY> "catch"                { return  StratosTypes.CATCH; }
<YYINITIAL, LONG_TEMPLATE_ENTRY> "class"                { return  StratosTypes.CLASS; }
<YYINITIAL, LONG_TEMPLATE_ENTRY> "const"                { return  StratosTypes.CONST; }
<YYINITIAL, LONG_TEMPLATE_ENTRY> "continue"             { return  StratosTypes.CONTINUE; }
<YYINITIAL, LONG_TEMPLATE_ENTRY> "default"              { return  StratosTypes.DEFAULT; }
<YYINITIAL, LONG_TEMPLATE_ENTRY> "do"                   { return  StratosTypes.DO; }
<YYINITIAL, LONG_TEMPLATE_ENTRY> "else"                 { return  StratosTypes.ELSE; }
<YYINITIAL, LONG_TEMPLATE_ENTRY> "enum"                 { return  StratosTypes.ENUM; }
<YYINITIAL, LONG_TEMPLATE_ENTRY> "extends"              { return  StratosTypes.EXTENDS; }
<YYINITIAL, LONG_TEMPLATE_ENTRY> "false"                { return  StratosTypes.FALSE; }
<YYINITIAL, LONG_TEMPLATE_ENTRY> "final"                { return  StratosTypes.FINAL; }
<YYINITIAL, LONG_TEMPLATE_ENTRY> "finally"              { return  StratosTypes.FINALLY; }
<YYINITIAL, LONG_TEMPLATE_ENTRY> "for"                  { return  StratosTypes.FOR; }
<YYINITIAL, LONG_TEMPLATE_ENTRY> "if"                   { return  StratosTypes.IF; }
<YYINITIAL, LONG_TEMPLATE_ENTRY> "in"                   { return  StratosTypes.IN; }
<YYINITIAL, LONG_TEMPLATE_ENTRY> "is"                   { return  StratosTypes.IS; }
<YYINITIAL, LONG_TEMPLATE_ENTRY> "new"                  { return  StratosTypes.NEW; }
<YYINITIAL, LONG_TEMPLATE_ENTRY> "null"                 { return  StratosTypes.NULL; }
<YYINITIAL, LONG_TEMPLATE_ENTRY> "rethrow"              { return  StratosTypes.RETHROW; }
<YYINITIAL, LONG_TEMPLATE_ENTRY> "return  StratosTypes."               { return  StratosTypes.RETURN; }
<YYINITIAL, LONG_TEMPLATE_ENTRY> "super"                { return  StratosTypes.SUPER; }
<YYINITIAL, LONG_TEMPLATE_ENTRY> "switch"               { return  StratosTypes.SWITCH; }
<YYINITIAL, LONG_TEMPLATE_ENTRY> "this"                 { return  StratosTypes.THIS; }
<YYINITIAL, LONG_TEMPLATE_ENTRY> "throw"                { return  StratosTypes.THROW; }
<YYINITIAL, LONG_TEMPLATE_ENTRY> "true"                 { return  StratosTypes.TRUE; }
<YYINITIAL, LONG_TEMPLATE_ENTRY> "try"                  { return  StratosTypes.TRY; }
<YYINITIAL, LONG_TEMPLATE_ENTRY> "var"                  { return  StratosTypes.VAR; }
<YYINITIAL, LONG_TEMPLATE_ENTRY> "void"                 { return  StratosTypes.VOID; }
<YYINITIAL, LONG_TEMPLATE_ENTRY> "while"                { return  StratosTypes.WHILE; }
<YYINITIAL, LONG_TEMPLATE_ENTRY> "with"                 { return  StratosTypes.WITH; }


// BUILT_IN_IDENTIFIER (can be used as normal identifiers)
<YYINITIAL, LONG_TEMPLATE_ENTRY> "abstract"             { return  StratosTypes.ABSTRACT; }
<YYINITIAL, LONG_TEMPLATE_ENTRY> "as"                   { return  StratosTypes.AS; }
<YYINITIAL, LONG_TEMPLATE_ENTRY> "covariant"            { return  StratosTypes.COVARIANT; }
<YYINITIAL, LONG_TEMPLATE_ENTRY> "deferred"             { return  StratosTypes.DEFERRED; }
<YYINITIAL, LONG_TEMPLATE_ENTRY> "export"               { return  StratosTypes.EXPORT; }
<YYINITIAL, LONG_TEMPLATE_ENTRY> "extension"            { return  StratosTypes.EXTENSION; }
<YYINITIAL, LONG_TEMPLATE_ENTRY> "external"             { return  StratosTypes.EXTERNAL; }
<YYINITIAL, LONG_TEMPLATE_ENTRY> "factory"              { return  StratosTypes.FACTORY; }
<YYINITIAL, LONG_TEMPLATE_ENTRY> "get"                  { return  StratosTypes.GET; }
<YYINITIAL, LONG_TEMPLATE_ENTRY> "implements"           { return  StratosTypes.IMPLEMENTS; }
<YYINITIAL, LONG_TEMPLATE_ENTRY> "import"               { return  StratosTypes.IMPORT; }
<YYINITIAL, LONG_TEMPLATE_ENTRY> "library"              { return  StratosTypes.LIBRARY; }
<YYINITIAL, LONG_TEMPLATE_ENTRY> "mixin"                { return  StratosTypes.MIXIN; }
<YYINITIAL, LONG_TEMPLATE_ENTRY> "operator"             { return  StratosTypes.OPERATOR; }
<YYINITIAL, LONG_TEMPLATE_ENTRY> "part"                 { return  StratosTypes.PART; }
<YYINITIAL, LONG_TEMPLATE_ENTRY> "set"                  { return  StratosTypes.SET; }
<YYINITIAL, LONG_TEMPLATE_ENTRY> "static"               { return  StratosTypes.STATIC; }
<YYINITIAL, LONG_TEMPLATE_ENTRY> "typedef"              { return  StratosTypes.TYPEDEF; }
<YYINITIAL, LONG_TEMPLATE_ENTRY> "sync"                 { return  StratosTypes.SYNC; }
<YYINITIAL, LONG_TEMPLATE_ENTRY> "async"                { return  StratosTypes.ASYNC; }
<YYINITIAL, LONG_TEMPLATE_ENTRY> "await"                { return  StratosTypes.AWAIT; }
<YYINITIAL, LONG_TEMPLATE_ENTRY> "yield"                { return  StratosTypes.YIELD; }

// next are not listed in spec, but they seem to have the same sense as BUILT_IN_IDENTIFIER: somewhere treated as keywords, but can be used as normal identifiers
<YYINITIAL, LONG_TEMPLATE_ENTRY> "on"                   { return  StratosTypes.ON; }
<YYINITIAL, LONG_TEMPLATE_ENTRY> "of"                   { return  StratosTypes.OF; }
<YYINITIAL, LONG_TEMPLATE_ENTRY> "native"               { return  StratosTypes.NATIVE; }
<YYINITIAL, LONG_TEMPLATE_ENTRY> "show"                 { return  StratosTypes.SHOW; }
<YYINITIAL, LONG_TEMPLATE_ENTRY> "hide"                 { return  StratosTypes.HIDE; }
<YYINITIAL, LONG_TEMPLATE_ENTRY> "late"                 { return  StratosTypes.LATE; }
<YYINITIAL, LONG_TEMPLATE_ENTRY> "required"             { return  StratosTypes.REQUIRED; }

<YYINITIAL, LONG_TEMPLATE_ENTRY> {IDENTIFIER}           { return  StratosTypes.IDENTIFIER; }
<YYINITIAL, LONG_TEMPLATE_ENTRY> "["                { return  StratosTypes.LBRACKET; }
<YYINITIAL, LONG_TEMPLATE_ENTRY> "]"                { return  StratosTypes.RBRACKET; }
<YYINITIAL, LONG_TEMPLATE_ENTRY> "("                { return  StratosTypes.LPAREN; }
<YYINITIAL, LONG_TEMPLATE_ENTRY> ")"                { return  StratosTypes.RPAREN; }
<YYINITIAL, LONG_TEMPLATE_ENTRY> ";"                { return  StratosTypes.SEMICOLON; }
<YYINITIAL, LONG_TEMPLATE_ENTRY> "-"                { return  StratosTypes.MINUS; }
<YYINITIAL, LONG_TEMPLATE_ENTRY> "-="               { return  StratosTypes.MINUS_EQ; }
<YYINITIAL, LONG_TEMPLATE_ENTRY> "--"               { return  StratosTypes.MINUS_MINUS; }
<YYINITIAL, LONG_TEMPLATE_ENTRY> "+"                { return  StratosTypes.PLUS; }
<YYINITIAL, LONG_TEMPLATE_ENTRY> "++"               { return  StratosTypes.PLUS_PLUS; }
<YYINITIAL, LONG_TEMPLATE_ENTRY> "+="               { return  StratosTypes.PLUS_EQ; }
<YYINITIAL, LONG_TEMPLATE_ENTRY> "/"                { return  StratosTypes.DIV; }
<YYINITIAL, LONG_TEMPLATE_ENTRY> "/="               { return  StratosTypes.DIV_EQ; }
<YYINITIAL, LONG_TEMPLATE_ENTRY> "*"                { return  StratosTypes.MUL; }
<YYINITIAL, LONG_TEMPLATE_ENTRY> "*="               { return  StratosTypes.MUL_EQ; }
<YYINITIAL, LONG_TEMPLATE_ENTRY> "~/"               { return  StratosTypes.INT_DIV; }
<YYINITIAL, LONG_TEMPLATE_ENTRY> "~/="              { return  StratosTypes.INT_DIV_EQ; }
<YYINITIAL, LONG_TEMPLATE_ENTRY> "%="               { return  StratosTypes.REM_EQ; }
<YYINITIAL, LONG_TEMPLATE_ENTRY> "%"                { return  StratosTypes.REM; }
<YYINITIAL, LONG_TEMPLATE_ENTRY> "~"                { return  StratosTypes.BIN_NOT; }
<YYINITIAL, LONG_TEMPLATE_ENTRY> "!"                { return  StratosTypes.NOT; }

<YYINITIAL, LONG_TEMPLATE_ENTRY> "=>"               { return  StratosTypes.EXPRESSION_BODY_DEF; }
<YYINITIAL, LONG_TEMPLATE_ENTRY> "="                { return  StratosTypes.EQ; }
<YYINITIAL, LONG_TEMPLATE_ENTRY> "=="               { return  StratosTypes.EQ_EQ; }
<YYINITIAL, LONG_TEMPLATE_ENTRY> "!="               { return  StratosTypes.NEQ; }
<YYINITIAL, LONG_TEMPLATE_ENTRY> "."                { return  StratosTypes.DOT; }
<YYINITIAL, LONG_TEMPLATE_ENTRY> ".."               { return  StratosTypes.DOT_DOT; }
<YYINITIAL, LONG_TEMPLATE_ENTRY> "?.."              { return  StratosTypes.QUEST_DOT_DOT; }
<YYINITIAL, LONG_TEMPLATE_ENTRY> "..."              { return  StratosTypes.DOT_DOT_DOT; }
<YYINITIAL, LONG_TEMPLATE_ENTRY> "...?"             { return  StratosTypes.DOT_DOT_DOT_QUEST; }
<YYINITIAL, LONG_TEMPLATE_ENTRY> ","                { return  StratosTypes.COMMA; }
<YYINITIAL, LONG_TEMPLATE_ENTRY> ":"                { return  StratosTypes.COLON; }
<YYINITIAL, LONG_TEMPLATE_ENTRY> ">"                { return  StratosTypes.GT; }
//<YYINITIAL, LONG_TEMPLATE_ENTRY> ">="               { return  StratosTypes.GT_EQ;    } breaks mixin app parsing
//<YYINITIAL, LONG_TEMPLATE_ENTRY> ">>"               { return  StratosTypes.GT_GT;    } breaks generics parsing
//<YYINITIAL, LONG_TEMPLATE_ENTRY> ">>="              { return  StratosTypes.GT_GT_EQ; } breaks mixin app parsing
<YYINITIAL, LONG_TEMPLATE_ENTRY> "<"                { return  StratosTypes.LT; }
<YYINITIAL, LONG_TEMPLATE_ENTRY> "<="               { return  StratosTypes.LT_EQ; }
<YYINITIAL, LONG_TEMPLATE_ENTRY> "<<"               { return  StratosTypes.LT_LT; }
<YYINITIAL, LONG_TEMPLATE_ENTRY> "<<="              { return  StratosTypes.LT_LT_EQ; }
<YYINITIAL, LONG_TEMPLATE_ENTRY> "?"                { return  StratosTypes.QUEST; }
<YYINITIAL, LONG_TEMPLATE_ENTRY> "?."               { return  StratosTypes.QUEST_DOT; }
<YYINITIAL, LONG_TEMPLATE_ENTRY> "??"               { return  StratosTypes.QUEST_QUEST; }
<YYINITIAL, LONG_TEMPLATE_ENTRY> "??="              { return  StratosTypes.QUEST_QUEST_EQ; }
<YYINITIAL, LONG_TEMPLATE_ENTRY> "|"                { return  StratosTypes.OR; }
<YYINITIAL, LONG_TEMPLATE_ENTRY> "|="               { return  StratosTypes.OR_EQ; }
<YYINITIAL, LONG_TEMPLATE_ENTRY> "||"               { return  StratosTypes.OR_OR; }
<YYINITIAL, LONG_TEMPLATE_ENTRY> "||="              { return  StratosTypes.OR_OR_EQ; }
<YYINITIAL, LONG_TEMPLATE_ENTRY> "^"                { return  StratosTypes.XOR; }
<YYINITIAL, LONG_TEMPLATE_ENTRY> "^="               { return  StratosTypes.XOR_EQ; }
<YYINITIAL, LONG_TEMPLATE_ENTRY> "&"                { return  StratosTypes.AND; }
<YYINITIAL, LONG_TEMPLATE_ENTRY> "&="               { return  StratosTypes.AND_EQ; }
<YYINITIAL, LONG_TEMPLATE_ENTRY> "&&"               { return  StratosTypes.AND_AND; }
<YYINITIAL, LONG_TEMPLATE_ENTRY> "&&="              { return  StratosTypes.AND_AND_EQ; }
<YYINITIAL, LONG_TEMPLATE_ENTRY> "@"                { return  StratosTypes.AT; }
<YYINITIAL, LONG_TEMPLATE_ENTRY> "#"                { return  StratosTypes.HASH; }

<YYINITIAL, LONG_TEMPLATE_ENTRY> {NUMERIC_LITERAL} { return  StratosTypes.NUMBER; }

// raw strings
<YYINITIAL, LONG_TEMPLATE_ENTRY> {RAW_TRIPLE_QUOTED_STRING} { return   StratosTypes.RAW_TRIPLE_QUOTED_STRING; }
<YYINITIAL, LONG_TEMPLATE_ENTRY> {RAW_SINGLE_QUOTED_STRING} { return   StratosTypes.RAW_SINGLE_QUOTED_STRING; }

// string start
<YYINITIAL, LONG_TEMPLATE_ENTRY>      \"                  { pushState(QUO_STRING);        return   StratosTypes.OPEN_QUOTE;    }
<YYINITIAL, LONG_TEMPLATE_ENTRY>      \'                  { pushState(APOS_STRING);       return   StratosTypes.OPEN_QUOTE;    }
<YYINITIAL, LONG_TEMPLATE_ENTRY>      {THREE_QUO}         { pushState(THREE_QUO_STRING);  return   StratosTypes.OPEN_QUOTE;    }
<YYINITIAL, LONG_TEMPLATE_ENTRY>      {THREE_APOS}        { pushState(THREE_APOS_STRING); return   StratosTypes.OPEN_QUOTE;    }
// correct string end
<QUO_STRING>                          \"                  { popState();                   return   StratosTypes.CLOSING_QUOTE; }
<APOS_STRING>                         \'                  { popState();                   return   StratosTypes.CLOSING_QUOTE; }
<THREE_QUO_STRING>                    {THREE_QUO}         { popState();                   return   StratosTypes.CLOSING_QUOTE; }
<THREE_APOS_STRING>                   {THREE_APOS}        { popState();                   return   StratosTypes.CLOSING_QUOTE; }
<QUO_STRING, APOS_STRING>             \n                  { popState();                   return   StratosTypes.WHITE_SPACE;   } // not closed single-line string literal.Do not return   BAD_CHARACTER here because red highlighting of bad \n looks awful
// string content
<QUO_STRING>                          ([^\\\"\n\$] | (\\ [^\n]))*   {                return   StratosTypes.REGULAR_STRING_PART; }
<APOS_STRING>                         ([^\\\'\n\$] | (\\ [^\n]))*   {                return   StratosTypes.REGULAR_STRING_PART; }
<THREE_QUO_STRING>                    ([^\\\"\$])*                  {                return   StratosTypes.REGULAR_STRING_PART; }
<THREE_QUO_STRING>                    (\"[^\"]) | (\"\"[^\"])       { yypushback(1); return   StratosTypes.REGULAR_STRING_PART; } // pushback because we could capture '\' that escapes something
<THREE_APOS_STRING>                   ([^\\\'\$])*                  {                return   StratosTypes.REGULAR_STRING_PART; }
<THREE_APOS_STRING>                   (\'[^\']) | (\'\'[^\'])       { yypushback(1); return   StratosTypes.REGULAR_STRING_PART; } // pushback because we could capture '\' that escapes something
<THREE_QUO_STRING, THREE_APOS_STRING> (\\[^])                       {                return   StratosTypes.REGULAR_STRING_PART; } // escape sequence
// bad string interpolation (no identifier after '$')
<QUO_STRING, APOS_STRING, THREE_QUO_STRING, THREE_APOS_STRING> \$   { return   StratosTypes.SHORT_TEMPLATE_ENTRY_START; }
// short string interpolation
<QUO_STRING, APOS_STRING, THREE_QUO_STRING, THREE_APOS_STRING> {SHORT_TEMPLATE_ENTRY}      { pushState(SHORT_TEMPLATE_ENTRY);
                                                                                             yypushback(yylength() - 1);
                                                                                             return   StratosTypes.SHORT_TEMPLATE_ENTRY_START;}
// long string interpolation
<QUO_STRING, APOS_STRING, THREE_QUO_STRING, THREE_APOS_STRING> {LONG_TEMPLATE_ENTRY_START} { pushState(LONG_TEMPLATE_ENTRY);
                                                                                             return   StratosTypes.LONG_TEMPLATE_ENTRY_START; }
// Only *this* keyword is itself an expression valid in this position
// *null*, *true* and *false* are also keywords and expression, but it does not make sense to put them
// in a string template for it'd be easier to just type them in without a dollar
<SHORT_TEMPLATE_ENTRY> "this"          { popState(); return StratosTypes.THIS; }
<SHORT_TEMPLATE_ENTRY> {IDENTIFIER_NO_DOLLAR}    { popState(); return StratosTypes.IDENTIFIER; }

<YYINITIAL, MULTI_LINE_COMMENT_STATE, QUO_STRING, THREE_QUO_STRING, APOS_STRING, THREE_APOS_STRING, SHORT_TEMPLATE_ENTRY, LONG_TEMPLATE_ENTRY> [^] { return StratosTypes.BAD_CHARACTER; }





//// Copyright 2000-2020 JetBrains s.r.o. and other contributors. Use of this source code is governed by the Apache 2.0 license that can be found in the LICENSE file.
//package lang.stratos.grammer;
//
//import com.intellij.lexer.FlexLexer;
//import com.intellij.psi.tree.IElementType;
//import lang.stratos.grammer.types.StratosTypes; // org.intellij.sdk.language.psi.StratosTypes;
////import lang.stratos.grammer.types.StratosTokenType; //com.intellij.psi.TokenType;
//import com.intellij.psi.TokenType;
//
//%%
//
//%class StratosLexer
//%implements FlexLexer
//%unicode
//%function advance
//%type IElementType
//%eof{  return;
//%eof}
//
//CRLF=\R
//WHITE_SPACE=[\ \n\t\f]
//FIRST_VALUE_CHARACTER=[^ \n\f\\] | "\\"{CRLF} | "\\".
//VALUE_CHARACTER=[^\n\f\\] | "\\"{CRLF} | "\\".
//END_OF_LINE_COMMENT=("#"|"!")[^\r\n]*
//SEPARATOR=[:=]
//KEY_CHARACTER=[^:=\ \n\t\f\\] | "\\ "
//
//%state WAITING_VALUE
//
//%%
//
//<YYINITIAL> {END_OF_LINE_COMMENT}                           { yybegin(YYINITIAL); return StratosTypes.COMMENT; }
//
//<YYINITIAL> {KEY_CHARACTER}+                                { yybegin(YYINITIAL); return StratosTypes.KEY; }
//
//<YYINITIAL> {SEPARATOR}                                     { yybegin(WAITING_VALUE); return StratosTypes.SEPARATOR; }
//
//<WAITING_VALUE> {CRLF}({CRLF}|{WHITE_SPACE})+               { yybegin(YYINITIAL); return TokenType.WHITE_SPACE; }
//
//<WAITING_VALUE> {WHITE_SPACE}+                              { yybegin(WAITING_VALUE); return TokenType.WHITE_SPACE; }
//
//<WAITING_VALUE> {FIRST_VALUE_CHARACTER}{VALUE_CHARACTER}*   { yybegin(YYINITIAL); return StratosTypes.VALUE; }
//
//({CRLF}|{WHITE_SPACE})+                                     { yybegin(YYINITIAL); return TokenType.WHITE_SPACE; }
//
//[^]                                                         { return TokenType.BAD_CHARACTER; }
