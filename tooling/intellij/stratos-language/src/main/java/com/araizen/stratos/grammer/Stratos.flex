// Copyright 2000-2019 JetBrains s.r.o. Use of this source code is governed by the Apache 2.0 license that can be found in the LICENSE file.
package com.araizen.stratos.lexer;

import com.intellij.pom.java.LanguageLevel;
import com.araizen.stratos.psi.StratosTokenType;
import com.araizen.stratos.documentation.StratosDocElementType;
import com.araizen.stratos.documentation.StratosDocTokenType;
import com.intellij.psi.tree.IElementType;
import com.intellij.lexer.FlexLexer;

@SuppressWarnings("ALL")
%%

%{
  private boolean myAssertKeyword;
  private boolean myEnumKeyword;

  public _JavaLexer(LanguageLevel level) {
    this((java.io.Reader)null);
    myAssertKeyword = level.isAtLeast(LanguageLevel.JDK_1_4);
    myEnumKeyword = level.isAtLeast(LanguageLevel.JDK_1_5);
  }

  public void goTo(int offset) {
    zzCurrentPos = zzMarkedPos = zzStartRead = offset;
    zzAtEOF = false;
  }
%}

%unicode
%class _JavaLexer
%implements FlexLexer
%function advance
%type IElementType

IDENTIFIER = [:jletter:] [:jletterdigit:]*

DIGIT = [0-9]
DIGIT_OR_UNDERSCORE = [_0-9]
DIGITS = {DIGIT} | {DIGIT} {DIGIT_OR_UNDERSCORE}*
HEX_DIGIT_OR_UNDERSCORE = [_0-9A-Fa-f]

INTEGER_LITERAL = {DIGITS} | {HEX_INTEGER_LITERAL} | {BIN_INTEGER_LITERAL}
LONG_LITERAL = {INTEGER_LITERAL} [Ll]
HEX_INTEGER_LITERAL = 0 [Xx] {HEX_DIGIT_OR_UNDERSCORE}*
BIN_INTEGER_LITERAL = 0 [Bb] {DIGIT_OR_UNDERSCORE}*

FLOAT_LITERAL = ({DEC_FP_LITERAL} | {HEX_FP_LITERAL}) [Ff] | {DIGITS} [Ff]
DOUBLE_LITERAL = ({DEC_FP_LITERAL} | {HEX_FP_LITERAL}) [Dd]? | {DIGITS} [Dd]
DEC_FP_LITERAL = {DIGITS} {DEC_EXPONENT} | {DEC_SIGNIFICAND} {DEC_EXPONENT}?
DEC_SIGNIFICAND = "." {DIGITS} | {DIGITS} "." {DIGIT_OR_UNDERSCORE}*
DEC_EXPONENT = [Ee] [+-]? {DIGIT_OR_UNDERSCORE}*
HEX_FP_LITERAL = {HEX_SIGNIFICAND} {HEX_EXPONENT}
HEX_SIGNIFICAND = 0 [Xx] ({HEX_DIGIT_OR_UNDERSCORE}+ "."? | {HEX_DIGIT_OR_UNDERSCORE}* "." {HEX_DIGIT_OR_UNDERSCORE}+)
HEX_EXPONENT = [Pp] [+-]? {DIGIT_OR_UNDERSCORE}*

%%

/*
 * NOTE: the rule set does not include rules for whitespaces, comments, and text literals -
 * they are implemented in com.intellij.lang.java.lexer.JavaLexer class.
 */

<YYINITIAL> {
  {LONG_LITERAL} { return StratosTokenType.LONG_LITERAL; }
  {INTEGER_LITERAL} { return StratosTokenType.INTEGER_LITERAL; }
  {FLOAT_LITERAL} { return StratosTokenType.FLOAT_LITERAL; }
  {DOUBLE_LITERAL} { return StratosTokenType.DOUBLE_LITERAL; }

  "true" { return StratosTokenType.TRUE_KEYWORD; }
  "false" { return StratosTokenType.FALSE_KEYWORD; }
  "null" { return StratosTokenType.NULL_KEYWORD; }

  "abstract" { return StratosTokenType.ABSTRACT_KEYWORD; }
  "assert" { return myAssertKeyword ? StratosTokenType.ASSERT_KEYWORD : StratosTokenType.IDENTIFIER; }
  "boolean" { return StratosTokenType.BOOLEAN_KEYWORD; }
  "break" { return StratosTokenType.BREAK_KEYWORD; }
  "byte" { return StratosTokenType.BYTE_KEYWORD; }
  "case" { return StratosTokenType.CASE_KEYWORD; }
  "catch" { return StratosTokenType.CATCH_KEYWORD; }
  "char" { return StratosTokenType.CHAR_KEYWORD; }
  "class" { return StratosTokenType.CLASS_KEYWORD; }
  "const" { return StratosTokenType.CONST_KEYWORD; }
  "continue" { return StratosTokenType.CONTINUE_KEYWORD; }
  "default" { return StratosTokenType.DEFAULT_KEYWORD; }
  "do" { return StratosTokenType.DO_KEYWORD; }
  "double" { return StratosTokenType.DOUBLE_KEYWORD; }
  "else" { return StratosTokenType.ELSE_KEYWORD; }
  "enum" { return myEnumKeyword ? StratosTokenType.ENUM_KEYWORD : StratosTokenType.IDENTIFIER; }
  "extends" { return StratosTokenType.EXTENDS_KEYWORD; }
  "final" { return StratosTokenType.FINAL_KEYWORD; }
  "finally" { return StratosTokenType.FINALLY_KEYWORD; }
  "float" { return StratosTokenType.FLOAT_KEYWORD; }
  "for" { return StratosTokenType.FOR_KEYWORD; }
  "goto" { return StratosTokenType.GOTO_KEYWORD; }
  "if" { return StratosTokenType.IF_KEYWORD; }
  "implements" { return StratosTokenType.IMPLEMENTS_KEYWORD; }
  "import" { return StratosTokenType.IMPORT_KEYWORD; }
  "instanceof" { return StratosTokenType.INSTANCEOF_KEYWORD; }
  "int" { return StratosTokenType.INT_KEYWORD; }
  "interface" { return StratosTokenType.INTERFACE_KEYWORD; }
  "long" { return StratosTokenType.LONG_KEYWORD; }
  "native" { return StratosTokenType.NATIVE_KEYWORD; }
  "new" { return StratosTokenType.NEW_KEYWORD; }
  "package" { return StratosTokenType.PACKAGE_KEYWORD; }
  "private" { return StratosTokenType.PRIVATE_KEYWORD; }
  "public" { return StratosTokenType.PUBLIC_KEYWORD; }
  "short" { return StratosTokenType.SHORT_KEYWORD; }
  "super" { return StratosTokenType.SUPER_KEYWORD; }
  "switch" { return StratosTokenType.SWITCH_KEYWORD; }
  "synchronized" { return StratosTokenType.SYNCHRONIZED_KEYWORD; }
  "this" { return StratosTokenType.THIS_KEYWORD; }
  "throw" { return StratosTokenType.THROW_KEYWORD; }
  "protected" { return StratosTokenType.PROTECTED_KEYWORD; }
  "transient" { return StratosTokenType.TRANSIENT_KEYWORD; }
  "return" { return StratosTokenType.RETURN_KEYWORD; }
  "void" { return StratosTokenType.VOID_KEYWORD; }
  "static" { return StratosTokenType.STATIC_KEYWORD; }
  "strictfp" { return StratosTokenType.STRICTFP_KEYWORD; }
  "while" { return StratosTokenType.WHILE_KEYWORD; }
  "try" { return StratosTokenType.TRY_KEYWORD; }
  "volatile" { return StratosTokenType.VOLATILE_KEYWORD; }
  "throws" { return StratosTokenType.THROWS_KEYWORD; }

  {IDENTIFIER} { return StratosTokenType.IDENTIFIER; }

  "==" { return StratosTokenType.EQEQ; }
  "!=" { return StratosTokenType.NE; }
  "||" { return StratosTokenType.OROR; }
  "++" { return StratosTokenType.PLUSPLUS; }
  "--" { return StratosTokenType.MINUSMINUS; }

  "<" { return StratosTokenType.LT; }
  "<=" { return StratosTokenType.LE; }
  "<<=" { return StratosTokenType.LTLTEQ; }
  "<<" { return StratosTokenType.LTLT; }
  ">" { return StratosTokenType.GT; }
  "&" { return StratosTokenType.AND; }
  "&&" { return StratosTokenType.ANDAND; }

  "+=" { return StratosTokenType.PLUSEQ; }
  "-=" { return StratosTokenType.MINUSEQ; }
  "*=" { return StratosTokenType.ASTERISKEQ; }
  "/=" { return StratosTokenType.DIVEQ; }
  "&=" { return StratosTokenType.ANDEQ; }
  "|=" { return StratosTokenType.OREQ; }
  "^=" { return StratosTokenType.XOREQ; }
  "%=" { return StratosTokenType.PERCEQ; }

  "("   { return StratosTokenType.LPARENTH; }
  ")"   { return StratosTokenType.RPARENTH; }
  "{"   { return StratosTokenType.LBRACE; }
  "}"   { return StratosTokenType.RBRACE; }
  "["   { return StratosTokenType.LBRACKET; }
  "]"   { return StratosTokenType.RBRACKET; }
  ";"   { return StratosTokenType.SEMICOLON; }
  ","   { return StratosTokenType.COMMA; }
  "..." { return StratosTokenType.ELLIPSIS; }
  "."   { return StratosTokenType.DOT; }

  "=" { return StratosTokenType.EQ; }
  "!" { return StratosTokenType.EXCL; }
  "~" { return StratosTokenType.TILDE; }
  "?" { return StratosTokenType.QUEST; }
  ":" { return StratosTokenType.COLON; }
  "+" { return StratosTokenType.PLUS; }
  "-" { return StratosTokenType.MINUS; }
  "*" { return StratosTokenType.ASTERISK; }
  "/" { return StratosTokenType.DIV; }
  "|" { return StratosTokenType.OR; }
  "^" { return StratosTokenType.XOR; }
  "%" { return StratosTokenType.PERC; }
  "@" { return StratosTokenType.AT; }

  "::" { return StratosTokenType.DOUBLE_COLON; }
  "->" { return StratosTokenType.ARROW; }
}

[^]  { return StratosTokenType.BAD_CHARACTER; }
