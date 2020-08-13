package com.araizen.stratos.psi;

//public class TokenType {
//}
//

//package com.intellij.psi;

        import com.intellij.lang.Language;
        import com.intellij.psi.tree.IElementType;
        import com.intellij.psi.tree.IFileElementType;

/**
 * The standard lexer token types common to all languages.
 */

public interface TokenType {
    /**
     * Token type for a sequence of whitespace characters.
     */
    IElementType WHITE_SPACE = new IElementType("WHITE_SPACE", Language.ANY);

    /**
     * Token type for a character which is not valid in the position where it was encountered,
     * according to the language grammar.
     */
    IElementType BAD_CHARACTER = new IElementType("BAD_CHARACTER", Language.ANY);

    /**
     * Internal token type used by the code formatter.
     */
    IElementType NEW_LINE_INDENT = new IElementType("NEW_LINE_INDENT", Language.ANY);

    IElementType ERROR_ELEMENT = new IElementType("ERROR_ELEMENT", Language.ANY) {
        @Override
        public boolean isLeftBound() {
            return true;
        }
    };

    IElementType CODE_FRAGMENT = new IFileElementType("CODE_FRAGMENT", Language.ANY);
    IElementType DUMMY_HOLDER = new IFileElementType("DUMMY_HOLDER", Language.ANY);
}
