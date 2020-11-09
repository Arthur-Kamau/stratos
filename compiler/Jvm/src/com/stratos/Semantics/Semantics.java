package com.stratos.Semantics;

import com.stratos.model.Node;
import com.stratos.model.NodeType;
import com.stratos.model.Token;

import java.util.ArrayList;
import java.util.List;

public class Semantics {
    List<Node> nodes = new ArrayList<>();

    int current = 0;

    public void execute( List<Node> nodes ){
     this.nodes.addAll(nodes);
    }

    protected boolean check(NodeType type) {
        if (isAtEnd()) return false;
        return peek().getType() == type;
    }


    protected Node peek() {
        return nodes.get(this.current);
    }

    protected Node advance() {

        if (!isAtEnd()) {

            this.current++;
            return previous();
        }else{
            return  peek();
        }

    }

    protected Node previous() {
        return nodes.get(this.current - 1);
    }


    protected boolean isAtEnd() {
        boolean r = current == nodes.size();

        return r;
    }

    protected boolean contains(NodeType type) {
        boolean containsNode = false;
        for (Node item : nodes) {
            if (item.getType() == type) {
                containsNode = true;
                break;
            }
        }
        return containsNode;
    }



}
