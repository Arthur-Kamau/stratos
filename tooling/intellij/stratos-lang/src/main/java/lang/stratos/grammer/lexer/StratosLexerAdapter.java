package lang.stratos.grammer.lexer;


import com.intellij.lexer.FlexAdapter;
import lang.stratos.grammer.lexer.StratosLexer;

import java.io.Reader;

public class StratosLexerAdapter extends FlexAdapter {

    public StratosLexerAdapter() {
        super(new StratosLexer(null));
    }

}