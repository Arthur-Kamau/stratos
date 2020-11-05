package com.stratos.Semantics;

import com.stratos.model.Node;
import com.stratos.model.NodeType;
import com.stratos.model.Statement.ExpressionStatement;
import com.stratos.model.Statement.Statement;
import com.stratos.model.Token;

import java.util.ArrayList;
import java.util.List;

public class ExpressionSemantics extends Semantics {


    ExpressionStatement expressionStatement = new ExpressionStatement();

    public Statement execute(Token token) throws Exception {
        super.execute(token.getNodes());

        if (conainsMoreThanOneExpresion(token)) {

            List<ExpressionStatement> expressionStatementList = new ArrayList<>();



            while (!isAtEnd()){
                ExpressionStatement expressionStatement =  makeStatement();
                expressionStatementList.add(expressionStatement);
            }

            expressionStatement.setComplexExpression(expressionStatementList);
            expressionStatement.setComplexExpression(false);

        } else {


            while (!super.isAtEnd()) {
                Node c = super.advance();

                if (c.getType() == NodeType.StringValue || c.getType() == NodeType.NumericNode || c.getType() == NodeType.AlphaNumericNode
                        &&
                        peek().getType() == NodeType.NotEqualToNode || peek().getType() == NodeType.EquateNode ||
                        peek().getType() == NodeType.LessThan ||
                        peek().getType() == NodeType.LessThanOrEqualTo ||
                        peek().getType() == NodeType.GreaterThan ||
                        peek().getType() == NodeType.GreaterThanOrEqualTo) {
                    expressionStatement.setLeftNode(c);
                    expressionStatement.setExpressionNode(super.advance());
                    expressionStatement.setExpressionNode(super.advance());
                    expressionStatement.setChildren(new ArrayList<>());
                    expressionStatement.setComplexExpression(false);
                    expressionStatement.setExpressionNode(null);
                } else {

                    checkErrors(c);
                }

            }
        }
        return expressionStatement;
    }

    private  ExpressionStatement makeStatement() throws Exception {
        Node c = super.advance();
        ExpressionStatement expressionStatementTemp = new ExpressionStatement();
        if (c.getType() == NodeType.StringValue || c.getType() == NodeType.NumericNode || c.getType() == NodeType.AlphaNumericNode) {

            if (peek().getType() == NodeType.NotEqualToNode || peek().getType() == NodeType.EquateNode ||
                    peek().getType() == NodeType.LessThan ||
                    peek().getType() == NodeType.LessThanOrEqualTo ||
                    peek().getType() == NodeType.GreaterThan ||
                    peek().getType() == NodeType.GreaterThanOrEqualTo) {


                    Node ex = super.advance();
                    Node ri = super.advance();

                    System.out.println(" ri "+ ex.toString());

                    expressionStatementTemp.setLeftNode(c);
                    expressionStatementTemp.setExpressionNode(ex);
                    expressionStatementTemp.setRightNode(ri);
                    expressionStatementTemp.setChildren(new ArrayList<>());
                    expressionStatementTemp.setComplexExpression(false);
//
                    if(peek().getType() == NodeType.SemiColonNode || peek().getType() == NodeType.NewLineNode || peek().getType() == NodeType.AndAndNode  ||   peek().getType() == NodeType.OrNode  ||  peek().getType() == NodeType.AndOrNode ){

                        expressionStatementTemp.setLogicalOperator(advance());
                    }else {
                        throw  new Exception("Expecting  Or (||) ,  And (&&)  , AndOr(&!) , Newline or Semicolon \n Current object "+expressionStatementTemp.toString()+"  \n got  "+ peek().toString());
                    }
            }
        } else {

            checkErrors(c);
        }
        return expressionStatementTemp;
    }
    private void checkErrors(Node c) throws Exception {
        if (peek().getType() != NodeType.NotEqualToNode || peek().getType() != NodeType.EquateNode ||
                peek().getType() != NodeType.LessThan ||
                peek().getType() != NodeType.LessThanOrEqualTo ||
                peek().getType() != NodeType.GreaterThan ||
                peek().getType() != NodeType.GreaterThanOrEqualTo) {
            throw new Exception("error at  line " + peek().getNodeLineStart() + "  character   " + peek().getNodeStart() + " expected equal to(==),  greater than(>) , less than(<) , not equal to (!=)  but found " + peek().getValue());
        }

        if (c.getType() != NodeType.StringValue || c.getType() != NodeType.NumericNode || c.getType() != NodeType.AlphaNumericNode) {
            throw new Exception("error at  line " + peek().getNodeLineStart() + "  character   " + peek().getNodeStart() + " expected  variable reference , integer   or string got " + peek().getValue());
        }

    }

    public boolean conainsMoreThanOneExpresion(Token token) {
        int foundOperation = 0;
        for (Node node : token.getNodes()) {
            if (node.getType() == NodeType.NotEqualToNode || node.getType() == NodeType.EquateNode ||
                    node.getType() == NodeType.LessThan ||
                    node.getType() == NodeType.LessThanOrEqualTo ||
                    node.getType() == NodeType.GreaterThan ||
                    node.getType() == NodeType.GreaterThanOrEqualTo) {
                foundOperation++;
            }
        }
        if (foundOperation == 1) {
            return false;
        } else {
            return true;
        }
    }
}
