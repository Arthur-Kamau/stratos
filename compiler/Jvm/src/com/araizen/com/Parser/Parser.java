package com.araizen.com.Parser;

import com.araizen.com.model.Node;
import com.araizen.com.model.NodeType;
import com.araizen.com.model.Token;

import java.util.ArrayList;
import java.util.List;

public class Parser {
    List<Token> tokensTree = new ArrayList<>();
    List<Node> nodes;
    private int current = 0;

    List<Token> parse(List<Node> nodes) {
        this.nodes = nodes;
        while (!isAtEnd()) {


        }
        return tokensTree;
    }



    private Node advance() {
        if (!isAtEnd()) current++;
        return previous();
    }

    private Node peek() {
        return nodes.get(current);
    }


    private Node previous() {
        return nodes.get(current - 1);
    }

    private boolean isAtEnd() {
        return peek().getType() == NodeType.EndOfFileNode;
    }


}
