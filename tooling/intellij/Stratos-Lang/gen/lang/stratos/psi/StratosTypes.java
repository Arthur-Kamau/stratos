// This is a generated file. Not intended for manual editing.
package lang.stratos.psi;

import com.intellij.psi.TokenType;
import com.intellij.psi.tree.IElementType;
import com.intellij.psi.PsiElement;
import com.intellij.lang.ASTNode;
import com.intellij.psi.tree.IFileElementType;
import com.intellij.psi.tree.TokenSet;
import lang.stratos.language.StratosElementType;
import lang.stratos.language.StratosLanguage;
import lang.stratos.language.StratosTokenType;
import lang.stratos.psi.impl.*;

public interface StratosTypes {

  IFileElementType STRATOS_FILE = new IFileElementType("STRATOSFILE", StratosLanguage.INSTANCE);


  IElementType WHITE_SPACE = TokenType.WHITE_SPACE;
  IElementType BAD_CHARACTER = TokenType.BAD_CHARACTER;

  IElementType ASSIGNMENT_STATEMENT = new StratosElementType("ASSIGNMENT_STATEMENT");
  IElementType BASIC_VARIABLE_DEFINITION = new StratosElementType("BASIC_VARIABLE_DEFINITION");
  IElementType DEFINITION = new StratosElementType("DEFINITION");
  IElementType ENUM_TYPE = new StratosElementType("ENUM_TYPE");
  IElementType FUNCTION_BODY = new StratosElementType("FUNCTION_BODY");
  IElementType FUNCTION_DEFINITION = new StratosElementType("FUNCTION_DEFINITION");
  IElementType FUNCTION_INVOCATION = new StratosElementType("FUNCTION_INVOCATION");
  IElementType PACKAGE_DEFINITION = new StratosElementType("PACKAGE_DEFINITION");
  IElementType STATEMENT = new StratosElementType("STATEMENT");
  IElementType STRUCT_TYPE = new StratosElementType("STRUCT_TYPE");
  IElementType TYPE = new StratosElementType("TYPE");
  IElementType TYPE_PACKAGE = new StratosElementType("TYPE_PACKAGE");
  IElementType TYPE_WRAPPER = new StratosElementType("TYPE_WRAPPER");
  IElementType VARIABLE_DEFINITION = new StratosElementType("VARIABLE_DEFINITION");

  IElementType AS = new StratosTokenType("AS");
  IElementType IS = new StratosTokenType("IS");

  IElementType DIVIDE = new StratosTokenType("DIVIDE");
  IElementType MULTIPLY = new StratosTokenType("MULTIPLY");
  IElementType ADD = new StratosTokenType("ADD");
  IElementType MINUS = new StratosTokenType("MINUS");
  IElementType BREAK = new StratosTokenType("BREAK");
  IElementType SEMICOLON = new StratosTokenType("SEMICOLON");
  IElementType GREATER_THAN = new StratosTokenType("GREATER_THAN");
  IElementType LESS_THAN = new StratosTokenType("LESS_THAN");
  IElementType ASSIGN = new StratosTokenType("ASSIGN");
  IElementType COLON = new StratosTokenType("COLON");
  IElementType NOT = new StratosTokenType("NOT");
  IElementType SEMI_COLON = new StratosTokenType("SEMI_COLON");
  IElementType OPEN_BRACKETS = new StratosTokenType("OPEN_BRACKETS");
  IElementType CLOSE_BRACKETS = new StratosTokenType("CLOSE_BRACKETS");
  IElementType OPEN_BRACE = new StratosTokenType("OPEN_BRACE");
  IElementType CLOSE_BRACE = new StratosTokenType("CLOSE_BRACE");
  IElementType ALIAS = new StratosTokenType("ALIAS");
  IElementType WHEN = new StratosTokenType("WHEN");
  IElementType IF = new StratosTokenType("IF");
  IElementType ELSE = new StratosTokenType("ELSE");
  IElementType EQUAL = new StratosTokenType("EQUAL");
  IElementType YIELD = new StratosTokenType("yield");
  IElementType ASSERT = new StratosTokenType("ASSERT");

  IElementType RAW_SINGLE_QUOTED_STRING = new StratosElementType("RAW_SINGLE_QUOTED_STRING");
  IElementType RAW_TRIPLE_QUOTED_STRING = new StratosElementType("RAW_TRIPLE_QUOTED_STRING");

  IElementType LONG_TEMPLATE_ENTRY_END = new StratosElementType("LONG_TEMPLATE_ENTRY_END");
  IElementType LONG_TEMPLATE_ENTRY_START = new StratosElementType("LONG_TEMPLATE_ENTRY_START");

  IElementType SHORT_TEMPLATE_ENTRY_START = new StratosElementType("SHORT_TEMPLATE_ENTRY_START");

  IElementType THIS = new StratosTokenType("THIS");
  IElementType MODULAS = new StratosTokenType("modulas");
  IElementType DOUBLE = new StratosTokenType("double");
  IElementType EXTENDS = new StratosTokenType("extends");
  IElementType UNIT = new StratosTokenType("UNIT");
  IElementType OVERRIDE = new StratosTokenType("override");
  IElementType IMPLEMENTS = new StratosTokenType("implements");
  IElementType PACKAGE = new StratosTokenType("package");
  IElementType IMPORT = new StratosTokenType("import");
  IElementType ENUM = new StratosTokenType("enum");
  IElementType FUNCTION = new StratosTokenType("function");
  IElementType IDENTIFIER = new StratosTokenType("IDENTIFIER");
  IElementType INT = new StratosTokenType("int");
  IElementType NUMBER = new StratosTokenType("number");
  IElementType AT = new StratosTokenType("at");
  IElementType VALUE = new StratosTokenType("value");
  IElementType VARIABLE = new StratosTokenType("variable");
  IElementType MUTABLE = new StratosTokenType("mutable");
  IElementType LET = new StratosTokenType("let");
  IElementType RETURNS = new StratosTokenType("returns");
  IElementType STRING = new StratosTokenType("string");
  IElementType BOOL = new StratosTokenType("BOOL");
  IElementType TRUE = new StratosTokenType("TRUE");
  IElementType FALSE = new StratosTokenType("FALSE");
  IElementType HASH = new StratosTokenType("HASH");
  IElementType STRUCT = new StratosTokenType("struct");

  IElementType MULTI_LINE_COMMENT_START = new StratosElementType("MULTI_LINE_COMMENT_START");

  IElementType MULTI_LINE_DOC_COMMENT_START = new StratosElementType("MULTI_LINE_DOC_COMMENT_START");
  IElementType MULTI_LINE_COMMENT_BODY = new StratosElementType("MULTI_LINE_COMMENT_BODY");
  IElementType DOC_COMMENT_LEADING_ASTERISK = new StratosElementType("DOC_COMMENT_LEADING_ASTERISK");
  IElementType MULTI_LINE_COMMENT_END = new StratosElementType("MULTI_LINE_COMMENT_END");
  IElementType REGULAR_STRING_PART = new StratosElementType("REGULAR_STRING_PART");

  IElementType CLOSING_QUOTE = new StratosElementType("CLOSING_QUOTE");
  IElementType OPEN_QUOTE = new StratosElementType("OPEN_QUOTE");
  IElementType SINGLE_LINE_COMMENT = new StratosElementType("SINGLE_LINE_COMMENT");
  IElementType SINGLE_LINE_DOC_COMMENT = new StratosElementType("SINGLE_LINE_DOC_COMMENT");
  IElementType MULTI_LINE_COMMENT = new StratosElementType("MULTI_LINE_COMMENT");
//  IElementType MULTI_LINE_DOC_COMMENT = new DartDocCommentElementType();

//  IElementType LAZY_PARSEABLE_BLOCK = new DartLazyParseableBlockElementType();

  TokenSet STRINGS = TokenSet.create(RAW_SINGLE_QUOTED_STRING, RAW_TRIPLE_QUOTED_STRING, OPEN_QUOTE, CLOSING_QUOTE, REGULAR_STRING_PART);
  TokenSet COMMENTS = TokenSet.create(SINGLE_LINE_COMMENT, SINGLE_LINE_DOC_COMMENT, MULTI_LINE_COMMENT);

  TokenSet WHITE_SPACES = TokenSet.create(WHITE_SPACE);


  class Factory {
    public static PsiElement createElement(ASTNode node) {
      IElementType type = node.getElementType();
      if (type == ASSIGNMENT_STATEMENT) {
        return new StratosAssignmentStatementImpl(node);
      }
      else if (type == BASIC_VARIABLE_DEFINITION) {
        return new StratosBasicVariableDefinitionImpl(node);
      }
      else if (type == DEFINITION) {
        return new StratosDefinitionImpl(node);
      }
      else if (type == ENUM_TYPE) {
        return new StratosEnumTypeImpl(node);
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
      else if (type == PACKAGE_DEFINITION) {
        return new StratosPackageDefinitionImpl(node);
      }
      else if (type == STATEMENT) {
        return new StratosStatementImpl(node);
      }
      else if (type == STRUCT_TYPE) {
        return new StratosStructTypeImpl(node);
      }
      else if (type == TYPE) {
        return new StratosTypeImpl(node);
      }
      else if (type == TYPE_PACKAGE) {
        return new StratosTypePackageImpl(node);
      }
      else if (type == TYPE_WRAPPER) {
        return new StratosTypeWrapperImpl(node);
      }
      else if (type == VARIABLE_DEFINITION) {
        return new StratosVariableDefinitionImpl(node);
      }
      throw new AssertionError("Unknown element type: " + type);
    }
  }
}
