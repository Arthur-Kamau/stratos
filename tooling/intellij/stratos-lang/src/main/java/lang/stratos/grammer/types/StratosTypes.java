// This is a generated file. Not intended for manual editing.
package lang.stratos.grammer.types;

import com.intellij.psi.tree.IElementType;
import com.intellij.psi.PsiElement;
import com.intellij.lang.ASTNode;
import lang.stratos.grammer.StratosElementType;
import lang.stratos.grammer.StratosTokenType;
import lang.stratos.grammer.psi.impl.*;

public interface StratosTypes {

  IElementType PROPERTY = new StratosElementType("PROPERTY");

  IElementType COMMENT = new StratosTokenType("COMMENT");
  IElementType CRLF = new StratosTokenType("CRLF");
  IElementType KEY = new StratosTokenType("KEY");
  IElementType SEPARATOR = new StratosTokenType("SEPARATOR");
  IElementType VALUE = new StratosTokenType("VALUE");

  class Factory {
    public static PsiElement createElement(ASTNode node) {
      IElementType type = node.getElementType();
      if (type == PROPERTY) {
        return new StratosPropertyImpl(node);
      }
      throw new AssertionError("Unknown element type: " + type);
    }
  }
}
