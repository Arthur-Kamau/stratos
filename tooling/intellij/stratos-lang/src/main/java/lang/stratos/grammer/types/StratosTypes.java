// This is a generated file. Not intended for manual editing.
package lang.stratos.grammer.types;

import com.intellij.psi.TokenType;
import com.intellij.psi.tree.IElementType;
import com.intellij.psi.PsiElement;
import com.intellij.lang.ASTNode;
import lang.stratos.grammer.StratosElementType;
import lang.stratos.grammer.StratosTokenType;
import lang.stratos.grammer.psi.impl.*;

public interface StratosTypes {

  IElementType ASSIGNMENT_STATEMENT = new StratosElementType("ASSIGNMENT_STATEMENT");


  IElementType WHITE_SPACE = TokenType.WHITE_SPACE;
  IElementType BAD_CHARACTER = TokenType.BAD_CHARACTER;

  // DartLexer returns multiline comments as a single MULTI_LINE_COMMENT or MULTI_LINE_DOC_COMMENT
  // DartDocLexer splits MULTI_LINE_DOC_COMMENT in tokens

  // can't appear in PSI because merged into MULTI_LINE_COMMENT
  IElementType MULTI_LINE_COMMENT_START = new StratosElementType("MULTI_LINE_COMMENT_START");

  IElementType MULTI_LINE_DOC_COMMENT_START = new StratosElementType("MULTI_LINE_DOC_COMMENT_START");
  IElementType MULTI_LINE_COMMENT_BODY = new StratosElementType("MULTI_LINE_COMMENT_BODY");
  IElementType DOC_COMMENT_LEADING_ASTERISK = new StratosElementType("DOC_COMMENT_LEADING_ASTERISK");
  IElementType MULTI_LINE_COMMENT_END = new StratosElementType("MULTI_LINE_COMMENT_END");

  IElementType SINGLE_LINE_COMMENT = new StratosElementType("SINGLE_LINE_COMMENT");
  IElementType SINGLE_LINE_DOC_COMMENT = new StratosElementType("SINGLE_LINE_DOC_COMMENT");
  IElementType MULTI_LINE_COMMENT = new StratosElementType("MULTI_LINE_COMMENT");

    IElementType HEX_NUMBER = new StratosElementType("HEX_NUMBER");


    IElementType DEFINITION = new StratosElementType("DEFINITION");
  IElementType FUNCTION_DEFINITION = new StratosElementType("FUNCTION_DEFINITION");
  IElementType FUNCTION_INVOCATION = new StratosElementType("FUNCTION_INVOCATION");
  IElementType NAME_SPACE_DEFINITION = new StratosElementType("NAME_SPACE_DEFINITION");
  IElementType STATEMENT = new StratosElementType("STATEMENT");
  IElementType TYPE = new StratosElementType("TYPE");
  IElementType VARIABLE_DECLARATION = new StratosElementType("VARIABLE_DECLARATION");
  IElementType VARIABLE_DECLARATION_WITHOUT_TYPES = new StratosElementType("VARIABLE_DECLARATION_WITHOUT_TYPES");
  IElementType VARIABLE_DECLARATION_WITH_TYPES = new StratosElementType("VARIABLE_DECLARATION_WITH_TYPES");
  IElementType VARIABLE_DEFINITION = new StratosElementType("VARIABLE_DEFINITION");
  IElementType IF_NULL_EXPRESSION = new StratosElementType("IF_NULL_EXPRESSION");
  IElementType IF_STATEMENT = new StratosElementType("IF_STATEMENT");
  IElementType IF_ELEMENT = new StratosElementType("IF_ELEMENT");
  IElementType IMPORT_STATEMENT = new StratosElementType("IMPORT_STATEMENT");
  IElementType INCOMPLETE_DECLARATION = new StratosElementType("INCOMPLETE_DECLARATION");
  IElementType INITIALIZERS = new StratosElementType("INITIALIZERS");
  IElementType INTERFACES = new StratosElementType("INTERFACES");
  IElementType IS_EXPRESSION = new StratosElementType("IS_EXPRESSION");
  IElementType LABEL = new StratosElementType("LABEL");
  IElementType LIBRARY_COMPONENT_REFERENCE_EXPRESSION = new StratosElementType("LIBRARY_COMPONENT_REFERENCE_EXPRESSION");
  IElementType LIBRARY_ID = new StratosElementType("LIBRARY_ID");
  IElementType LIBRARY_NAME_ELEMENT = new StratosElementType("LIBRARY_NAME_ELEMENT");
  IElementType LIBRARY_REFERENCE_LIST = new StratosElementType("LIBRARY_REFERENCE_LIST");
  IElementType LIBRARY_STATEMENT = new StratosElementType("LIBRARY_STATEMENT");
  IElementType LIST_LITERAL_EXPRESSION = new StratosElementType("LIST_LITERAL_EXPRESSION");
  IElementType LITERAL_EXPRESSION = new StratosElementType("LITERAL_EXPRESSION");
  IElementType LOGIC_AND_EXPRESSION = new StratosElementType("LOGIC_AND_EXPRESSION");
  IElementType LOGIC_OR_EXPRESSION = new StratosElementType("LOGIC_OR_EXPRESSION");
  IElementType LONG_TEMPLATE_ENTRY = new StratosElementType("LONG_TEMPLATE_ENTRY");
  IElementType MAP_ENTRY = new StratosElementType("MAP_ENTRY");
  IElementType METADATA = new StratosElementType("METADATA");
  IElementType METHOD_DECLARATION = new StratosElementType("METHOD_DECLARATION");
  IElementType MIXINS = new StratosElementType("MIXINS");
  IElementType MIXIN_APPLICATION = new StratosElementType("MIXIN_APPLICATION");
  IElementType MIXIN_DECLARATION = new StratosElementType("MIXIN_DECLARATION");
  IElementType MULTIPLICATIVE_EXPRESSION = new StratosElementType("MULTIPLICATIVE_EXPRESSION");
  IElementType MULTIPLICATIVE_OPERATOR = new StratosElementType("MULTIPLICATIVE_OPERATOR");
  IElementType NAMED_ARGUMENT = new StratosElementType("NAMED_ARGUMENT");
  IElementType NAMED_CONSTRUCTOR_DECLARATION = new StratosElementType("NAMED_CONSTRUCTOR_DECLARATION");
  IElementType NEW_EXPRESSION = new StratosElementType("NEW_EXPRESSION");
  IElementType NORMAL_FORMAL_PARAMETER = new StratosElementType("NORMAL_FORMAL_PARAMETER");
  IElementType NORMAL_PARAMETER_TYPE = new StratosElementType("NORMAL_PARAMETER_TYPE");
  IElementType ON_MIXINS = new StratosElementType("ON_MIXINS");
  IElementType ON_PART = new StratosElementType("ON_PART");

  IElementType FUNCTION_BODY = new StratosElementType("FUNCTION_BODY");
  IElementType FUNCTION_DECLARATION_WITH_BODY = new StratosElementType("FUNCTION_DECLARATION_WITH_BODY");
  IElementType FUNCTION_DECLARATION_WITH_BODY_OR_NATIVE = new StratosElementType("FUNCTION_DECLARATION_WITH_BODY_OR_NATIVE");
  IElementType FUNCTION_EXPRESSION = new StratosElementType("FUNCTION_EXPRESSION");
  IElementType FUNCTION_EXPRESSION_BODY = new StratosElementType("FUNCTION_EXPRESSION_BODY");
  IElementType FUNCTION_FORMAL_PARAMETER = new StratosElementType("FUNCTION_FORMAL_PARAMETER");
  IElementType FUNCTION_TYPE_ALIAS = new StratosElementType("FUNCTION_TYPE_ALIAS");
  IElementType GETTER_DECLARATION = new StratosElementType("GETTER_DECLARATION");
 IElementType SETTER_DECLARATION = new StratosElementType("SETTER_DECLARATION");

  IElementType HIDE_COMBINATOR = new StratosElementType("HIDE_COMBINATOR");
  IElementType CLASS_BODY = new StratosElementType("CLASS_BODY");
  IElementType CLASS_DEFINITION = new StratosElementType("CLASS_DEFINITION");
  IElementType CLASS_MEMBERS = new StratosElementType("CLASS_MEMBERS");

  IElementType RAW_SINGLE_QUOTED_STRING = new StratosElementType("RAW_SINGLE_QUOTED_STRING");
  IElementType RAW_TRIPLE_QUOTED_STRING = new StratosElementType("RAW_TRIPLE_QUOTED_STRING");
  IElementType CLOSING_QUOTE = new StratosElementType("CLOSING_QUOTE");
  IElementType OPEN_QUOTE = new StratosElementType("OPEN_QUOTE");
  IElementType REGULAR_STRING_PART = new StratosElementType("REGULAR_STRING_PART");
    IElementType VAR_DECLARATION_LIST = new StratosElementType("VAR_DECLARATION_LIST");
    IElementType VAR_DECLARATION_LIST_PART = new StratosElementType("VAR_DECLARATION_LIST_PART");
    IElementType BLOCK = new StratosElementType("BLOCK");
    IElementType ARGUMENTS = new StratosElementType("ARGUMENTS");
  IElementType ARGUMENT_LIST = new StratosElementType("ARGUMENT_LIST");
  IElementType ARRAY_ACCESS_EXPRESSION = new StratosElementType("ARRAY_ACCESS_EXPRESSION");
  IElementType ASSERT_STATEMENT = new StratosElementType("ASSERT_STATEMENT");
  IElementType ASSIGNMENT_OPERATOR = new StratosElementType("ASSIGNMENT_OPERATOR");
  IElementType ASSIGN_EXPRESSION = new StratosElementType("ASSIGN_EXPRESSION");
  IElementType AS_EXPRESSION = new StratosElementType("AS_EXPRESSION");
  IElementType AWAIT_EXPRESSION = new StratosElementType("AWAIT_EXPRESSION");
  IElementType BITWISE_EXPRESSION = new StratosElementType("BITWISE_EXPRESSION");
  IElementType BITWISE_OPERATOR = new StratosElementType("BITWISE_OPERATOR");

  IElementType COMPARE_EXPRESSION = new StratosElementType("COMPARE_EXPRESSION");
  IElementType COMPONENT_NAME = new StratosElementType("COMPONENT_NAME");
  IElementType CONTINUE_STATEMENT = new StratosElementType("CONTINUE_STATEMENT");
  IElementType SHIFT_EXPRESSION = new StratosElementType("SHIFT_EXPRESSION");

    IElementType LONG_TEMPLATE_ENTRY_END = new StratosElementType("LONG_TEMPLATE_ENTRY_END");
    IElementType LONG_TEMPLATE_ENTRY_START = new StratosElementType("LONG_TEMPLATE_ENTRY_START");
    IElementType SHORT_TEMPLATE_ENTRY_START = new StratosElementType("SHORT_TEMPLATE_ENTRY_START");


    IElementType ADDITIVE_EXPRESSION = new StratosElementType("ADDITIVE_EXPRESSION");
  IElementType ADDITIVE_OPERATOR = new StratosElementType("ADDITIVE_OPERATOR");
  IElementType ABSTRACT = new StratosTokenType("abstract");
  IElementType AND = new StratosTokenType("&");
  IElementType AND_AND = new StratosTokenType("&&");
  IElementType AND_AND_EQ = new StratosTokenType("&&=");
  IElementType AND_EQ = new StratosTokenType("&=");
  IElementType ARRAYORSLICETYPE = new StratosTokenType("ArrayOrSliceType");
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
  IElementType LET = new StratosTokenType("let");
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
  IElementType NAMESPACE = new StratosTokenType("namespace");
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
  IElementType VAL = new StratosTokenType("val");
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
      else if (type == CLASS_DEFINITION) {
        return new StratosClassDefinitionImpl(node);
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
      else if (type == NAME_SPACE_DEFINITION) {
        return new StratosNameSpaceDefinitionImpl(node);
      }
      else if (type == STATEMENT) {
        return new StratosStatementImpl(node);
      }
      else if (type == TYPE) {
        return new StratosTypeImpl(node);
      }
      else if (type == VARIABLE_DECLARATION) {
        return new StratosVariableDeclarationImpl(node);
      }
      else if (type == VARIABLE_DECLARATION_WITHOUT_TYPES) {
        return new StratosVariableDeclarationWithoutTypesImpl(node);
      }
      else if (type == VARIABLE_DECLARATION_WITH_TYPES) {
        return new StratosVariableDeclarationWithTypesImpl(node);
      }
      else if (type == VARIABLE_DEFINITION) {
        return new StratosVariableDefinitionImpl(node);
      }
      throw new AssertionError("Unknown element type: " + type);
    }
  }
}
