package com.araizen.com.model;

import java.util.List;

public class Token {
    List<Node> nodes;
    List<Token> children;

    public Token(List<Node> nodes, List<Token> children) {
        this.nodes = nodes;
        this.children = children;
    }

    public List<Node> getNodes() {
        return nodes;
    }

    public void setNodes(List<Node> nodes) {
        this.nodes = nodes;
    }

    public List<Token> getChildren() {
        return children;
    }

    public void setChildren(List<Token> children) {
        this.children = children;
    }
}
