package com.stratos.model;

import java.util.List;

public class TokenTree {
    List<Node> nodes;
    TokenTree nodesChildren;

    public TokenTree(List<Node> nodes,TokenTree nodesChildren) {
        this.nodes = nodes;
        this.nodesChildren = nodesChildren;
    }

    public List<Node> getNodes() {
        return nodes;
    }

    public void setNodes(List<Node> nodes) {
        this.nodes = nodes;
    }

    public TokenTree getNodesChildren() {
        return nodesChildren;
    }

    public void setNodesChildren(TokenTree nodesChildren) {
        this.nodesChildren = nodesChildren;
    }

    @Override
    public String toString() {
        return "TokenTree{" +
                "nodes=" + nodes +
                ", nodesChildren=" + nodesChildren +
                '}';
    }
}
