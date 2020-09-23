
//
//%%
//%{
//  private static final class State {
//    final int lBraceCount;
//    final int state;
//
//    private State(int state, int lBraceCount) {
//      this.state = state;
//      this.lBraceCount = lBraceCount;
//    }
//
//    @Override
//    public String toString() {
//      return "yystate = " + state + (lBraceCount == 0 ? "" : "lBraceCount = " + lBraceCount);
//    }
//  }
//
//  protected final Stack<State> myStateStack = new Stack<>();
//  protected int myLeftBraceCount;
//
//  private void pushState(int state) {
//    myStateStack.push(new State(yystate(), myLeftBraceCount));
//    myLeftBraceCount = 0;
//    yybegin(state);
//  }
//
//  private void popState() {
//    State state = myStateStack.pop();
//    myLeftBraceCount = state.lBraceCount;
//    yybegin(state.state);
//  }
//
//  StratosLexer() {
//    this(null);
//  }
//%}
//
//%public
//%class StratosLexer // Name of the lexer class which will be generated.
//%implements FlexLexer  // Name of the super class.
//%function advance
//%type IElementType
//%unicode
//%eof{
//  myLeftBraceCount = 0;
//  myStateStack.clear();
//%eof}
//// We define various Lexer rules as regular expressions first.
//
//
//// If some character sequence is matched to this regex, it will be treated as an IDENTIFIER.
////IDENTIFIER=[a-zA-Z_][a-zA-Z0-9_]*
//
//// If some character sequence is matched to this regex, it will be treated as a WHITE_SPACE.
////WHITE_SPACE=[ \t\n\x0B\f\r]+
//
//
//DIGIT=[0-9]
//HEX_DIGIT=[0-9a-fA-F]
//LETTER=[a-z]|[A-Z]
//WHITE_SPACE=[ \n\t\f]+
//
//SINGLE_LINE_COMMENT="/""/"[^\n]*
//SINGLE_LINE_DOC_COMMENT="/""/""/"[^\n]*
//SINGLE_LINE_COMMENTED_COMMENT="/""/""/""/"[^\n]*
//
//MULTI_LINE_DEGENERATE_COMMENT = "/*" "*"+ "/"
//MULTI_LINE_COMMENT_START      = "/*"
//MULTI_LINE_DOC_COMMENT_START  = "/**"
//MULTI_LINE_COMMENT_END        = "*/"
//
//RAW_SINGLE_QUOTED_STRING= "r" ((\" ([^\"\n])* \"?) | ("'" ([^\'\n])* \'?))
//RAW_TRIPLE_QUOTED_STRING= "r" ({RAW_TRIPLE_QUOTED_LITERAL} | {RAW_TRIPLE_APOS_LITERAL})
//
//RAW_TRIPLE_QUOTED_LITERAL = {THREE_QUO}  ([^\"] | \"[^\"] | \"\"[^\"])* {THREE_QUO}?
//RAW_TRIPLE_APOS_LITERAL   = {THREE_APOS} ([^\'] | \'[^\'] | \'\'[^\'])* {THREE_APOS}?
//
//THREE_QUO =  (\"\"\")
//THREE_APOS = (\'\'\')
//
//
//NUMERIC_LITERAL = {NUMBER} | {HEX_NUMBER}
//NUMBER = ({DIGIT}+ ("." {DIGIT}+)? {EXPONENT}?) | ("." {DIGIT}+ {EXPONENT}?)
//EXPONENT = [Ee] ["+""-"]? {DIGIT}*
//HEX_NUMBER = 0 [Xx] {HEX_DIGIT}*
//
//
//
//// Initial state. We can specify mutiple states for more complex grammars. This corresponds to `modes` in ANTLR grammar.
//%%
//<YYINITIAL> {
//  // In here, we match keywords. So if a keyword is found, this returns a token which corresponds to that keyword.
//  // These tokens are generated using the `Ballerina.bnf` file and located in the SimpleTypes `class`.
//  // These tokens are Parser uses these return values to match token squence to a parser rule.
//  "function"         { return FUNCTION; } // This indicates that the `function` keyword is encountered.
//  "yeild"          { return YIELD; }  //
//  "returns"          { return RETURNS; }  //
//  "package"          { return PACKAGE; }  //
//  "if"          { return IF; }  // indicated a nullable , const value
//  "else"          { return ELSE; }  // indicated a nullable , const value
//  "when"          { return WHEN; }  // indicated a nullable , const value
//  "%"          { return MODULAS; }  // remainder of divide operation
//  "/"          { return DIVIDE; }  // divide values
//  "*"          { return MULTIPLY; }  // multiply
//  "+"          { return ADD; }  // add
//  "-"          { return MINUS; }  // minus
//  ">"          { return GREATER_THAN; }  // grter that
//  "<"          { return LESS_THAN; }  // less than
//  "="          { return ASSIGN; }  //  asssign
//  "=="          { return EQUAL; }  // equal
//  ":"          { return COLON; }  // scope operation
//  "!"          { return NOT; }  // not
//  ";"          { return SEMI_COLON; }  //
//  "{"          { return OPEN_BRACKETS; }  //
//  "}"          { return CLOSE_BRACKETS; }  //
//  "("          { return OPEN_BRACE; }  //
//  ")"          { return CLOSE_BRACE; }  //
//  "let"          { return LET; }  // indicated a nullable , const value
//  "var"          { return VARIABLE; }  // mutable nullable type
//  "val"          { return VALUE; }  // immutable nullable type
//  "mut"          { return MUTABLE; }  //
//  "int"              { return INT; }      // This indicates that the `int` keyword is encountered.
//  "@"              { return AT; }      // This indicates that the `int` keyword is encountered.
//  "alias"              { return ALIAS; }      // This indicates that the `int` keyword is encountered.
//  "override"              { return OVERRIDE; }      //
//  "import"              { return IMPORT; }      //
//  "struct"              { return STRUCT; }      //
//  "enum"              { return ENUM; }      //
//  "implement"              { return IMPLEMENTS; }      // This indicates that the `int` keyword is encountered.
//  "extends"              { return EXTENDS; }      //extends class functionality
//  "string"           { return STRING; }   //string data type
//  "unit"           { return UNIT; }   // empty/nothing
//  "as"           { return AS; }   // soft cast
//  "is"           { return IS; }   // check types is of oother type
//
//  // In here, we check for character sequences which matches regular expressions which we defined above.
////  {IDENTIFIER}       { return IDENTIFIER; } // This indicates that a character sequence which matches to the rule
//                                            // identifier is encountered.
////  {WHITE_SPACE}      { return WHITE_SPACE; } // This indicates that a character sequence which matches to the rule
//                                             // whitespace is encountered.
//
//
//  {SINGLE_LINE_COMMENTED_COMMENT} { return SINGLE_LINE_COMMENT; }
//  {NUMERIC_LITERAL} { return NUMBER; }
//  {RAW_TRIPLE_QUOTED_STRING} { return RAW_TRIPLE_QUOTED_STRING; }
//  {RAW_SINGLE_QUOTED_STRING} { return RAW_SINGLE_QUOTED_STRING; }
//  {WHITE_SPACE}                   { return WHITE_SPACE;             }
//
//
//
//}
//
//
//// If the character sequence does not match any of the above rules, we return BAD_CHARACTER which indicates that
//// there is an error in the character sequence. This is used to highlight errors.
//[^] { return BAD_CHARACTER; }