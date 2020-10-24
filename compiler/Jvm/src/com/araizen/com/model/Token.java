package com.araizen.com.model;

import java.util.ArrayList;
import java.util.List;

public class Token {
    List<List<Node>> nodesGroup;
    List<Token> children;

    @Override
    public String toString() {
        return "Token{" +
                "nodesGroup=" + nodesGroup +
                ", children=" + children +
                '}';
    }

//    public Token() {
//        this.nodesGroup = new ArrayList<>();
//        this.children = new ArrayList<>();
//    }

    public Token(List<List<Node>> nodesGroup, List<Token> children) {
        this.nodesGroup = nodesGroup;
        this.children = children;
    }

    public List<List<Node>> getnodesGroup() {
        return nodesGroup;
    }

    public void setnodesGroup(List<List<Node>> nodesGroup) {
        this.nodesGroup = nodesGroup;
    }

    public List<Token> getChildren() {
        return children;
    }

    public void setChildren(List<Token> children) {
        this.children = children;
    }
}
