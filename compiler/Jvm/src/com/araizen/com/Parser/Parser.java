package com.araizen.com.Parser;

import com.araizen.com.model.*;

import java.util.ArrayList;
import java.util.List;

public class Parser {

    List<Token> children = new ArrayList<Token>();
    List<List<Node>> nodeGroup = new ArrayList();
    List<Node> nodes;

    List<Diagnostics> diagnostics = new ArrayList<>();
    private int current = 0;

    public Token parse(List<Node> nodes) {
        this.nodes = nodes;
        System.out.println("start parsing ");
        while (!isAtEnd()) {
            Node n = advance();
            createTokenTree(n);
        }


        return new Token(
                nodeGroup,
                children
        );
    }


    void createTokenTree(Node n) {

        List<Node> nodeGroupItem = new ArrayList();
        if (n.getType() == NodeType.SemiColonNode || n.getType() == NodeType.NewLineNode || n.getType() == NodeType.EndOfFileNode) {
            if (!nodeGroupItem.isEmpty()) {
                nodeGroup.add(nodeGroupItem);
                nodeGroupItem.clear();
            }
        } else if (n.getType() == NodeType.CurlyBracketOpenNode) {
            System.out.println("\n Curly \n ");
            int closingNode = findClosingCurlyBrace(nodes.subList(current, nodes.size()));
            if (0 == closingNode) {
                diagnostics.add(new Diagnostics("Unable to find closing Curly brace", SeverityLevel.Critical, n.getNodeLineStart(), n.getNodeLineEnd(), n.getNodeStart(), getLastNode().getNodeEnd()));
            } else {
                List<Node> subListNodes = nodes.subList(current, closingNode);
                for (Node nx : subListNodes
                ) {
                    System.out.println("item " + nx);
                }
            }

        } else {
            nodeGroupItem.clear();
            nodeGroupItem.add(n);
            while (currentNode().getType() != NodeType.SemiColonNode && currentNode().getType() != NodeType.NewLineNode && currentNode().getType() != NodeType.EndOfFileNode) {
                Node n2 = advance();
                if (n2.getType() == NodeType.CurlyBracketOpenNode) {
                    System.out.println("\n Curly \n ");
                    int closingNode = findClosingCurlyBrace(nodes.subList(current, nodes.size()));
                    if (0 == closingNode) {
                        diagnostics.add(new Diagnostics("Unable to find closing Curly brace", SeverityLevel.Critical, n.getNodeLineStart(), n.getNodeLineEnd(), n.getNodeStart(), getLastNode().getNodeEnd()));
                    } else {
                        List<Node> subListNodes = nodes.subList(current, closingNode);
                        for (Node nx : subListNodes
                        ) {
                            System.out.println("item " + nx);
                        }
                    }
                } else {
                    nodeGroupItem.add(n2);
                }
            }

            if (!nodeGroupItem.isEmpty()) {
                nodeGroup.add(nodeGroupItem);

            }
        }

    }


    List<Token> createChildTokens(List<Node> subListNodes) {
        List<Token> token = new ArrayList<Token>();
        System.out.println("sub list nodes ");

        for (Node n : subListNodes
        ) {
            System.out.println("item " + n);
        }
        return token;
    }


//    Token createChildTokens(List<Node> nodeList) {
//        Token token = new Token();
//        int _currentItem =0;
//        for (int i = 0; i < nodeList.size() ; i++) {
//
//        }
//        while (_currentItem > nodeList.size()){
//            Node n = nodeList.get(_currentItem);
//            if (n.getType() == NodeType.CurlyBracketOpenNode) {

//            } else {
//                List<Node> tokenNodes = token.getNodes();
//                tokenNodes.add(n);
//                token.setNodes(tokenNodes);
//            }
//            _currentItem++;
//        }
////        for (int i = 0; i < nodeList.size(); i++) {
//
////        }
//
//
//        return token;
//    }

    public List<Diagnostics> getDiagnostics() {
        return diagnostics;
    }

    private Node advance() {
        if (!isAtEnd()) {

            current++;
        }
        return previous();
    }

    private Node currentNode() {
        return nodes.get(current);
    }

    private Node previous() {
        return nodes.get(current - 1);
    }

    private boolean isAtEnd() {
        boolean r = currentNode().getType() == NodeType.EndOfFileNode;

        return r;
    }


    private int findClosingCurlyBrace(List<Node> nodes) {
        int closingCurlyBracePosition = 0;
        int _counter = 1;
        while (_counter > 0) {
            Node node = advance();
            if (node.getType() == NodeType.CurlyBracketOpenNode) {
                _counter += 1;
            } else if (node.getType() == NodeType.CurlyBracketCloseNode) {
                _counter -= 1;
            } else if (node.getType() == NodeType.EndOfFileNode) {
                System.out.println("Unexpected end of file node");
                break;
            } else {
                System.out.println("ignore node " + node.toString());
            }
        }
        return closingCurlyBracePosition;
    }

    private int findClosingCurlyBrace() {
        int closingCurlyBracePosition = current;
        int counter = 1;
        while (counter > 0) {
            Node node = advance();
            if (node.getType() == NodeType.CurlyBracketOpenNode) {
                counter += 1;
            } else if (node.getType() == NodeType.CurlyBracketCloseNode) {
                counter -= 1;
            } else if (node.getType() == NodeType.EndOfFileNode) {
                System.out.println("Unexpected end of file node");
                break;
            } else {
                System.out.println("ignore node " + node.toString());
            }
        }
        return closingCurlyBracePosition;
    }

    private Node getLastNode() {
        return nodes.get(nodes.size() - 1);
    }
}
