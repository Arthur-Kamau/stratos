package com.araizen.com.model;

import java.util.ArrayList;
import java.util.List;

public class Token {
    List<List<Node>> nodesGroup;


    @Override
    public String toString() {
        return "Token{" +
                "nodesGroup=" + nodesGroup +
                '}';
    }


    public Token(List<List<Node>> nodesGroup) {
        this.nodesGroup = nodesGroup;

    }

    public List<List<Node>> getnodesGroup() {
        return nodesGroup;
    }

    public void setnodesGroup(List<List<Node>> nodesGroup) {
        this.nodesGroup = nodesGroup;
    }

}
