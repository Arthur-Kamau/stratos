package com.stratos.Semantics;

import com.stratos.model.Token;

public class OperationSemantics extends Statement {

    public com.stratos.model.Statement.Statement execute(Token token) {
        super.execute(token.getNodes());

    }
}
