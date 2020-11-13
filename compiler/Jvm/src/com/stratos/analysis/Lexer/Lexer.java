package com.stratos.analysis.Lexer;


import com.stratos.model.Token;
import com.stratos.model.TokenType;
import old.stratos.model.Node;
import old.stratos.model.NodeType;

import static  com.stratos.model.TokenType.*;

import java.nio.charset.Charset;
import java.nio.file.Files;
import java.nio.file.Paths;
import java.util.ArrayList;
import java.util.HashMap;
import java.util.List;
import java.util.Map;


public class Lexer {

    private static final Map<String, TokenType> keywords;

    static {
        keywords = new HashMap<>();
        keywords.put("and",    AND);
        keywords.put("class",  CLASS);
        keywords.put("else",   ELSE);
        keywords.put("false",  FALSE);
        keywords.put("for",    FOR);
        keywords.put("fun",    FUNCTION);
        keywords.put("if",     IF);
        keywords.put("none",    NONE);
        keywords.put("some",    SOME);
        keywords.put("or",     OR);
        keywords.put("print",  PRINT);
        keywords.put("return", RETURN);
        keywords.put("super",  SUPER);
        keywords.put("this",   THIS);
        keywords.put("true",   TRUE);
        keywords.put("var",    VAR);
        keywords.put("while",  WHILE);
    }
    //< keyword-map

    private final String source;
    private final List<Token> tokens = new ArrayList<>();
    //> scan-state
    private int start = 0;
    private int current = 0;
    private int line = 1;
//< scan-state

    Lexer(String source) {
        this.source = source;
    }
    //> scan-tokens
    List<Token> scanTokens() {
        while (!isAtEnd()) {
            // We are at the beginning of the next lexeme.
            start = current;
            scanToken();
        }

        tokens.add(new Token(current, current, line, line,EOF,"",null));
        return tokens;
    }





    private void scanToken() {
        char c = advance();

        switch (c) {
            case '!':
                if (peek() == '=') {
                    advance();
                    tokens.add(new Token(
                            current-2,
                            current,
                            line,
                            line,
                            NOTEQUALTO,
                            "!=" ,
                            false
                    ));
                } else {
                    tokens.add(new Token(
                            current-1,
                            current,
                            line,
                            line,
                            NOT,
                            "!" ,
                            false
                    ));
                }

                break;
            case ';':
                tokens.add(new Token(
                        current-1,
                        current,
                        line,
                        line,
                        SEMICOLON,
                        ";" ,
                        false
                ));
                break;

            case '\n':
                tokens.add(new Token(
                        current-1,
                        current,
                        line,
                        line,
                        NEWLINE,
                        "\\n" ,
                        false
                ));

                line++;
                break;



            case '.':
                tokens.add(new Token(
                        current-1,
                        current,
                        line,
                        line,
                        DOT,
                        "." ,
                        false
                ));

                line++;
                break;

            case ',':
                tokens.add(new Token(
                        current-1,
                        current,
                        line,
                        line,
                        COMMA,
                        "," ,
                        false
                ));

                line++;
                break;

            case '?':
                tokens.add(new Token(
                        current-1,
                        current,
                        line,
                        line,
                        QUESTIONMARK,
                        "?" ,
                        false
                ));

                line++;
                break;


            case ':':
                tokens.add(new Token(
                        current-1,
                        current,
                        line,
                        line,
                        COLON,
                        ";" ,
                        false
                ));

                line++;
                break;



            case '(':
                tokens.add(new Token(
                        current-1,
                        current,
                        line,
                        line,
                        LEFT_PAREN,
                        "(" ,
                        false
                ));
                break;
            case ')':
                tokens.add(new Token(
                        current-1,   
                        current,
                        line,
                        line,
                        RIGHT_PAREN,
                        ")" ,
                        false
                ));
                break;

            case '[':
                tokens.add(new Token(
                        current-1,   
                        current,
                        line,
                        line,
                        LEFT_SQUARE,
                        "[" ,
                        false
                ));
                break;
            case ']':
                tokens.add(new Token(
                        current-1,   
                        current,
                        line,
                        line,
                        RIGHT_SQUARE,
                        "]" ,
                        false
                ));
                break;

            case '{':
                tokens.add(new Token(
                        current-1,   
                        current,
                        line,
                        line,
                        LEFT_BRACE,
                        "{" ,
                        false
                ));
                break;
            case '}':
                tokens.add(new Token(
                        current-1,   
                        current,
                        line,
                        line,
                        RIGHT_BRACE,
                        "}" ,
                        false
                ));
                break;

            case '%':
                tokens.add(new Token(
                        current-1,   
                        current,
                        line,
                        line,
                        MODULAS,
                        "%" ,
                        false
                ));
                break;
            case '*':
                tokens.add(new Token(
                        current-1,   
                        current,
                        line,
                        line,
                        MULTIPLY,
                        "*" ,
                        false
                ));
                break;
            case '+':
                tokens.add(new Token(
                        current-1,   
                        current,
                        line,
                        line,
                        ADD,
                        "+" ,
                        false
                ));
                break;

            case '&':
                if (peek() == '&') {

                    // consume the extra &
                    advance();
                    tokens.add(new Token(
                            current-1,   
                        current,
                        line,
                        line,
                            AND,
                            "&&" ,
                            false
                    ));
                } else {
                    tokens.add(new Token(
                            current-1,   
                        current,
                        line,
                        line,
                            BINARYAND,
                            "&" ,
                            false
                    ));
                }

                break;

            case '|':
                if (peek() == '|') {

                    // consume the extra |
                    advance();
                    tokens.add(new Token(
                            current-1,   
                        current,
                        line,
                        line,
                            OR,
                            "||" ,
                            false
                    ));
                } else {
                    tokens.add(new Token(
                            current-1,   
                        current,
                        line,
                        line,
                            BINARYOR,
                            "|" ,
                            false
                    ));
                }

                break;



            case '-':
                tokens.add(new Token(
                        current-1,
                        current,
                        line,
                        line,
                        SUBTRACT,
                        "-" ,
                        false
                ));
        

        break;




        
        
        

            case '<':
                if (peek() == '=') {
                    advance();
                     tokens.add(new Token(
                            current-1,
                        current,
                        line,
                        line,
                            LESSTHANOREQUALTO,
                            "<=" ,
                            false
                    ));
                } else {
                     tokens.add(new Token(
                            current-1,
                        current,
                        line,
                        line,
                            LESSTHAN,
                            "<" ,
                            false
                    ));
                }
                break;
            case '>':
                if (peek() == '=') {
                    advance();
                     tokens.add(new Token(
                            current-2,
                        current,
                        line,
                        line,
                            GREATERTHANOREQUALTO,
                            ">=",
                            false
                    ));
                } else {
                     tokens.add(new Token(
                            current-1,
                        current,
                        line,
                        line,
                             GREATERTHAN,
                            ">" ,
                            false
                    ));
                }
                break;
            case '=':
                if (peek() == '=') {
                    advance();
                     tokens.add(new Token(
                            current-1,
                        current,
                        line,
                        line,
                            EQUALTO,
                            "==" ,
                            false
                    ));
                } else {
                     tokens.add(new Token(
                            current-1,
                        current,
                        line,
                        line,
                            ASSIGN,
                            "=" ,
                            false
                    ));
                }

                break;


            case '/':
                if (peek() == '/') {


                    // read the extra  / that both make a line comment
                    advance();


                    String commentLine = "";
                    while (peek() != '\n') {
                        commentLine += advance();
                    }
                     tokens.add(new Token(
                            current-1,
                             current,
                             line,
                             line,
                             LINECOMMENT,
                            "//" + commentLine ,
                            false
                    ));
                } else if (peek() == '*') {
                    // read the extra  * that both make  start of multi comment
                    advance();
                    int startLineCharcter = current;
                    int currentNumber = line;
                    //System.out.println("start of multi line comment " + currentCharacter + " peek " + peek());
                    String multiLineComment = "" + c;
                    while (peekNext() != '/' && !isAtEnd()) {

                        char c1 = advance();
                        if (c1 == '\n') {
                            current = 0;
                            line++;
                        }

                        multiLineComment += c1;

                    }


                    // consume  * and /  that make end of multi line comment
                    multiLineComment += advance();

                    multiLineComment += advance();


                     tokens.add(new Token(
                            startLineCharcter,
                             current,
                            currentNumber,
                             line,
                            MULTILINECOMMENT,
                            multiLineComment,
                            false
                    ));

                } else {
                     tokens.add(new Token(
                            current-1,
                        current,
                        line,
                        line,
                            DIVIDE,
                            "/",
                            false
                    ));

                }
                break;
            case '"':


                String stringData = "";
                int stringLineStart = line;
                int stringLineCharacterStart = current;
                while (peek() != '"' && !isAtEnd()) {
                    if (peek() == '\n') {
                        current = 0;
                        line++;
                    }
                    stringData += advance();
                }
                 tokens.add(new Token(
                        stringLineCharacterStart,
                        current,
                        stringLineStart,
                        line,
                         STRING,
                        stringData,
                         stringData
                ));

                // consume the closing "
                advance();

                break;










            default:

                break;

    }
    }




    //> peek
    private char peek() {
        if (isAtEnd()) return '\0';
        return source.charAt(current);
    }
    //< peek
//> peek-next
    private char peekNext() {
        if (current + 1 >= source.length()) return '\0';
        return source.charAt(current + 1);
    } // [peek-next]
//< peek-next

    //> is-alpha
    private boolean isAlpha(char c) {
        return (c >= 'a' && c <= 'z') ||
                (c >= 'A' && c <= 'Z') ||
                c == '_';
    }


    private boolean isAlphaNumeric(char c) {
        return isAlpha(c) || isDigit(c);
    }
    //< is-alpha
//> is-digit
    private boolean isDigit(char c) {
        return c >= '0' && c <= '9';
    } // [is-digit]
    //< is-digit
//> is-at-end
    private boolean isAtEnd() {
        return current >= source.length();
    }

    private char advance() {
        current++;
        return source.charAt(current - 1);
    }



}




//
//    List<Token> nodesList = new ArrayList<>();
//    int lineNumber = 1;
//    int currentLineCharacter = 0;
//    int currentCharacter;
//    String sourceCode;
//
//
//    public List<Token> scanTokens(String file) throws Exception {
//
//
//        byte[] bytes = Files.readAllBytes(Paths.get(file));
//        sourceCode = new String(bytes, Charset.defaultCharset());
//        while (!isAtEnd()) {
//
//
//            scanFileData();
//
//        }
//
//        tokens.add(new Token(
//                currentLineCharacter,
//                currentLineCharacter,
//                lineNumber,
//                lineNumber,
//                TokenType.EOF,
//                "\0",
//                false
//        ));
//        return nodesList;
//    }
//
//    private void scanFileData() throws Exception {
//
//        char c = advance();
//        switch (c) {
//            case '!':
//                if (peek() == '=') {
//                    advance();
//                    tokens.add(new Token(
//                            currentLineCharacter,
//                            currentLineCharacter,
//                            lineNumber,
//                            lineNumber,
//                            TokenType.NotEqualToNode,
//                            "!=" ,
//                            false
//                    ));
//                } else {
//                    tokens.add(new Token(
//                            currentLineCharacter,
//                            currentLineCharacter,
//                            lineNumber,
//                            lineNumber,
//                            TokenType.NotNodeType,
//                            "!" ,
//                            false
//                    ));
//                }
//
//                break;
//            case ';':
//                tokens.add(new Token(
//                        currentLineCharacter,
//                        currentLineCharacter,
//                        lineNumber,
//                        lineNumber,
//                        TokenType.SemiColonNode,
//                        ";" ,
//                        false
//                ));
//                break;
//            case '\n':
//                tokens.add(new Token(
//                        currentLineCharacter,
//                        currentLineCharacter,
//                        lineNumber,
//                        lineNumber,
//                        TokenType.NewLineNode,
//                        "\\n" ,
//                        false
//                ));
//
//                lineNumber++;
//                break;
//            case '(':
//                tokens.add(new Token(
//                        currentLineCharacter,
//                        currentLineCharacter,
//                        lineNumber,
//                        lineNumber,
//                        TokenType.CurvedBracketOpenNode,
//                        "(" ,
//                        false
//                ));
//                break;
//            case ')':
//                tokens.add(new Token(
//                        currentLineCharacter,
//                        currentLineCharacter,
//                        lineNumber,
//                        lineNumber,
//                        TokenType.CurvedBracketCloseNode,
//                        ")" ,
//                        false
//                ));
//                break;
//
//            case '[':
//                tokens.add(new Token(
//                        currentLineCharacter,
//                        currentLineCharacter,
//                        lineNumber,
//                        lineNumber,
//                        TokenType.SquareBracketOpenNode,
//                        "[" ,
//                        false
//                ));
//                break;
//            case ']':
//                tokens.add(new Token(
//                        currentLineCharacter,
//                        currentLineCharacter,
//                        lineNumber,
//                        lineNumber,
//                        TokenType.SquareBracketCloseNode,
//                        "]" ,
//                        false
//                ));
//                break;
//
//            case '{':
//                tokens.add(new Token(
//                        currentLineCharacter,
//                        currentLineCharacter,
//                        lineNumber,
//                        lineNumber,
//                        TokenType.CurlyBracketOpenNode,
//                        "{" ,
//                        false
//                ));
//                break;
//            case '}':
//                tokens.add(new Token(
//                        currentLineCharacter,
//                        currentLineCharacter,
//                        lineNumber,
//                        lineNumber,
//                        TokenType.CurlyBracketCloseNode,
//                        "}" ,
//                        false
//                ));
//                break;
//
//            case '%':
//                tokens.add(new Token(
//                        currentLineCharacter,
//                        currentLineCharacter,
//                        lineNumber,
//                        lineNumber,
//                        TokenType.ModulasNode,
//                        "%" ,
//                        false
//                ));
//                break;
//            case '*':
//                tokens.add(new Token(
//                        currentLineCharacter,
//                        currentLineCharacter,
//                        lineNumber,
//                        lineNumber,
//                        TokenType.MultiplyNode,
//                        "*" ,
//                        false
//                ));
//                break;
//            case '+':
//                tokens.add(new Token(
//                        currentLineCharacter,
//                        currentLineCharacter,
//                        lineNumber,
//                        lineNumber,
//                        TokenType.AddNode,
//                        "+" ,
//                        false
//                ));
//                break;
//
//            case '&':
//                if (peek() == '&') {
//
//                    // consume the extra &
//                    advance();
//                    tokens.add(new Token(
//                            currentLineCharacter,
//                            currentLineCharacter,
//                            lineNumber,
//                            lineNumber,
//                            TokenType.AndAndNode,
//                            "&&" ,
//                            false
//                    ));
//                } else {
//                    tokens.add(new Token(
//                            currentLineCharacter,
//                            currentLineCharacter,
//                            lineNumber,
//                            lineNumber,
//                            TokenType.BinaryAndNode,
//                            "&" ,
//                            false
//                    ));
//                }
//
//                break;
//
//            case '|':
//                if (peek() == '|') {
//
//                    // consume the extra |
//                    advance();
//                    tokens.add(new Token(
//                            currentLineCharacter,
//                            currentLineCharacter,
//                            lineNumber,
//                            lineNumber,
//                            TokenType.OrNode,
//                            "||" ,
//                            false
//                    ));
//                } else {
//                    tokens.add(new Token(
//                            currentLineCharacter,
//                            currentLineCharacter,
//                            lineNumber,
//                            lineNumber,
//                            TokenType.BinaryOrNode,
//                            "|" ,
//                            false
//                    ));
//                }
//
//                break;
//
//            case '-':
//                if (isDigit(peek())) {
//
//                    String negativeInteger = "";
//                    int negativeIntegerLineCharacterStart = currentLineCharacter;
//                    while (peek() != '{' && peek() != '}' && peek() != '[' && peek() != ']' && peek() != '(' && peek() != ')' && peek() != ';' && peek() != '\t' && peek() != '\r' && peek() != '\n' && peek() != ' ') {
//
//                        negativeInteger += advance();
//                    }
//                    tokens.add(new Token(
//                            negativeIntegerLineCharacterStart,
//                            currentLineCharacter,
//                            lineNumber,
//                            lineNumber,
//                            TokenType.NegativeNumericNode,
//                            negativeInteger ,
//                            false
//                    ));
//                } else {
//                    tokens.add(new Token(
//                            currentLineCharacter,
//                            currentLineCharacter,
//                            lineNumber,
//                            lineNumber,
//                            TokenType.SubtractNode,
//                            "-" ,
//                            false
//                    ));
//                }
//
//                break;
//            case '<':
//                if (peek() == '=') {
//                    advance();
//                    tokens.add(new Token(
//                            currentLineCharacter,
//                            currentLineCharacter,
//                            lineNumber,
//                            lineNumber,
//                            TokenType.LessThanOrEqualTo,
//                            "<=" ,
//                            false
//                    ));
//                } else {
//                    tokens.add(new Token(
//                            currentLineCharacter,
//                            currentLineCharacter,
//                            lineNumber,
//                            lineNumber,
//                            TokenType.LessThan,
//                            "<" ,
//                            false
//                    ));
//                }
//                break;
//            case '>':
//                if (peek() == '=') {
//                    advance();
//                    tokens.add(new Token(
//                            currentLineCharacter,
//                            currentLineCharacter,
//                            lineNumber,
//                            lineNumber,
//                            TokenType.GreaterThanOrEqualTo,
//                            ">=",
//                            false
//                    ));
//                } else {
//                    tokens.add(new Token(
//                            currentLineCharacter,
//                            currentLineCharacter,
//                            lineNumber,
//                            lineNumber,
//                            TokenType.GreaterThan,
//                            ">" ,
//                            false
//                    ));
//                }
//                break;
//            case '=':
//                if (peek() == '=') {
//                    advance();
//                    tokens.add(new Token(
//                            currentLineCharacter,
//                            currentLineCharacter,
//                            lineNumber,
//                            lineNumber,
//                            TokenType.EquateNode,
//                            "==" ,
//                            false
//                    ));
//                } else {
//                    tokens.add(new Token(
//                            currentLineCharacter,
//                            currentLineCharacter,
//                            lineNumber,
//                            lineNumber,
//                            TokenType.AssignNode,
//                            "=" ,
//                            false
//                    ));
//                }
//
//                break;
//
//
//            case '/':
//                if (peek() == '/') {
//
//
//                    // read the extra  / that both make a line comment
//                    advance();
//
//
//                    String commentLine = "";
//                    while (peek() != '\n') {
//                        commentLine += advance();
//                    }
//                    tokens.add(new Token(
//                            currentLineCharacter,
//                            currentLineCharacter + 1,
//                            lineNumber,
//                            lineNumber,
//                            TokenType.LineCommentNode,
//                            "//" + commentLine ,
//                            false
//                    ));
//                } else if (peek() == '*') {
//                    // read the extra  * that both make  start of multi comment
//                    advance();
//                    int startLineCharcter = currentLineCharacter;
//                    int currentNumber = lineNumber;
//                    System.out.println("start of multi line comment " + currentCharacter + " peek " + peek());
//                    String multiLineComment = "" + c;
//                    while (peekDouble() != '/' && !isAtEnd()) {
//
//                        char c1 = advance();
//                        if (c1 == '\n') {
//                            currentLineCharacter = 0;
//                            lineNumber++;
//                        }
//
//                        multiLineComment += c1;
//
//                    }
//
//
//                    // consume  * and /  that make end of multi line comment
//                    multiLineComment += advance();
//
//                    multiLineComment += advance();
//
//
//                    tokens.add(new Token(
//                            startLineCharcter,
//                            currentLineCharacter,
//                            currentNumber,
//                            lineNumber,
//                            TokenType.MultiLineCommentNode,
//                            multiLineComment,
//                            false
//                    ));
//
//                } else {
//                    tokens.add(new Token(
//                            currentLineCharacter,
//                            currentLineCharacter,
//                            lineNumber,
//                            lineNumber,
//                            TokenType.DivideNode,
//                            "/",
//                            false
//                    ));
//
//                }
//                break;
//            case '"':
//
//
//                String stringData = "";
//                int stringLineStart = lineNumber;
//                int stringLineCharacterStart = currentLineCharacter;
//                while (peek() != '"' && !isAtEnd()) {
//                    if (peek() == '\n') {
//                        currentLineCharacter = 0;
//                        lineNumber++;
//                    }
//                    stringData += advance();
//                }
//                tokens.add(new Token(
//                        stringLineCharacterStart,
//                        currentLineCharacter,
//                        stringLineStart,
//                        lineNumber,
//                        TokenType.StringValue,
//                        stringData,
//                        false
//                ));
//
//                // consume the closing "
//                advance();
//
//                break;
//            default:
//
//                String alphanumeric = "";
//                int alphaLineCharacterStart = currentLineCharacter;
//                if (isAlphaNumeric(c)) {
//
//                    alphanumeric += c;
//                    while (!isAtEnd()) {
//                        if (peek() != '{' &&
//                                peek() != '}' &&
//                                peek() != '[' &&
//                                peek() != ']' &&
//                                peek() != '(' &&
//                                peek() != ')' &&
//                                peek() != '*' &&
//                                peek() != '+' &&
//                                peek() != '%' &&
//                                peek() != '-' &&
//                                peek() != '/' &&
//                                peek() != ';' &&
//                                peek() != '\t' &&
//                                peek() != '\r' && peek() != '\n' && peek() != ' ' && peek() != '\0' && peek() != '=') {
//                            char c2 = advance();
//                            alphanumeric += c2;
//                        } else {
//                            break;
//                        }
//
//
//                    }
//
//                    if (new Util().isNumeric(alphanumeric)) {
//                        tokens.add(new Token(
//                                alphaLineCharacterStart,
//                                currentLineCharacter,
//                                lineNumber,
//                                lineNumber,
//                                TokenType.NumericNode,
//                                alphanumeric,
//                                false
//                        ));
//                    } else {
//
//                        tokens.add(new Token(
//                                alphaLineCharacterStart,
//                                currentLineCharacter,
//                                lineNumber,
//                                lineNumber,
//                                getKeyWordNodeType(alphanumeric),
//                                alphanumeric,
//                                false
//                        ));
//
//
//                    }
//                } else if (c == '\0') {
//                    //System.out.println("endline  token " + c);
//                    break;
//                } else if (c == '\n') {
//                    currentLineCharacter = 0;
//                    lineNumber++;
//                } else if (c == '\r' || c == '\t' || c == ' ') {
//                    // System.out.println("empty space token");
//                } else {
//                    // System.out.println("unknown  token " + c);
//                    tokens.add(new Token(
//                            alphaLineCharacterStart,
//                            currentLineCharacter,
//                            lineNumber,
//                            lineNumber,
//                            TokenType.UnknownNode,
//                            "" + c,
//                            false
//                    ));
//                }
//
//                break;
//
//        }
//
//    }
//
//    private char advance() {
//        if (isAtEnd()) {
//            return '\0';
//        } else {
//            currentLineCharacter++;
//            currentCharacter++;
//            return sourceCode.charAt(currentCharacter - 1);
//
//        }
//    }
//
//    private char peek() {
//        if (isAtEnd()) {
//
//            return '\0';
//        } else {
//            return sourceCode.charAt(currentCharacter);
//        }
//    }
//
//    private char peekDouble() {
//        return sourceCode.charAt(currentCharacter + 1);
//    }
//
//    private boolean isAlpha(char c) {
//        return (c >= 'a' && c <= 'z') ||
//                (c >= 'A' && c <= 'Z') ||
//                c == '_';
//    }
//
//    private boolean isAlphaNumeric(char c) {
//        return isAlpha(c) || isDigit(c);
//    }
//
//    //< is-alpha
////> is-digit
//    private boolean isDigit(char c) {
//        return c >= '0' && c <= '9';
//    } // [is-digit]
//
//
//    private boolean isAtEnd() {
//
//        boolean st = currentCharacter >= sourceCode.length();
//        //System.out.println("is at end "+currentCharacter + "length "+ sourceCode.length() +  "is at end " +st);
//        return st;
//    }
//
//
//
//    public TokenType getKeyWordNodeType(String text) {
//        switch (text) {
//
//            case "var":
//                return TokenType.VarNode;
//            case "val":
//                return TokenType.ValNode;
//            case "when":
//                return TokenType.WhenNode;
//            case "let":
//                return TokenType.LetNode;
//            case "list":
//                return TokenType.ListNode;
//            case "if":
//                return TokenType.IfNode;
//            case "else":
//                return TokenType.ElseNode;
//            case "enum":
//                return TokenType.EnumNode;
//            case "strut":
//                return TokenType.StructNode;
//            case "double":
//                return TokenType.DoubleNode;
//            case "int":
//                return TokenType.IntegerNode;
//            case "string":
//                return TokenType.StringNode;
//            case "char":
//                return TokenType.CharacterNode;
//            case "alias":
//                return TokenType.AliasNode;
//            case "class":
//                return TokenType.ClassNode;
//            case "then":
//                return TokenType.ThenNode;
//            case "or":
//                return TokenType.OrNode;
//            case "and":
//                return TokenType.AndNode;
//            case "function":
//                return TokenType.FunctionNode;
//            case "some":
//                return TokenType.SomeNode;
//            case "none":
//                return TokenType.NoneNode;
//            case "return":
//                return TokenType.ReturnNode;
//            case "package":
//                return TokenType.PackageNode;
//            case "import":
//                return TokenType.ImportNode;
//            case "implements":
//                return TokenType.ImplementsNode;
//            case "extends":
//                return TokenType.ExtendsNode;
//            case "private":
//                return TokenType.PrivateNode;
//
//            default:
//                return TokenType.AlphaNumericNode;
//        }
//
//
//    }
//}
