package model;

import model.Node;

import java.util.List;

public class NodeList {
    List<List<Node>> nodesGroup;


    @Override
    public String toString() {
        return "Token{" +
                "nodesGroup=" + nodesGroup +
                '}';
    }


    public NodeList(List<List<Node>> nodesGroup) {
        this.nodesGroup = nodesGroup;

    }

    public List<List<Node>> getnodesGroup() {
        return nodesGroup;
    }

    public void setnodesGroup(List<List<Node>> nodesGroup) {
        this.nodesGroup = nodesGroup;
    }

}
