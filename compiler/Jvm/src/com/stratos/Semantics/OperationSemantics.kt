package com.stratos.Semantics

import com.stratos.model.Node
import com.stratos.model.NodeType
import com.stratos.model.Statement.OperationExpressionStatement
import com.stratos.model.Token
import com.stratos.util.node.NodeUtil
import java.util.*

class OperationSemantics : Statement() {
    @Throws(Exception::class)
    fun execute(token: Token): com.stratos.model.Statement.Statement? {
        super.execute(token.nodes)
        val operation = containsMoreThanOneOperation(token.nodes)
        println(" operation $operation")
        return if (operation == 1) {
            simpleExpression()
        } else if (operation > 1) {
            while (!isAtEnd) {
                parseExpression()
            }
            null
        } else {
            throw Exception("Operation not found")
        }
    }

    @Throws(Exception::class)
    private fun simpleExpression(): OperationExpressionStatement? {
        val node = advance()
        return if (node.type == NodeType.NumericNode) {
            if (peek().type == NodeType.AddNode || peek().type == NodeType.SubtractNode || peek().type == NodeType.ModulasNode || peek().type == NodeType.MultiplyNode) {
                val operation = advance()
                val node2 = advance()
                //                return new  OperationExpressionStatement(
//                        node,
//                        operation,
//                        node2
//                );
                null
            } else {
                throw Exception("nopes")
            }
        } else {
            null
        }
    }

    @Throws(Exception::class)
    fun parseExpression(): List<OperationExpressionStatement>? {
        val operationExpressionStatementList: MutableList<OperationExpressionStatement> = mutableListOf()
        val node = advance()
        if (node.type == NodeType.NumericNode) {
            if (peek().type == NodeType.AddNode || peek().type == NodeType.SubtractNode || peek().type == NodeType.ModulasNode || peek().type == NodeType.MultiplyNode) {
                val operation = advance()
                val node2 = advance()
                                operationExpressionStatementList.add( OperationExpressionStatement(
                                        returnValue = null,
                                        leftValue = node, operationValue = operation , rightValue = node2

                ));
            } else if (node.type == NodeType.CurvedBracketOpenNode) {
                val closingurlyBrackets = NodeUtil().findClosingCurlyBrace(nodes.subList(nodes.indexOf(node) + 1, nodes.size))
                // zero based plus number of steps
                val pos = nodes.indexOf(node) + 1 + closingurlyBrackets
                val itemsSublist: List<Node> = nodes.subList(current, pos)
                val operation = containsMoreThanOneOperation(itemsSublist)
                return when {
                    operation == 1 -> {

                            operationExpressionStatementList.add(
                                     OperationExpressionStatement(
                                      returnValue = Node(
                                              0,0,0,0,NodeType.AlphaNumericNode, "" , isTemp = true
                                      ),
                                              leftValue =advance(),
                                             operationValue = advance(),
                                             rightValue = advance()


                                    )
                            );
                        null
                    }
                    operation > 1 -> {
                        null
                    }
                    else -> {
                        throw Exception("Operation not found")
                    }
                }
            } else {
                throw Exception("nopes")
            }
        }
        return operationExpressionStatementList
    }

    private fun containsMoreThanOneOperation(nodeItems: List<Node>): Int {
        var foundOperation = 0
        for (node in nodeItems) {
            if (node.type == NodeType.AddNode || node.type == NodeType.SubtractNode || node.type == NodeType.ModulasNode || node.type == NodeType.MultiplyNode) {
                foundOperation++
            }
        }
        return foundOperation
    }

    private inner class parseComplexoperation : Statement() {
        var operationExpressionStatementList: List<OperationExpressionStatement> = ArrayList()
        fun parse(items: List<Node?>?): List<OperationExpressionStatement>? {
            super.execute(items)
            return null
        }
    }
}