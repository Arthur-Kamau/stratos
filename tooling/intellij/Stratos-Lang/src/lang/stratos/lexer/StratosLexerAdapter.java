package lang.stratos.lexer;


import com.intellij.lexer.FlexAdapter;


public class StratosLexerAdapter extends FlexAdapter {

    public StratosLexerAdapter() {
        super(new _StratosLexer());
    }

}
