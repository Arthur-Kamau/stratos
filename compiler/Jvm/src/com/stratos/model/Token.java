package com.stratos.model;

import java.util.List;

public class Token {
    List<Node> nodes;
    List<Token> child;

    public Token(List<Node> nodes, List<Token> child) {
        this.nodes = nodes;
        this.child = child;
    }

    public List<Node> getNodes() {
        return nodes;
    }

    public void setNodes(List<Node> nodes) {
        this.nodes = nodes;
    }

    public List<Token> getChild() {
        return child;
    }

    public void setChild(List<Token> child) {
        this.child = child;
    }

    @Override
    public String toString() {
        return "Token{" +
                "nodes=" + nodes +
                ", child=" + child +
                '}';
    }
}
