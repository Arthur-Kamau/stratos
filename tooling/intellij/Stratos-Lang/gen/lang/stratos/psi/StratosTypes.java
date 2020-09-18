// This is a generated file. Not intended for manual editing.
package lang.stratos.psi;

import com.intellij.psi.tree.IElementType;
import com.intellij.psi.PsiElement;
import com.intellij.lang.ASTNode;
import lang.stratos.language.StratosElementType;
import lang.stratos.language.StratosTokenType;
import lang.stratos.psi.impl.*;

public interface StratosTypes {

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

  IElementType DOUBLE = new StratosTokenType("double");
  IElementType EXTENDS = new StratosTokenType("extends");
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
  IElementType STRUCT = new StratosTokenType("struct");

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
