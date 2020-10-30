package com.stratos.model.Statement;

import com.stratos.model.Node;

import java.util.List;

// math expression ie
// add, minus, divide, subtract , modulas.
// presendence BODMAS (brackats, Of , division , multiplication , addition , subtraction
public class OperationExpressionStatement {
    Node leftValue;
    Node operationValue;
    Node rightValue;

    boolean complexStatement;
    List<OperationExpressionStatement> operationExpressionStatementList;

    public boolean isComplexStatement() {
        return complexStatement;
    }

    public void setComplexStatement(boolean complexStatement) {
        this.complexStatement = complexStatement;
    }

    public List<OperationExpressionStatement> getOperationExpressionStatementList() {
        return operationExpressionStatementList;
    }

    public void setOperationExpressionStatementList(List<OperationExpressionStatement> operationExpressionStatementList) {
        this.operationExpressionStatementList = operationExpressionStatementList;
    }

    public Node getLeftValue() {
        return leftValue;
    }

    public void setLeftValue(Node leftValue) {
        this.leftValue = leftValue;
    }

    public Node getOperationValue() {
        return operationValue;
    }

    public void setOperationValue(Node operationValue) {
        this.operationValue = operationValue;
    }

    public Node getRightValue() {
        return rightValue;
    }

    public void setRightValue(Node rightValue) {
        this.rightValue = rightValue;
    }
}
