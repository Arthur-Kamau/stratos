package com.araizen.com.Lexer;

import com.araizen.com.model.Node;
import com.araizen.com.model.NodeType;
import com.araizen.com.util.StringUtil.StringUtil;

import java.io.BufferedReader;
import java.io.FileNotFoundException;
import java.io.FileReader;
import java.io.IOException;
import java.nio.charset.Charset;
import java.nio.file.Files;
import java.nio.file.Paths;
import java.util.ArrayList;
import java.util.List;

public class Lexer {
    List<Node> nodesList = new ArrayList<>();
    int lineNumber = 0;
    int currentLineCharacter = 0;
    int currentCharacter;
    String sourceCode;


    public List<Node> generateNodes(String file) throws Exception {


        byte[] bytes = Files.readAllBytes(Paths.get(file));
        sourceCode = new String(bytes, Charset.defaultCharset());
        while (!isAtEnd()) {

            scanFileData();

        }
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
                            "!="
                    ));
                } else {
                    nodesList.add(new Node(
                            currentLineCharacter,
                            currentLineCharacter,
                            lineNumber,
                            lineNumber,
                            NodeType.NotNodeType,
                            "!"
                    ));
                }

                break;
            case '(':
                nodesList.add(new Node(
                        currentLineCharacter,
                        currentLineCharacter,
                        lineNumber,
                        lineNumber,
                        NodeType.CurlyBracketOpenNode,
                        "("
                ));
                break;
            case ')':
                nodesList.add(new Node(
                        currentLineCharacter,
                        currentLineCharacter,
                        lineNumber,
                        lineNumber,
                        NodeType.CurlyBracketCloseNode,
                        ")"
                ));
                break;

            case '[':
                nodesList.add(new Node(
                        currentLineCharacter,
                        currentLineCharacter,
                        lineNumber,
                        lineNumber,
                        NodeType.SquareBracketOpenNode,
                        "["
                ));
                break;
            case ']':
                nodesList.add(new Node(
                        currentLineCharacter,
                        currentLineCharacter,
                        lineNumber,
                        lineNumber,
                        NodeType.SquareBracketCloseNode,
                        "]"
                ));
                break;

            case '{':
                nodesList.add(new Node(
                        currentLineCharacter,
                        currentLineCharacter,
                        lineNumber,
                        lineNumber,
                        NodeType.CurvedBracketOpenNode,
                        "{"
                ));
                break;
            case '}':
                nodesList.add(new Node(
                        currentLineCharacter,
                        currentLineCharacter,
                        lineNumber,
                        lineNumber,
                        NodeType.CurvedBracketCloseNode,
                        "}"
                ));
                break;

            case '%':
                nodesList.add(new Node(
                        currentLineCharacter,
                        currentLineCharacter,
                        lineNumber,
                        lineNumber,
                        NodeType.ModulasNode,
                        "%"
                ));
                break;
            case '+':
                nodesList.add(new Node(
                        currentLineCharacter,
                        currentLineCharacter,
                        lineNumber,
                        lineNumber,
                        NodeType.AddNode,
                        "+"
                ));
                break;


            case '-':
                if (isDigit(peek())) {

                    String negativeInteger = "";
                    int negativeIntegerLineCharacterStart = currentLineCharacter;
                    while (peek() != '{' || peek() != '}' || peek() != '[' || peek() != ']' || peek() != '(' || peek() != ')' || peek() != ';' || peek() != '\t' || peek() != '\r' || peek() != '\n' || peek() != ' ') {

                        negativeInteger += advance();
                    }
                    nodesList.add(new Node(
                            negativeIntegerLineCharacterStart,
                            currentLineCharacter,
                            lineNumber,
                            lineNumber,
                            NodeType.NegativeNumericNode,
                            negativeInteger
                    ));
                } else {
                    nodesList.add(new Node(
                            currentLineCharacter,
                            currentLineCharacter,
                            lineNumber,
                            lineNumber,
                            NodeType.SubtractNode,
                            "-"
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
                            "=="
                    ));
                } else {
                    nodesList.add(new Node(
                            currentLineCharacter,
                            currentLineCharacter,
                            lineNumber,
                            lineNumber,
                            NodeType.AssignNode,
                            "="
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
                            "//" + commentLine
                    ));
                } else if (peek() == '*') {
                    // read the extra  * that both make  start of multi comment
                    advance();
                    String multiLineComment = "";
                    while (peek() != '*' && peekDouble() != '/') {
                        char c1 = advance();
                        if (c1 == '\n') {
                            currentLineCharacter = 0;
                            lineNumber++;
                        }
                        multiLineComment += c1;

                    }

                    // consume  * and /  that make end of multi line comment
                    advance();
                    advance();

                } else {
                    nodesList.add(new Node(
                            currentLineCharacter,
                            currentLineCharacter,
                            lineNumber,
                            lineNumber,
                            NodeType.DivideNode,
                            "/"
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
                        stringData
                ));


                break;
            default:

                String alphanumeric = "";
                int alphaLineCharacterStart = currentLineCharacter;
                if (isAlpha(c)) {
                    while (peek() != '{' || peek() != '}' || peek() != '[' || peek() != ']' || peek() != '(' || peek() != ')' || peek() != ';' || peek() != '\t' || peek() != '\r' || peek() != '\n' || peek() != ' ') {

                        alphanumeric += advance();
                    }
                    if (new StringUtil().isNumeric(alphanumeric)) {
                        nodesList.add(new Node(
                                alphaLineCharacterStart,
                                currentLineCharacter,
                                lineNumber,
                                lineNumber,
                                NodeType.NumericNode,
                                alphanumeric
                        ));
                    } else {
                        nodesList.add(new Node(
                                alphaLineCharacterStart,
                                currentLineCharacter,
                                lineNumber,
                                lineNumber,
                                NodeType.AlphaNumericNode,
                                alphanumeric
                        ));
                    }
                } else if (c == '\n') {
                    currentLineCharacter = 0;
                    lineNumber++;
                } else if (c == '\r' || c == '\t' || c == ' ') {
                    System.out.println("empty space token");
                } else {

                }

                break;

        }

    }

    private char advance() {
        currentLineCharacter++;
        currentCharacter++;
        return sourceCode.charAt(currentCharacter - 1);
    }

    private char peek() {
        return sourceCode.charAt(currentCharacter + 1);
    }

    private char peekDouble() {
        return sourceCode.charAt(currentCharacter + 2);
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
        return currentCharacter >= sourceCode.length();
    }


}
