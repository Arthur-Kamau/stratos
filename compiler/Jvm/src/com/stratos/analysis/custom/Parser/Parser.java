package com.stratos.analysis.custom.Parser;

import com.stratos.model.*;
import com.stratos.util.node.NodeUtil;

import java.util.ArrayList;
import java.util.List;

public class Parser {

    List<List<Node>> nodeGroup = new ArrayList();
    List<Node> nodes;

    List<Diagnostics> diagnostics = new ArrayList<>();
    private int current = 0;

    public NodeList parse(List<Node> nodes) {
        this.nodes = nodes;
        System.out.println("start parsing ");

        while (!isAtEnd()) {
            Node n = advance();
            createTokenTree(n);
        }



        return new NodeList(
                nodeGroup
        );
    }


   private void createTokenTree(Node n) {

        List<Node> nodeGroupItem = new ArrayList();
        if (n.getType() == NodeType.SemiColonNode || n.getType() == NodeType.NewLineNode || n.getType() == NodeType.EndOfFileNode) {
            if (!nodeGroupItem.isEmpty()) {
                nodeGroup.add(nodeGroupItem);
                nodeGroupItem.clear();
            }
        } else if (n.getType() == NodeType.CurlyBracketOpenNode) {
            System.out.println("\n Curly  token tree unhandled \n ");
            int stepsToClosingCurlyNode = new NodeUtil().findClosingCurlyBrace(nodes.subList(nodes.indexOf(n), nodes.size()));
            int closingNode = stepsToClosingCurlyNode +current;
//            System.out.println("steps "+stepsToClosingCurlyNode+" closing node "+closingNode + " current " + current);
            nodeGroupItem.add(n);
            if (0 == closingNode) {
                diagnostics.add(new Diagnostics("Unable to find closing Curly brace", SeverityLevel.Critical, n.getNodeLineStart(), n.getNodeLineEnd(), n.getNodeStart(), getLastNode().getNodeEnd()));
            } else {

                for (int i = 0; i <stepsToClosingCurlyNode ; i++) {
                    nodeGroupItem.add(advance());
                }

            }

        } else {
            nodeGroupItem.clear();
            nodeGroupItem.add(n);
            while (currentNode().getType() != NodeType.SemiColonNode && currentNode().getType() != NodeType.NewLineNode && currentNode().getType() != NodeType.EndOfFileNode) {
                Node n2 = advance();
                if (n2.getType() == NodeType.CurlyBracketOpenNode) {

                    int stepsToClosingCurlyNode = new NodeUtil().findClosingCurlyBrace(nodes.subList(nodes.indexOf(n2), nodes.size()));
                    int closingNode = stepsToClosingCurlyNode +current;
//                    System.out.println("steps "+stepsToClosingCurlyNode+" closing node "+closingNode + " current " + current);
                    nodeGroupItem.add(n2);
                    if (0 == closingNode) {
                        diagnostics.add(new Diagnostics("Unable to find closing Curly brace", SeverityLevel.Critical, n.getNodeLineStart(), n.getNodeLineEnd(), n.getNodeStart(), getLastNode().getNodeEnd()));
                    } else {

                        for (int i = 0; i <stepsToClosingCurlyNode ; i++) {
                            nodeGroupItem.add(advance());
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
        boolean r = currentNode().getType() == NodeType.EndOfFileNode || current+1 == nodes.size();

        return r;
    }


    private Node getLastNode() {
        return nodes.get(nodes.size() - 1);
    }
}
