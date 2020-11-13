package old.stratos.Syntax

import old.stratos.model.Node
import old.stratos.model.NodeType
import old.stratos.model.Statement.OperationStatement
import old.stratos.model.Statement.Statement
import old.stratos.model.Token
import old.stratos.util.node.NodeUtil
import java.util.*

class OperationSyntax : Syntax() {
    @Throws(Exception::class)
    fun execute(token: Token): Statement? {
        super.execute(token.nodes)
        val operation = containsMoreThanOneOperation(token.nodes)
        return when {
            operation == 1 -> {
                simpleExpression()
            }
            operation > 1 -> {
//            while (!isAtEnd) {
//                parseExpression()
//            }
//            null

                throw Exception("Operation not found")
            }
            else -> {
                throw Exception("Operation not found")
            }
        }
    }

    @Throws(Exception::class)
    private fun simpleExpression(): OperationStatement? {
        val node = advance()
        return if (node.type == NodeType.NumericNode) {
            if (peek().type == NodeType.AddNode || peek().type == NodeType.SubtractNode || peek().type == NodeType.ModulasNode || peek().type == NodeType.MultiplyNode) {
                val operation = advance()
                val node2 = advance()
                return OperationStatement(
                        leftValue = node,
                        rightValue = node2,
                        operationValue = operation,
                        returnValue = null,
                        complexStatement = false,
                        operationStatementList = emptyList()
                );

            } else {
                throw Exception("nopes")
            }
        } else {
            null
        }
    }

    @Throws(Exception::class)
    fun parseExpression(): List<OperationStatement>? {
        val operationStatementList: MutableList<OperationStatement> = mutableListOf()
        val node = advance()
        if (node.type == NodeType.NumericNode) {
            if (peek().type == NodeType.AddNode || peek().type == NodeType.SubtractNode || peek().type == NodeType.ModulasNode || peek().type == NodeType.MultiplyNode) {
                val operation = advance()
                val node2 = advance()
                operationStatementList.add(OperationStatement(
                        returnValue = null,
                        leftValue = node, operationValue = operation, rightValue = node2

                ));
            } else if (node.type == NodeType.CurvedBracketOpenNode) {
                val closingurlyBrackets = NodeUtil().findClosingCurlyBrace(nodes.subList(nodes.indexOf(node) + 1, nodes.size))
                // zero based plus number of steps
                val pos = nodes.indexOf(node) + 1 + closingurlyBrackets
                val itemsSublist: List<Node> = nodes.subList(current, pos)
                val operation = containsMoreThanOneOperation(itemsSublist)
                return when {
                    operation == 1 -> {

                        operationStatementList.add(
                                OperationStatement(
                                        returnValue = Node(
                                                0, 0, 0, 0, NodeType.AlphaNumericNode, "", isTemp = true
                                        ),
                                        leftValue = advance(),
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
        return operationStatementList
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

    private inner class parseComplexoperation : Syntax() {
        var operationStatementList: List<OperationStatement> = ArrayList()
        fun parse(items: List<Node?>?): List<OperationStatement>? {
            super.execute(items)
            return null
        }
    }
}