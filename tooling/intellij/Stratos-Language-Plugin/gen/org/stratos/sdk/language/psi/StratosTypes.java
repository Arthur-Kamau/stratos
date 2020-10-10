// This is a generated file. Not intended for manual editing.
package org.stratos.sdk.language.psi;

import com.intellij.psi.tree.IElementType;
import com.intellij.psi.PsiElement;
import com.intellij.lang.ASTNode;
import org.stratos.sdk.language.psi.impl.*;

public interface StratosTypes {

  IElementType ASSIGNMENT_STATEMENT = new StratosElementType("ASSIGNMENT_STATEMENT");
  IElementType DEFINITION = new StratosElementType("DEFINITION");
  IElementType FUNCTION_BODY = new StratosElementType("FUNCTION_BODY");
  IElementType FUNCTION_DEFINITION = new StratosElementType("FUNCTION_DEFINITION");
  IElementType FUNCTION_INVOCATION = new StratosElementType("FUNCTION_INVOCATION");
  IElementType STATEMENT = new StratosElementType("STATEMENT");
  IElementType TYPE = new StratosElementType("TYPE");
  IElementType VARIABLE_DEFINITION = new StratosElementType("VARIABLE_DEFINITION");

  IElementType ABSTRACT = new StratosTokenType("abstract");
  IElementType ADD = new StratosTokenType("+");
  IElementType ALIAS = new StratosTokenType("alias");
  IElementType AND = new StratosTokenType("&");
  IElementType AND_AND = new StratosTokenType("&&");
  IElementType AND_AND_EQ = new StratosTokenType("&&=");
  IElementType AND_EQ = new StratosTokenType("&=");
  IElementType ARROW = new StratosTokenType("->");
  IElementType AS = new StratosTokenType("as");
  IElementType ASSERT = new StratosTokenType("assert");
  IElementType ASSIGN = new StratosTokenType("=");
  IElementType ASYNC = new StratosTokenType("async");
  IElementType AT = new StratosTokenType("@");
  IElementType AWAIT = new StratosTokenType("await");
  IElementType BIN_NOT = new StratosTokenType("~");
  IElementType BOOL = new StratosTokenType("bool");
  IElementType BREAK = new StratosTokenType("break");
  IElementType CASE = new StratosTokenType("case");
  IElementType CATCH = new StratosTokenType("catch");
  IElementType CHAR = new StratosTokenType("char");
  IElementType CLASS = new StratosTokenType("class");
  IElementType CLOSE_BRACE = new StratosTokenType(")");
  IElementType CLOSE_BRACKETS = new StratosTokenType("}");
  IElementType CLOSE_QUOTE = new StratosTokenType("\"");
  IElementType COLONCOLON = new StratosTokenType("::");
  IElementType COMMA = new StratosTokenType(",");
  IElementType COMMENT = new StratosTokenType("comment");
  IElementType CONST = new StratosTokenType("const");
  IElementType CONTINUE = new StratosTokenType("continue");
  IElementType COVARIANT = new StratosTokenType("covariant");
  IElementType DEFAULT = new StratosTokenType("default");
  IElementType DEFERRED = new StratosTokenType("deferred");
  IElementType DIVIDE = new StratosTokenType("/");
  IElementType DIV_EQ = new StratosTokenType("/=");
  IElementType DO = new StratosTokenType("do");
  IElementType DOLLAR = new StratosTokenType("$");
  IElementType DOT = new StratosTokenType(".");
  IElementType DOT_DOT = new StratosTokenType("..");
  IElementType DOT_DOT_DOT = new StratosTokenType("...");
  IElementType DOT_DOT_DOT_QUEST = new StratosTokenType("...?");
  IElementType DOUBLE = new StratosTokenType("double");
  IElementType ELSE = new StratosTokenType("else");
  IElementType ENUM = new StratosTokenType("enum");
  IElementType EQUAL = new StratosTokenType("==");
  IElementType EXPORT = new StratosTokenType("export");
  IElementType EXTENDS = new StratosTokenType("extends");
  IElementType EXTENSION = new StratosTokenType("extension");
  IElementType EXTERNAL = new StratosTokenType("external");
  IElementType FACTORY = new StratosTokenType("factory");
  IElementType FALSE = new StratosTokenType("false");
  IElementType FAT_ARROW = new StratosTokenType("=>");
  IElementType FINAL = new StratosTokenType("final");
  IElementType FINALLY = new StratosTokenType("finally");
  IElementType FOR = new StratosTokenType("for");
  IElementType FUNCTION = new StratosTokenType("function");
  IElementType GET = new StratosTokenType("get");
  IElementType GREATER_THAN = new StratosTokenType(">");
  IElementType GT_EQ = new StratosTokenType(">=");
  IElementType GT_GT = new StratosTokenType(">>");
  IElementType GT_GT_EQ = new StratosTokenType(">>=");
  IElementType HASH = new StratosTokenType("#");
  IElementType HIDE = new StratosTokenType("hide");
  IElementType IDENTIFIER = new StratosTokenType("IDENTIFIER");
  IElementType IF = new StratosTokenType("if");
  IElementType IMPLEMENTS = new StratosTokenType("implements");
  IElementType IMPORT = new StratosTokenType("import");
  IElementType IN = new StratosTokenType("in");
  IElementType INT = new StratosTokenType("int");
  IElementType INT_DIV = new StratosTokenType("~/");
  IElementType INT_DIV_EQ = new StratosTokenType("~/=");
  IElementType IS = new StratosTokenType("is");
  IElementType KEY = new StratosTokenType("key");
  IElementType LATE = new StratosTokenType("late");
  IElementType LESS_THAN = new StratosTokenType("<");
  IElementType LET = new StratosTokenType("let");
  IElementType LIBRARY = new StratosTokenType("library");
  IElementType LOOP = new StratosTokenType("loop");
  IElementType LSQUAREBRACKET = new StratosTokenType("[");
  IElementType LT_EQ = new StratosTokenType("<=");
  IElementType LT_LT = new StratosTokenType("<<");
  IElementType LT_LT_EQ = new StratosTokenType("<<=");
  IElementType MACRO_KW = new StratosTokenType("macro");
  IElementType MATCH = new StratosTokenType("match");
  IElementType MINUS = new StratosTokenType("-");
  IElementType MINUS_EQ = new StratosTokenType("-=");
  IElementType MINUS_MINUS = new StratosTokenType("--");
  IElementType MIXIN = new StratosTokenType("mixin");
  IElementType MULTIPLY = new StratosTokenType("*");
  IElementType MUL_EQ = new StratosTokenType("*=");
  IElementType MUTABLE = new StratosTokenType("mut");
  IElementType NATIVE = new StratosTokenType("native");
  IElementType NEQ = new StratosTokenType("!=");
  IElementType NEW = new StratosTokenType("new");
  IElementType NOT = new StratosTokenType("!");
  IElementType NULL = new StratosTokenType("null");
  IElementType OF = new StratosTokenType("of");
  IElementType ON = new StratosTokenType("on");
  IElementType OPEN_BRACE = new StratosTokenType("(");
  IElementType OPEN_BRACKETS = new StratosTokenType("{");
  IElementType OPERATOR = new StratosTokenType("operator");
  IElementType OR = new StratosTokenType("|");
  IElementType OR_EQ = new StratosTokenType("|=");
  IElementType OR_OR = new StratosTokenType("||");
  IElementType OR_OR_EQ = new StratosTokenType("||=");
  IElementType OVERRIDE = new StratosTokenType("overide");
  IElementType PACKAGE = new StratosTokenType("package");
  IElementType PART = new StratosTokenType("part");
  IElementType PLUS_EQ = new StratosTokenType("+=");
  IElementType PLUS_PLUS = new StratosTokenType("++");
  IElementType QUEST = new StratosTokenType("?");
  IElementType QUEST_DOT = new StratosTokenType("?.");
  IElementType QUEST_DOT_DOT = new StratosTokenType("?..");
  IElementType QUEST_QUEST = new StratosTokenType("??");
  IElementType QUEST_QUEST_EQ = new StratosTokenType("??=");
  IElementType REM = new StratosTokenType("%");
  IElementType REM_EQ = new StratosTokenType("%=");
  IElementType REQUIRED = new StratosTokenType("required");
  IElementType RESERVED_KEYWORD = new StratosTokenType("reserved_keyword");
  IElementType RETHROW = new StratosTokenType("rethrow");
  IElementType RETURN = new StratosTokenType("return");
  IElementType RETURNS = new StratosTokenType("returns");
  IElementType RSQUAREBRACKET = new StratosTokenType("]");
  IElementType SEMICOLON = new StratosTokenType(":");
  IElementType SEMI_COLON = new StratosTokenType(";");
  IElementType SET = new StratosTokenType("set");
  IElementType SHOW = new StratosTokenType("show");
  IElementType STATIC = new StratosTokenType("static");
  IElementType STRING = new StratosTokenType("string");
  IElementType STRUCT = new StratosTokenType("struct");
  IElementType SUPER = new StratosTokenType("super");
  IElementType SWITCH = new StratosTokenType("switch");
  IElementType SYNC = new StratosTokenType("sync");
  IElementType THIS = new StratosTokenType("this");
  IElementType THROW = new StratosTokenType("throw");
  IElementType TRUE = new StratosTokenType("true");
  IElementType TRY = new StratosTokenType("try");
  IElementType TYPEDEF = new StratosTokenType("typedef");
  IElementType TYPE_KW = new StratosTokenType("type");
  IElementType UNDERSCORE = new StratosTokenType("_");
  IElementType UNIT = new StratosTokenType("unit");
  IElementType UNSAFE = new StratosTokenType("unsafe");
  IElementType USE = new StratosTokenType("use");
  IElementType VALUE = new StratosTokenType("value");
  IElementType VARIABLE = new StratosTokenType("var");
  IElementType VOID = new StratosTokenType("void");
  IElementType WHEN = new StratosTokenType("when");
  IElementType WHERE = new StratosTokenType("where");
  IElementType WHILE = new StratosTokenType("while");
  IElementType WITH = new StratosTokenType("with");
  IElementType XOR = new StratosTokenType("^");
  IElementType XOR_EQ = new StratosTokenType("^=");
  IElementType YIELD = new StratosTokenType("yield");

  class Factory {
    public static PsiElement createElement(ASTNode node) {
      IElementType type = node.getElementType();
      if (type == ASSIGNMENT_STATEMENT) {
        return new stratosAssignmentStatementImpl(node);
      }
      else if (type == DEFINITION) {
        return new stratosDefinitionImpl(node);
      }
      else if (type == FUNCTION_BODY) {
        return new stratosFunctionBodyImpl(node);
      }
      else if (type == FUNCTION_DEFINITION) {
        return new stratosFunctionDefinitionImpl(node);
      }
      else if (type == FUNCTION_INVOCATION) {
        return new stratosFunctionInvocationImpl(node);
      }
      else if (type == STATEMENT) {
        return new stratosStatementImpl(node);
      }
      else if (type == TYPE) {
        return new stratosTypeImpl(node);
      }
      else if (type == VARIABLE_DEFINITION) {
        return new stratosVariableDefinitionImpl(node);
      }
      throw new AssertionError("Unknown element type: " + type);
    }
  }
}
