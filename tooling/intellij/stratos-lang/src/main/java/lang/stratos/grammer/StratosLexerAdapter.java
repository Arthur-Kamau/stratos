package lang.stratos.grammer;


import com.intellij.lexer.FlexAdapter;

        import java.io.Reader;

public class StratosLexerAdapter extends FlexAdapter {

    public StratosLexerAdapter() {
        super(new StratosLexer(null));
    }

}