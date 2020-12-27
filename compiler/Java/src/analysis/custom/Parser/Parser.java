package analysis.custom.Parser;

//import old.stratos.util.node.NodeUtil;

import model.*;
import model.Statement.*;

import java.util.ArrayList;
import java.util.List;

public class Parser {

    List<Statement> statementList = new ArrayList();
    List<Node> nodes;

    List<Diagnostics> diagnostics = new ArrayList<>();
    private int current = 0;

    public List<Statement> parse(List<Node> nodes) throws Exception {
        this.nodes = nodes;


        while (!isAtEnd()) {
            Node n = advance();
            evaluateStatements(n);
        }


        return statementList;
    }


    private void evaluateStatements(Node n) throws Exception {

        if (n.getType() == NodeType.LineCommentNode || n.getType() == NodeType.MultiLineCommentNode || n.getType() == NodeType.NewLineNode) {
            System.out.println("ignore node " + n.toString());
        } else if (n.getType() == NodeType.ImportNode) {
            String path = "";
            Node nxt = advance();

            if (nxt.getType() == NodeType.AlphaNumericNode) {
            path+=nxt.getValue();
            while (!isAtEnd()){
                Node nxtScopeOrNot = advance();
                if(nxtScopeOrNot.getType() == NodeType.SemiColonNode || nxtScopeOrNot.getType() == NodeType.NewLineNode ){
                    break;
                }else{
                   if(nxtScopeOrNot.getType() == NodeType.AlphaNumericNode ||nxtScopeOrNot.getType() == NodeType.DotNode  ){
                       path+=nxtScopeOrNot.getValue();
                   }else{
                       System.out.println("Error expected . or name in import path but got "+nxtScopeOrNot.getValue() );

                   }
                }
            }

                statementList.add(new ImportStatement(StatementType.ImportStatement, path));
            }else{
                System.out.println("Expected import name but got "+ n.toString() );
            }
        } else if (n.getType() == NodeType.PackageNode) {

            Node nxt = advance();
            if (nxt.getType() == NodeType.AlphaNumericNode) {

                statementList.add(new PackageStatement(StatementType.PackageStatement, nxt.getValue()));
                if (peek() != null) {
                    if (peek().getType() == NodeType.SemiColonNode) {
                        // consume new line and semi colon
                        advance();
                        advance();
                    } else if (peek().getType() == NodeType.NewLineNode) {
                        // consume new
                        advance();
                    } else {
                        System.out.println("Continue execution");
                    }
                }
            } else {
                System.out.println("Expected package name");
            }


        } else if (n.getType() == NodeType.ValNode || n.getType() == NodeType.VarNode || n.getType() == NodeType.LetNode) {

            Node nxt = advance();
            if (nxt.getType() != NodeType.AlphaNumericNode && nxt.getType() != NodeType.NumericNode) {
                System.out.println("Expected integer or variable name ");
                throw new Exception("Invalid statement");
            }
            if (nxt.getType() == NodeType.SemiColonNode || nxt.getType() == NodeType.NewLineNode) {

                statementList.add(new VariableStatement(n.getValue(), nxt.getValue(), VariableType.unknownType, null, StatementType.VariableDeclaration));
            } else {

                Node assignOrColon = advance();
                if (assignOrColon.getType() == NodeType.AssignNode) {
                    statementList.add(new VariableStatement(n.getValue(), nxt.getValue(), VariableType.objectType, evaluateExpression(), StatementType.VariableDeclaration));

                } else if (assignOrColon.getType() == NodeType.ColonNode) {

                    Node type = advance();
                    VariableType variableType = getVariableType(type);
                    Node assign = advance();
                    if (assign.getType() == NodeType.AssignNode) {

                        statementList.add(new VariableStatement(n.getValue(), nxt.getValue(), variableType, evaluateExpression(), StatementType.VariableDeclaration));
                    } else {
                        System.out.println("Expected Assignment  node but got " + assign.toString());
                    }


                } else {

                    System.out.println("Expected Assignment or Colon node but got " + assignOrColon.toString());
                }
            }

        } else {
            System.out.println("Node parse not captured" + n.toString());
        }

    }

    private VariableType getVariableType(Node n) {

        if (n.getType() == NodeType.StringNode) {
            return VariableType.stringType;
        } else if (n.getType() == NodeType.DoubleNode) {
            return VariableType.integerType;
        } else if (n.getType() == NodeType.DoubleNode) {
            return VariableType.doubleType;
        } else {
            return VariableType.customType;
        }
    }

    public Statement evaluateExpression() throws Exception {

        List<Node> nodeList = new ArrayList<Node>();
        while (!isAtEnd()) {

            Node curr = advance();
            System.out.println("item " + curr.toString());

            if (curr.getType() == NodeType.NewLineNode || curr.getType() == NodeType.SemiColonNode || curr.getType() == NodeType.EndOfFileNode) {
                break;
            } else {
                System.out.println("Add ");
                nodeList.add(curr);
            }

        }

        if (nodeList.size() == 1) {
            Node n = nodeList.get(0);
            if (n.getType() == NodeType.NumericNode || n.getType() == NodeType.AlphaNumericNode || n.getType() == NodeType.StringValue) {
                VariableType type;
                if (n.getType() == NodeType.NumericNode) {
                    type = VariableType.integerType;
                } else if (n.getType() == NodeType.AlphaNumericNode) {
                    type = VariableType.customType;
                } else if (n.getType() == NodeType.StringValue) {
                    type = VariableType.stringType;
                } else {
                    type = VariableType.customType;
                }

                return new LiteralStatement(StatementType.LiteralStatement, n.getValue(), type);
            } else {
                System.out.println("Unexpected node " + n.toString());
            }

        } else {

            List<Statement> statementList = new Parser().parse(nodeList);

            System.out.println("Statements in expression  " + statementList.toString());
            if (statementList.size() > 1) {
                throw new Exception("Expected on e expression ");
            } else {
                return statementList.get(0);
            }

        }


        return null;
    }


    private boolean contains(List<Node> nodeList, NodeType type) {
        boolean exist = false;
        for (Node i : nodeList) {
            if (i.getType() == type) {
                exist = true;
                break;
            }

        }
        return exist;
    }


    public List<Diagnostics> getDiagnostics() {
        return diagnostics;
    }

    private Node peek() {
        if (!isAtEnd()) {
            int _tmp = current;
            return nodes.get(_tmp++);
        } else {
            return null;
        }
    }

    private Node advance() {
        if (!isAtEnd()) {

            current++;
        }
        return previous();
    }

    private Node currentNode() {
        return nodes.get(current);
    }

    private Node previous() {
        return nodes.get(current - 1);
    }

    private boolean isAtEnd() {
        boolean r = currentNode().getType() == NodeType.EndOfFileNode || current + 1 == nodes.size();

        return r;
    }


    private Node getLastNode() {
        return nodes.get(nodes.size() - 1);
    }
}