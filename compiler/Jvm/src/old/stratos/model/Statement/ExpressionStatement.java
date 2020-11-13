package old.stratos.model.Statement;

import old.stratos.model.Node;

import java.util.List;

public class ExpressionStatement extends Statement {
    Node leftNode;
    Node ExpressionNode;
    Node rightNode ;

    Node LogicalOperator;
    boolean isComplexExpression;



    List<ExpressionStatement>  complexExpression ;


    public ExpressionStatement() {
    }


    @Override
    public String toString() {
        return "ExpressionStatement{" +
                "leftNode=" + leftNode +
                ", ExpressionNode=" + ExpressionNode +
                ", rightNode=" + rightNode +
                ", LogicalOperator=" + LogicalOperator +
                ", isComplexExpression=" + isComplexExpression +
                ", complexExpression=" + complexExpression +
                '}';
    }

    public ExpressionStatement(Node leftNode, Node expressionNode, Node rightNode, Node logicalOperator, boolean isComplexExpression, List<ExpressionStatement> complexExpression) {
        this.leftNode = leftNode;
        ExpressionNode = expressionNode;
        this.rightNode = rightNode;
        LogicalOperator = logicalOperator;
        this.isComplexExpression = isComplexExpression;
        this.complexExpression = complexExpression;
    }

    public boolean isComplexExpression() {
        return isComplexExpression;
    }

    public void setComplexExpression(boolean complexExpression) {
        isComplexExpression = complexExpression;
    }

    public List<ExpressionStatement> getComplexExpression() {
        return complexExpression;
    }

    public void setComplexExpression(List<ExpressionStatement> complexExpression) {
        this.complexExpression = complexExpression;
    }

    public Node getLeftNode() {
        return leftNode;
    }

    public void setLeftNode(Node leftNode) {
        this.leftNode = leftNode;
    }

    public Node getExpressionNode() {
        return ExpressionNode;
    }

    public void setExpressionNode(Node expressionNode) {
        ExpressionNode = expressionNode;
    }

    public Node getRightNode() {
        return rightNode;
    }

    public void setRightNode(Node rightNode) {
        this.rightNode = rightNode;
    }


    public Node getLogicalOperator() {
        return LogicalOperator;
    }

    public void setLogicalOperator(Node logicalOperator) {
        LogicalOperator = logicalOperator;
    }
}
