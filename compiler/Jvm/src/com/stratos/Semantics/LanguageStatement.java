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

    public List<Statement> analysis(List<Token> tokens) throws Exception {
        for (Token item : tokens) {

            List<Statement> statement = new LanguageStatementAnalysis().nodesAnalysis(item);
            statementList.addAll(statement);
        }
        return statementList;
    }

    private class LanguageStatementAnalysis {

        List<Statement> statements = new ArrayList<>();
        List<Node> node;

        public List<Statement> nodesAnalysis(Token token) throws Exception {

            if (token.getNodes() != null) {
                node = token.getNodes();
                if (contains(NodeType.PackageNode)) {
                    statements.add(new PackageSemantics().execute(token));
                } else if (contains(NodeType.FunctionNode)) {
                    throw new Exception("unimplemented ....... ");
                } else if (contains(NodeType.ClassNode)) {
                    throw new Exception("unimplemented ....... ");
                } else if (contains(NodeType.ModulasNode) || contains(NodeType.DivideNode) || contains(NodeType.SubtractNode) || contains(NodeType.AddNode) || contains(NodeType.MultiplyNode)) {
                    statements.add(new OperationSemantics().execute(token));
                }
            } else {
                Log.error("Token contains empty nodes");
            }
            return statements;
        }


        protected boolean contains(NodeType type) {
            boolean containsNode = false;
            for (Node item : node) {
                if (item.getType() == type) {
                    containsNode = true;
                    break;
                }
            }
            return containsNode;
        }


    }
}
