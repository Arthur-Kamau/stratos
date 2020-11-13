package com.stratos.old.Statement

import com.stratos.model.Node

// math expression ie
// add, minus, divide, subtract , modulas.
// presendence BODMAS (brackats, Of , division , multiplication , addition , subtraction
class OperationStatement : Statement {
    var returnValue: Node?
    var leftValue: Node
    var operationValue: Node
    var rightValue: Node
    var isComplexStatement = false
    var operationStatementList: List<OperationStatement>? = null

    constructor(returnValue: Node?, leftValue: Node, operationValue: Node, rightValue: Node) {
        this.returnValue = returnValue
        this.leftValue = leftValue
        this.operationValue = operationValue
        this.rightValue = rightValue
    }

    constructor(returnValue: Node?, leftValue: Node, operationValue: Node, rightValue: Node, complexStatement: Boolean, operationStatementList: List<OperationStatement>?) {
        this.returnValue = returnValue
        this.leftValue = leftValue
        this.operationValue = operationValue
        this.rightValue = rightValue
        isComplexStatement = complexStatement
        this.operationStatementList = operationStatementList
    }

}