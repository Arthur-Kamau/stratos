package com.araizen.com.model;

public class Node {

    int nodeStart;
    int nodeEnd;
    int nodeLineStart;
    int nodeLineEnd;
    NodeType type;
    String value;

    public Node(int nodeStart, int nodeEnd, int nodeLineStart, int nodeLineEnd, NodeType type, String value) {
        this.nodeStart = nodeStart;
        this.nodeEnd = nodeEnd;
        this.nodeLineStart = nodeLineStart;
        this.nodeLineEnd = nodeLineEnd;
        this.type = type;
        this.value = value;
    }

    public int getNodeStart() {
        return nodeStart;
    }

    public void setNodeStart(int nodeStart) {
        this.nodeStart = nodeStart;
    }

    public int getNodeEnd() {
        return nodeEnd;
    }

    public void setNodeEnd(int nodeEnd) {
        this.nodeEnd = nodeEnd;
    }

    public int getNodeLineStart() {
        return nodeLineStart;
    }

    public void setNodeLineStart(int nodeLineStart) {
        this.nodeLineStart = nodeLineStart;
    }

    public int getNodeLineEnd() {
        return nodeLineEnd;
    }

    public void setNodeLineEnd(int nodeLineEnd) {
        this.nodeLineEnd = nodeLineEnd;
    }

    public NodeType getType() {
        return type;
    }

    public void setType(NodeType type) {
        this.type = type;
    }

    public String getValue() {
        return value;
    }

    public void setValue(String value) {
        this.value = value;
    }

    @Override
    public String toString() {
        return "Node{" +
                "nodeStart=" + nodeStart +
                ", nodeEnd=" + nodeEnd +
                ", nodeLineStart=" + nodeLineStart +
                ", nodeLineEnd=" + nodeLineEnd +
                ", type=" + type +
                ", value='" + value + '\'' +
                '}';
    }
}
