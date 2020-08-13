package com.araizen.stratos.psi;

import com.intellij.psi.tree.IElementType;

@SuppressWarnings("SpellCheckingInspection")
public interface StratosTokenType extends TokenType {
    IElementType IDENTIFIER = new StratosElementType("IDENTIFIER");
    IElementType C_STYLE_COMMENT = new StratosElementType("C_STYLE_COMMENT");
    IElementType END_OF_LINE_COMMENT = new StratosElementType("END_OF_LINE_COMMENT");

    IElementType INTEGER_LITERAL = new StratosElementType("INTEGER_LITERAL");
    IElementType LONG_LITERAL = new StratosElementType("LONG_LITERAL");
    IElementType FLOAT_LITERAL = new StratosElementType("FLOAT_LITERAL");
    IElementType DOUBLE_LITERAL = new StratosElementType("DOUBLE_LITERAL");
    IElementType CHARACTER_LITERAL = new StratosElementType("CHARACTER_LITERAL");
    IElementType STRING_LITERAL = new StratosElementType("STRING_LITERAL");
    IElementType TEXT_BLOCK_LITERAL = new StratosElementType("TEXT_BLOCK_LITERAL");

    IElementType TRUE_KEYWORD = new KeywordElementType("TRUE_KEYWORD");
    IElementType FALSE_KEYWORD = new KeywordElementType("FALSE_KEYWORD");
    IElementType NULL_KEYWORD = new KeywordElementType("NULL_KEYWORD");

    IElementType ABSTRACT_KEYWORD = new KeywordElementType("ABSTRACT_KEYWORD");
    IElementType ASSERT_KEYWORD = new KeywordElementType("ASSERT_KEYWORD");
    IElementType BOOLEAN_KEYWORD = new KeywordElementType("BOOLEAN_KEYWORD");
    IElementType BREAK_KEYWORD = new KeywordElementType("BREAK_KEYWORD");
    IElementType BYTE_KEYWORD = new KeywordElementType("BYTE_KEYWORD");
    IElementType CASE_KEYWORD = new KeywordElementType("CASE_KEYWORD");
    IElementType CATCH_KEYWORD = new KeywordElementType("CATCH_KEYWORD");
    IElementType CHAR_KEYWORD = new KeywordElementType("CHAR_KEYWORD");
    IElementType CLASS_KEYWORD = new KeywordElementType("CLASS_KEYWORD");
    IElementType CONST_KEYWORD = new KeywordElementType("CONST_KEYWORD");
    IElementType CONTINUE_KEYWORD = new KeywordElementType("CONTINUE_KEYWORD");
    IElementType DEFAULT_KEYWORD = new KeywordElementType("DEFAULT_KEYWORD");
    IElementType DO_KEYWORD = new KeywordElementType("DO_KEYWORD");
    IElementType DOUBLE_KEYWORD = new KeywordElementType("DOUBLE_KEYWORD");
    IElementType ELSE_KEYWORD = new KeywordElementType("ELSE_KEYWORD");
    IElementType ENUM_KEYWORD = new KeywordElementType("ENUM_KEYWORD");
    IElementType EXTENDS_KEYWORD = new KeywordElementType("EXTENDS_KEYWORD");
    IElementType FINAL_KEYWORD = new KeywordElementType("FINAL_KEYWORD");
    IElementType FINALLY_KEYWORD = new KeywordElementType("FINALLY_KEYWORD");
    IElementType FLOAT_KEYWORD = new KeywordElementType("FLOAT_KEYWORD");
    IElementType FOR_KEYWORD = new KeywordElementType("FOR_KEYWORD");
    IElementType GOTO_KEYWORD = new KeywordElementType("GOTO_KEYWORD");
    IElementType IF_KEYWORD = new KeywordElementType("IF_KEYWORD");
    IElementType IMPLEMENTS_KEYWORD = new KeywordElementType("IMPLEMENTS_KEYWORD");
    IElementType IMPORT_KEYWORD = new KeywordElementType("IMPORT_KEYWORD");
    IElementType INSTANCEOF_KEYWORD = new KeywordElementType("INSTANCEOF_KEYWORD");
    IElementType INT_KEYWORD = new KeywordElementType("INT_KEYWORD");
    IElementType INTERFACE_KEYWORD = new KeywordElementType("INTERFACE_KEYWORD");
    IElementType LONG_KEYWORD = new KeywordElementType("LONG_KEYWORD");
    IElementType NATIVE_KEYWORD = new KeywordElementType("NATIVE_KEYWORD");
    IElementType NEW_KEYWORD = new KeywordElementType("NEW_KEYWORD");
    IElementType PACKAGE_KEYWORD = new KeywordElementType("PACKAGE_KEYWORD");
    IElementType PRIVATE_KEYWORD = new KeywordElementType("PRIVATE_KEYWORD");
    IElementType PUBLIC_KEYWORD = new KeywordElementType("PUBLIC_KEYWORD");
    IElementType SHORT_KEYWORD = new KeywordElementType("SHORT_KEYWORD");
    IElementType SUPER_KEYWORD = new KeywordElementType("SUPER_KEYWORD");
    IElementType SWITCH_KEYWORD = new KeywordElementType("SWITCH_KEYWORD");
    IElementType SYNCHRONIZED_KEYWORD = new KeywordElementType("SYNCHRONIZED_KEYWORD");
    IElementType THIS_KEYWORD = new KeywordElementType("THIS_KEYWORD");
    IElementType THROW_KEYWORD = new KeywordElementType("THROW_KEYWORD");
    IElementType PROTECTED_KEYWORD = new KeywordElementType("PROTECTED_KEYWORD");
    IElementType TRANSIENT_KEYWORD = new KeywordElementType("TRANSIENT_KEYWORD");
    IElementType RETURN_KEYWORD = new KeywordElementType("RETURN_KEYWORD");
    IElementType VOID_KEYWORD = new KeywordElementType("VOID_KEYWORD");
    IElementType STATIC_KEYWORD = new KeywordElementType("STATIC_KEYWORD");
    IElementType STRICTFP_KEYWORD = new KeywordElementType("STRICTFP_KEYWORD");
    IElementType WHILE_KEYWORD = new KeywordElementType("WHILE_KEYWORD");
    IElementType TRY_KEYWORD = new KeywordElementType("TRY_KEYWORD");
    IElementType VOLATILE_KEYWORD = new KeywordElementType("VOLATILE_KEYWORD");
    IElementType THROWS_KEYWORD = new KeywordElementType("THROWS_KEYWORD");

    IElementType LPARENTH = new StratosElementType("LPARENTH");
    IElementType RPARENTH = new StratosElementType("RPARENTH");
    IElementType LBRACE = new StratosElementType("LBRACE");
    IElementType RBRACE = new StratosElementType("RBRACE");
    IElementType LBRACKET = new StratosElementType("LBRACKET");
    IElementType RBRACKET = new StratosElementType("RBRACKET");
    IElementType SEMICOLON = new StratosElementType("SEMICOLON");
    IElementType COMMA = new StratosElementType("COMMA");
    IElementType DOT = new StratosElementType("DOT");
    IElementType ELLIPSIS = new StratosElementType("ELLIPSIS");
    IElementType AT = new StratosElementType("AT");

    IElementType EQ = new StratosElementType("EQ");
    IElementType GT = new StratosElementType("GT");
    IElementType LT = new StratosElementType("LT");
    IElementType EXCL = new StratosElementType("EXCL");
    IElementType TILDE = new StratosElementType("TILDE");
    IElementType QUEST = new StratosElementType("QUEST");
    IElementType COLON = new StratosElementType("COLON");
    IElementType PLUS = new StratosElementType("PLUS");
    IElementType MINUS = new StratosElementType("MINUS");
    IElementType ASTERISK = new StratosElementType("ASTERISK");
    IElementType DIV = new StratosElementType("DIV");
    IElementType AND = new StratosElementType("AND");
    IElementType OR = new StratosElementType("OR");
    IElementType XOR = new StratosElementType("XOR");
    IElementType PERC = new StratosElementType("PERC");

    IElementType EQEQ = new StratosElementType("EQEQ");
    IElementType LE = new StratosElementType("LE");
    IElementType GE = new StratosElementType("GE");
    IElementType NE = new StratosElementType("NE");
    IElementType ANDAND = new StratosElementType("ANDAND");
    IElementType OROR = new StratosElementType("OROR");
    IElementType PLUSPLUS = new StratosElementType("PLUSPLUS");
    IElementType MINUSMINUS = new StratosElementType("MINUSMINUS");
    IElementType LTLT = new StratosElementType("LTLT");
    IElementType GTGT = new StratosElementType("GTGT");
    IElementType GTGTGT = new StratosElementType("GTGTGT");
    IElementType PLUSEQ = new StratosElementType("PLUSEQ");
    IElementType MINUSEQ = new StratosElementType("MINUSEQ");
    IElementType ASTERISKEQ = new StratosElementType("ASTERISKEQ");
    IElementType DIVEQ = new StratosElementType("DIVEQ");
    IElementType ANDEQ = new StratosElementType("ANDEQ");
    IElementType OREQ = new StratosElementType("OREQ");
    IElementType XOREQ = new StratosElementType("XOREQ");
    IElementType PERCEQ = new StratosElementType("PERCEQ");
    IElementType LTLTEQ = new StratosElementType("LTLTEQ");
    IElementType GTGTEQ = new StratosElementType("GTGTEQ");
    IElementType GTGTGTEQ = new StratosElementType("GTGTGTEQ");

    IElementType DOUBLE_COLON = new StratosElementType("DOUBLE_COLON");
    IElementType ARROW = new StratosElementType("ARROW");

    IElementType OPEN_KEYWORD = new StratosElementType("OPEN");
    IElementType MODULE_KEYWORD = new StratosElementType("MODULE");
    IElementType REQUIRES_KEYWORD = new StratosElementType("REQUIRES");
    IElementType EXPORTS_KEYWORD = new StratosElementType("EXPORTS");
    IElementType OPENS_KEYWORD = new StratosElementType("OPENS");
    IElementType USES_KEYWORD = new StratosElementType("USES");
    IElementType PROVIDES_KEYWORD = new StratosElementType("PROVIDES");
    IElementType TRANSITIVE_KEYWORD = new StratosElementType("TRANSITIVE");
    IElementType TO_KEYWORD = new StratosElementType("TO");
    IElementType WITH_KEYWORD = new StratosElementType("WITH");

    IElementType VAR_KEYWORD = new StratosElementType("VAR");
    IElementType YIELD_KEYWORD = new StratosElementType("YIELD");
    IElementType RECORD_KEYWORD = new StratosElementType("RECORD");

    IElementType SEALED_KEYWORD = new StratosElementType("SEALED");
    IElementType NON_SEALED_KEYWORD = new KeywordElementType("NON_SEALED");
    IElementType PERMITS_KEYWORD = new StratosElementType("PERMITS");
}