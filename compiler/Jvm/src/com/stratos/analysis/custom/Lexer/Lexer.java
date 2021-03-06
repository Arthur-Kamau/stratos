package com.stratos.analysis.custom.Lexer;

import com.stratos.util.Language.Language;
import com.stratos.model.Node;
import com.stratos.model.NodeType;
import com.stratos.util.StringUtil.StringUtil;

import java.nio.charset.Charset;
import java.nio.file.Files;
import java.nio.file.Paths;
import java.util.ArrayList;
import java.util.List;

public class Lexer {
    List<Node> nodesList = new ArrayList<>();
    int lineNumber = 1;
    int currentLineCharacter = 0;
    int currentCharacter;
    String sourceCode;


    public List<Node> generateNodes(String file) throws Exception {


        byte[] bytes = Files.readAllBytes(Paths.get(file));
        sourceCode = new String(bytes, Charset.defaultCharset());
        while (!isAtEnd()) {


            scanFileData();

        }

        nodesList.add(new Node(
                currentLineCharacter,
                currentLineCharacter,
                lineNumber,
                lineNumber,
                NodeType.EndOfFileNode,
                "\0",
                false
        ));
        return nodesList;
    }

    private void scanFileData() throws Exception {

        char c = advance();
        switch (c) {
            case '!':
                if (peek() == '=') {
                    advance();
                    nodesList.add(new Node(
                            currentLineCharacter,
                            currentLineCharacter,
                            lineNumber,
                            lineNumber,
                            NodeType.NotEqualToNode,
                            "!=" ,
                            false
                    ));
                } else {
                    nodesList.add(new Node(
                            currentLineCharacter,
                            currentLineCharacter,
                            lineNumber,
                            lineNumber,
                            NodeType.NotNodeType,
                            "!" ,
                            false
                    ));
                }

                break;
            case ';':
                nodesList.add(new Node(
                        currentLineCharacter,
                        currentLineCharacter,
                        lineNumber,
                        lineNumber,
                        NodeType.SemiColonNode,
                        ";" ,
                        false
                ));
                break;
            case '\n':
                nodesList.add(new Node(
                        currentLineCharacter,
                        currentLineCharacter,
                        lineNumber,
                        lineNumber,
                        NodeType.NewLineNode,
                        "\\n" ,
                        false
                ));

                lineNumber++;
                break;
            case '(':
                nodesList.add(new Node(
                        currentLineCharacter,
                        currentLineCharacter,
                        lineNumber,
                        lineNumber,
                        NodeType.CurvedBracketOpenNode,
                        "(" ,
                        false
                ));
                break;
            case ')':
                nodesList.add(new Node(
                        currentLineCharacter,
                        currentLineCharacter,
                        lineNumber,
                        lineNumber,
                        NodeType.CurvedBracketCloseNode,
                        ")" ,
                        false
                ));
                break;

            case '[':
                nodesList.add(new Node(
                        currentLineCharacter,
                        currentLineCharacter,
                        lineNumber,
                        lineNumber,
                        NodeType.SquareBracketOpenNode,
                        "[" ,
                        false
                ));
                break;
            case ']':
                nodesList.add(new Node(
                        currentLineCharacter,
                        currentLineCharacter,
                        lineNumber,
                        lineNumber,
                        NodeType.SquareBracketCloseNode,
                        "]" ,
                        false
                ));
                break;

            case '{':
                nodesList.add(new Node(
                        currentLineCharacter,
                        currentLineCharacter,
                        lineNumber,
                        lineNumber,
                        NodeType.CurlyBracketOpenNode,
                        "{" ,
                        false
                ));
                break;
            case '}':
                nodesList.add(new Node(
                        currentLineCharacter,
                        currentLineCharacter,
                        lineNumber,
                        lineNumber,
                        NodeType.CurlyBracketCloseNode,
                        "}" ,
                        false
                ));
                break;

            case '%':
                nodesList.add(new Node(
                        currentLineCharacter,
                        currentLineCharacter,
                        lineNumber,
                        lineNumber,
                        NodeType.ModulasNode,
                        "%" ,
                        false
                ));
                break;
            case '*':
                nodesList.add(new Node(
                        currentLineCharacter,
                        currentLineCharacter,
                        lineNumber,
                        lineNumber,
                        NodeType.MultiplyNode,
                        "*" ,
                        false
                ));
                break;
            case '+':
                nodesList.add(new Node(
                        currentLineCharacter,
                        currentLineCharacter,
                        lineNumber,
                        lineNumber,
                        NodeType.AddNode,
                        "+" ,
                        false
                ));
                break;

            case '&':
                if (peek() == '&') {

                    // consume the extra &
                    advance();
                    nodesList.add(new Node(
                            currentLineCharacter,
                            currentLineCharacter,
                            lineNumber,
                            lineNumber,
                            NodeType.AndAndNode,
                            "&&" ,
                            false
                    ));
                } else {
                    nodesList.add(new Node(
                            currentLineCharacter,
                            currentLineCharacter,
                            lineNumber,
                            lineNumber,
                            NodeType.BinaryAndNode,
                            "&" ,
                            false
                    ));
                }

                break;

            case '|':
                if (peek() == '|') {

                    // consume the extra |
                    advance();
                    nodesList.add(new Node(
                            currentLineCharacter,
                            currentLineCharacter,
                            lineNumber,
                            lineNumber,
                            NodeType.OrNode,
                            "||" ,
                            false
                    ));
                } else {
                    nodesList.add(new Node(
                            currentLineCharacter,
                            currentLineCharacter,
                            lineNumber,
                            lineNumber,
                            NodeType.BinaryOrNode,
                            "|" ,
                            false
                    ));
                }

                break;

            case '-':
                if (isDigit(peek())) {

                    String negativeInteger = "";
                    int negativeIntegerLineCharacterStart = currentLineCharacter;
                    while (peek() != '{' && peek() != '}' && peek() != '[' && peek() != ']' && peek() != '(' && peek() != ')' && peek() != ';' && peek() != '\t' && peek() != '\r' && peek() != '\n' && peek() != ' ') {

                        negativeInteger += advance();
                    }
                    nodesList.add(new Node(
                            negativeIntegerLineCharacterStart,
                            currentLineCharacter,
                            lineNumber,
                            lineNumber,
                            NodeType.NegativeNumericNode,
                            negativeInteger ,
                            false
                    ));
                } else {
                    nodesList.add(new Node(
                            currentLineCharacter,
                            currentLineCharacter,
                            lineNumber,
                            lineNumber,
                            NodeType.SubtractNode,
                            "-" ,
                            false
                    ));
                }

                break;
            case '<':
                if (peek() == '=') {
                    advance();
                    nodesList.add(new Node(
                            currentLineCharacter,
                            currentLineCharacter,
                            lineNumber,
                            lineNumber,
                            NodeType.LessThanOrEqualTo,
                            "<=" ,
                            false
                    ));
                } else {
                    nodesList.add(new Node(
                            currentLineCharacter,
                            currentLineCharacter,
                            lineNumber,
                            lineNumber,
                            NodeType.LessThan,
                            "<" ,
                            false
                    ));
                }
                break;
            case '>':
                if (peek() == '=') {
                    advance();
                    nodesList.add(new Node(
                            currentLineCharacter,
                            currentLineCharacter,
                            lineNumber,
                            lineNumber,
                            NodeType.GreaterThanOrEqualTo,
                            ">=",
                            false
                    ));
                } else {
                    nodesList.add(new Node(
                            currentLineCharacter,
                            currentLineCharacter,
                            lineNumber,
                            lineNumber,
                            NodeType.GreaterThan,
                            ">" ,
                            false
                    ));
                }
                break;
            case '=':
                if (peek() == '=') {
                    advance();
                    nodesList.add(new Node(
                            currentLineCharacter,
                            currentLineCharacter,
                            lineNumber,
                            lineNumber,
                            NodeType.EquateNode,
                            "==" ,
                            false
                    ));
                } else {
                    nodesList.add(new Node(
                            currentLineCharacter,
                            currentLineCharacter,
                            lineNumber,
                            lineNumber,
                            NodeType.AssignNode,
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
                    nodesList.add(new Node(
                            currentLineCharacter,
                            currentLineCharacter + 1,
                            lineNumber,
                            lineNumber,
                            NodeType.LineCommentNode,
                            "//" + commentLine ,
                            false
                    ));
                } else if (peek() == '*') {
                    // read the extra  * that both make  start of multi comment
                    advance();
                    int startLineCharcter = currentLineCharacter;
                    int currentNumber = lineNumber;
                    System.out.println("start of multi line comment " + currentCharacter + " peek " + peek());
                    String multiLineComment = "" + c;
                    while (peekDouble() != '/' && !isAtEnd()) {

                        char c1 = advance();
                        if (c1 == '\n') {
                            currentLineCharacter = 0;
                            lineNumber++;
                        }

                        multiLineComment += c1;

                    }


                    // consume  * and /  that make end of multi line comment
                    multiLineComment += advance();

                    multiLineComment += advance();


                    nodesList.add(new Node(
                            startLineCharcter,
                            currentLineCharacter,
                            currentNumber,
                            lineNumber,
                            NodeType.MultiLineCommentNode,
                            multiLineComment,
                            false
                    ));

                } else {
                    nodesList.add(new Node(
                            currentLineCharacter,
                            currentLineCharacter,
                            lineNumber,
                            lineNumber,
                            NodeType.DivideNode,
                            "/",
                            false
                    ));

                }
                break;
            case '"':


                String stringData = "";
                int stringLineStart = lineNumber;
                int stringLineCharacterStart = currentLineCharacter;
                while (peek() != '"' && !isAtEnd()) {
                    if (peek() == '\n') {
                        currentLineCharacter = 0;
                        lineNumber++;
                    }
                    stringData += advance();
                }
                nodesList.add(new Node(
                        stringLineCharacterStart,
                        currentLineCharacter,
                        stringLineStart,
                        lineNumber,
                        NodeType.StringValue,
                        stringData,
                        false
                ));

                // consume the closing "
                advance();

                break;
            default:

                String alphanumeric = "";
                int alphaLineCharacterStart = currentLineCharacter;
                if (isAlphaNumeric(c)) {

                    alphanumeric += c;
                    while (!isAtEnd()) {
                        if (peek() != '{' &&
                                peek() != '}' &&
                                peek() != '[' &&
                                peek() != ']' &&
                                peek() != '(' &&
                                peek() != ')' &&
                                peek() != '*' &&
                                peek() != '+' &&
                                peek() != '%' &&
                                peek() != '-' &&
                                peek() != '/' &&
                                peek() != ';' &&
                                peek() != '\t' &&
                                peek() != '\r' && peek() != '\n' && peek() != ' ' && peek() != '\0' && peek() != '=') {
                            char c2 = advance();
                            alphanumeric += c2;
                        } else {
                            break;
                        }


                    }

                    if (new StringUtil().isNumeric(alphanumeric)) {
                        nodesList.add(new Node(
                                alphaLineCharacterStart,
                                currentLineCharacter,
                                lineNumber,
                                lineNumber,
                                NodeType.NumericNode,
                                alphanumeric,
                                false
                        ));
                    } else {
                        Language language = new Language();

                        nodesList.add(new Node(
                                alphaLineCharacterStart,
                                currentLineCharacter,
                                lineNumber,
                                lineNumber,
                                language.getKeyWordNodeType(alphanumeric),
                                alphanumeric,
                                false
                        ));


                    }
                } else if (c == '\0') {
                    //System.out.println("endline  token " + c);
                    break;
                } else if (c == '\n') {
                    currentLineCharacter = 0;
                    lineNumber++;
                } else if (c == '\r' || c == '\t' || c == ' ') {
                    // System.out.println("empty space token");
                } else {
                    // System.out.println("unknown  token " + c);
                    nodesList.add(new Node(
                            alphaLineCharacterStart,
                            currentLineCharacter,
                            lineNumber,
                            lineNumber,
                            NodeType.UnknownNode,
                            "" + c,
                            false
                    ));
                }

                break;

        }

    }

    private char advance() {
        if (isAtEnd()) {
            return '\0';
        } else {
            currentLineCharacter++;
            currentCharacter++;
            return sourceCode.charAt(currentCharacter - 1);

        }
    }

    private char peek() {
        if (isAtEnd()) {

            return '\0';
        } else {
            return sourceCode.charAt(currentCharacter);
        }
    }

    private char peekDouble() {
        return sourceCode.charAt(currentCharacter + 1);
    }

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


    private boolean isAtEnd() {

        boolean st = currentCharacter >= sourceCode.length();
        //System.out.println("is at end "+currentCharacter + "length "+ sourceCode.length() +  "is at end " +st);
        return st;
    }


}
