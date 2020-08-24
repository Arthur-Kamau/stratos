// This is a generated file. Not intended for manual editing.
package lang.stratos.grammer;

import com.intellij.psi.tree.IElementType;
import com.intellij.psi.PsiElement;
import com.intellij.lang.ASTNode;
import lang.stratos.grammer.impl.*;

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
  IElementType AND = new StratosTokenType("&");
  IElementType AND_AND = new StratosTokenType("&&");
  IElementType AND_AND_EQ = new StratosTokenType("&&=");
  IElementType AND_EQ = new StratosTokenType("&=");
  IElementType AS = new StratosTokenType("as");
  IElementType ASSERT = new StratosTokenType("assert");
  IElementType ASYNC = new StratosTokenType("async");
  IElementType AT = new StratosTokenType("@");
  IElementType AWAIT = new StratosTokenType("await");
  IElementType BIN_NOT = new StratosTokenType("~");
  IElementType BREAK = new StratosTokenType("break");
  IElementType CASE = new StratosTokenType("case");
  IElementType CATCH = new StratosTokenType("catch");
  IElementType CLASS = new StratosTokenType("class");
  IElementType COLON = new StratosTokenType(":");
  IElementType COMMA = new StratosTokenType(",");
  IElementType COMMENT = new StratosTokenType("//");
  IElementType CONST = new StratosTokenType("const");
  IElementType CONTINUE = new StratosTokenType("continue");
  IElementType COVARIANT = new StratosTokenType("covariant");
  IElementType DEFAULT = new StratosTokenType("default");
  IElementType DEFERRED = new StratosTokenType("deferred");
  IElementType DIV = new StratosTokenType("/");
  IElementType DIV_EQ = new StratosTokenType("/=");
  IElementType DO = new StratosTokenType("do");
  IElementType DOT = new StratosTokenType(".");
  IElementType DOT_DOT = new StratosTokenType("..");
  IElementType DOT_DOT_DOT = new StratosTokenType("...");
  IElementType DOT_DOT_DOT_QUEST = new StratosTokenType("...?");
  IElementType DOUBLE = new StratosTokenType("double");
  IElementType ELSE = new StratosTokenType("else");
  IElementType ENUM = new StratosTokenType("enum");
  IElementType EQ = new StratosTokenType("=");
  IElementType EQ_EQ = new StratosTokenType("==");
  IElementType EXPORT = new StratosTokenType("export");
  IElementType EXPRESSION_BODY_DEF = new StratosTokenType("=>");
  IElementType EXTENDS = new StratosTokenType("extends");
  IElementType EXTENSION = new StratosTokenType("extension");
  IElementType EXTERNAL = new StratosTokenType("external");
  IElementType FACTORY = new StratosTokenType("factory");
  IElementType FALSE = new StratosTokenType("false");
  IElementType FINAL = new StratosTokenType("final");
  IElementType FINALLY = new StratosTokenType("finally");
  IElementType FOR = new StratosTokenType("for");
  IElementType FUNCTION = new StratosTokenType("function");
  IElementType GET = new StratosTokenType("get");
  IElementType GT = new StratosTokenType(">");
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
  IElementType LATE = new StratosTokenType("late");
  IElementType LBRACE = new StratosTokenType("{");
  IElementType LBRACKET = new StratosTokenType("[");
  IElementType LIBRARY = new StratosTokenType("library");
  IElementType LPAREN = new StratosTokenType("(");
  IElementType LT = new StratosTokenType("<");
  IElementType LT_EQ = new StratosTokenType("<=");
  IElementType LT_LT = new StratosTokenType("<<");
  IElementType LT_LT_EQ = new StratosTokenType("<<=");
  IElementType MINUS = new StratosTokenType("-");
  IElementType MINUS_EQ = new StratosTokenType("-=");
  IElementType MINUS_MINUS = new StratosTokenType("--");
  IElementType MIXIN = new StratosTokenType("mixin");
  IElementType MUL = new StratosTokenType("*");
  IElementType MUL_EQ = new StratosTokenType("*=");
  IElementType NATIVE = new StratosTokenType("native");
  IElementType NEQ = new StratosTokenType("!=");
  IElementType NEW = new StratosTokenType("new");
  IElementType NOT = new StratosTokenType("!");
  IElementType NULL = new StratosTokenType("null");
  IElementType NUMBER = new StratosTokenType("number");
  IElementType OF = new StratosTokenType("of");
  IElementType ON = new StratosTokenType("on");
  IElementType OPERATOR = new StratosTokenType("operator");
  IElementType OR = new StratosTokenType("|");
  IElementType OR_EQ = new StratosTokenType("|=");
  IElementType OR_OR = new StratosTokenType("||");
  IElementType OR_OR_EQ = new StratosTokenType("||=");
  IElementType PART = new StratosTokenType("part");
  IElementType PLUS = new StratosTokenType("+");
  IElementType PLUS_EQ = new StratosTokenType("+=");
  IElementType PLUS_PLUS = new StratosTokenType("++");
  IElementType QUEST = new StratosTokenType("?");
  IElementType QUEST_DOT = new StratosTokenType("?.");
  IElementType QUEST_DOT_DOT = new StratosTokenType("?..");
  IElementType QUEST_QUEST = new StratosTokenType("??");
  IElementType QUEST_QUEST_EQ = new StratosTokenType("??=");
  IElementType RBRACE = new StratosTokenType("}");
  IElementType RBRACKET = new StratosTokenType("]");
  IElementType REM = new StratosTokenType("%");
  IElementType REM_EQ = new StratosTokenType("%=");
  IElementType REQUIRED = new StratosTokenType("required");
  IElementType RETHROW = new StratosTokenType("rethrow");
  IElementType RETURN = new StratosTokenType("return");
  IElementType RETURNS = new StratosTokenType("returns");
  IElementType RPAREN = new StratosTokenType(")");
  IElementType SEMICOLON = new StratosTokenType(";");
  IElementType SET = new StratosTokenType("set");
  IElementType SHOW = new StratosTokenType("show");
  IElementType STATIC = new StratosTokenType("static");
  IElementType STRING = new StratosTokenType("string");
  IElementType SUPER = new StratosTokenType("super");
  IElementType SWITCH = new StratosTokenType("switch");
  IElementType SYNC = new StratosTokenType("sync");
  IElementType THIS = new StratosTokenType("this");
  IElementType THROW = new StratosTokenType("throw");
  IElementType TRUE = new StratosTokenType("true");
  IElementType TRY = new StratosTokenType("try");
  IElementType TYPEDEF = new StratosTokenType("typedef");
  IElementType VAR = new StratosTokenType("var");
  IElementType VOID = new StratosTokenType("void");
  IElementType WHILE = new StratosTokenType("while");
  IElementType WITH = new StratosTokenType("with");
  IElementType XOR = new StratosTokenType("^");
  IElementType XOR_EQ = new StratosTokenType("^=");
  IElementType YIELD = new StratosTokenType("yield");

  class Factory {
    public static PsiElement createElement(ASTNode node) {
      IElementType type = node.getElementType();
      if (type == ASSIGNMENT_STATEMENT) {
        return new StratosAssignmentStatementImpl(node);
      }
      else if (type == DEFINITION) {
        return new StratosDefinitionImpl(node);
      }
      else if (type == FUNCTION_BODY) {
        return new StratosFunctionBodyImpl(node);
      }
      else if (type == FUNCTION_DEFINITION) {
        return new StratosFunctionDefinitionImpl(node);
      }
      else if (type == FUNCTION_INVOCATION) {
        return new StratosFunctionInvocationImpl(node);
      }
      else if (type == STATEMENT) {
        return new StratosStatementImpl(node);
      }
      else if (type == TYPE) {
        return new StratosTypeImpl(node);
      }
      else if (type == VARIABLE_DEFINITION) {
        return new StratosVariableDefinitionImpl(node);
      }
      throw new AssertionError("Unknown element type: " + type);
    }
  }
}
