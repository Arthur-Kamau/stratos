package com.stratos.Semantics;

import com.stratos.model.*;
import com.stratos.model.Statement.PackageStatement;
import com.stratos.model.Statement.Statement;
import com.stratos.util.Print.Log;

import java.util.ArrayList;
import java.util.List;

public class LanguageStatement {
    List<Statement> statementList = new ArrayList<>();
    List<Diagnostics> diagnostics = new ArrayList<>();

    public List<Statement> analysis(List<Token> tokens) {
        for (Token item : tokens) {

            List<Statement> statement = new LanguageStatementAnalysis().nodesAnalysis(item);
            statementList.addAll(statement);
        }
        return statementList;
    }

    private class LanguageStatementAnalysis {
        Token token;
        List<Node> nodes = new ArrayList<>();
        List<Statement> statements = new ArrayList<>();
        int current = 0;

        public List<Statement> nodesAnalysis(Token token) {
            this.token = token;
            nodes.addAll(token.getNodes());

            while (!isAtEnd()) {
                Node c = advance();
                if (c.getType() == NodeType.PackageNode) {

                    if (peek().getType() == NodeType.AlphaNumericNode || peek().getType() == NodeType.StringNode) {

                        Node packageName = advance();
                        statements.add(new PackageStatement(
                                packageName,
                                c
                        ));
                    } else {
                        Log.error("No alpanumeric " +peek().toString());
                    }

                }
            }

//            if (token.getChild() != null) {
//                List<Statement> items =  new LanguageStatementAnalysis().nodesAnalysis()
//            }
            return statements;
        }

        private boolean check(NodeType type) {
            if (isAtEnd()) return false;
            return peek().getType() == type;
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
            boolean r = current+1 == nodes.size();

            return r;
        }



    }
}
