package com.araizen.com.Analysis;

import com.araizen.com.model.Node;
import com.araizen.com.model.NodeType;
import com.araizen.com.model.Statement.Statement;
import com.araizen.com.model.Token;

import java.util.ArrayList;
import java.util.List;

public class SemanticAnalysis {
    List<Statement> statementList = new ArrayList<>();

    public List<Statement> analysis(Token tokens) {
        for (List<Node> item : tokens.getnodesGroup()) {
            Statement statement = new nodeAnalysis().nodesAnalysis(item);
            statementList.add(statement);
        }
        return statementList;
    }

    private class nodeAnalysis {

        List<Node> nodes;
        int current = 0;

        public Statement nodesAnalysis(List<Node> nodes) {
            this.nodes = nodes;

            return new Statement();
        }

        private boolean check(NodeType type) {
            if (isAtEnd()) return false;
            return peek().getType() == type;
        }

        private Node consume(NodeType type, String message) {
            if (check(type)) return advance();

//            throw error(peek(), message);
        }

        private Node peek() {
            return nodes.get(current);
        }

        private Node advance() {
            if (!isAtEnd()) {

                current++;
            }
            return previous();
        }

        private Node previous() {
            return nodes.get(current - 1);
        }


        private boolean isAtEnd() {
            boolean r = currentNode().getType() == NodeType.EndOfFileNode;

            return r;
        }

        private Node currentNode() {
            return nodes.get(current);
        }

    }
}
