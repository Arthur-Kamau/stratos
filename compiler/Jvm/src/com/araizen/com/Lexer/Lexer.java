package com.araizen.com.Lexer;

import com.araizen.com.model.Node;
import com.araizen.com.model.NodeType;

import java.io.BufferedReader;
import java.io.FileNotFoundException;
import java.io.FileReader;
import java.io.IOException;
import java.util.ArrayList;
import java.util.List;

public class Lexer {
    int lineNumber = 0;
    int currentLineCharacter = 0;
    int lineLength = 0;
    String currentLine;

    public List<Node> generateNodes(String file) throws IOException {
        List<Node> nodesList = new ArrayList<>();
        BufferedReader reader = new BufferedReader(new FileReader(file));

//        boolean inString  = false;
//        boolean inVerbatimString  = false;
//        String multiLineComment = "";
//        int multiLineCommentStart = 0;

        // read the file line by line
        while (reader.readLine() != null) {
            currentLine = reader.readLine();
            lineLength = currentLine.length();

            // read the line characters
            while (currentLineCharacter == lineLength || currentLineCharacter < lineLength) {
                char c = advance();
                switch (c) {
                    case '!':
                        if(peek() == '='){
                            advance();
                            nodesList.add(new Node(
                                    currentLineCharacter,
                                    currentLineCharacter,
                                    lineNumber,
                                    lineNumber,
                                    NodeType.NotEqualToNode,
                                    "!="
                            ));
                        }else{
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
                    case '=':
                        if(peek() == '='){
                            advance();
                            nodesList.add(new Node(
                                    currentLineCharacter,
                                    currentLineCharacter,
                                    lineNumber,
                                    lineNumber,
                                    NodeType.EquateNode,
                                    "=="
                            ));
                        }else{
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

                }

            }
            lineNumber++;
            currentLineCharacter=0;
            lineLength=0;
            currentLine="";
        }
        return nodesList;
    }

    private char advance() {
        currentLineCharacter++;
        return currentLine.charAt(currentLineCharacter - 1);
    }
    private char peek(){
        return currentLine.charAt(currentLineCharacter + 1);
    }


}
