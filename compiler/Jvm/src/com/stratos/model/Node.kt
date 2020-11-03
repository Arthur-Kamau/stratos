package com.stratos.model

class Node(var nodeStart: Int, var nodeEnd: Int, var nodeLineStart: Int, var nodeLineEnd: Int, var type: NodeType, var value: String, var isTemp: Boolean = false) {

    override fun toString(): String {
        return "Node{" +
                "nodeStart=" + nodeStart +
                ", nodeEnd=" + nodeEnd +
                ", nodeLineStart=" + nodeLineStart +
                ", nodeLineEnd=" + nodeLineEnd +
                ", type=" + type +
                ", value='" + value + '\'' +
                '}'
    }

}