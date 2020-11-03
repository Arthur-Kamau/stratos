package com.stratos.model.Statement;

import com.stratos.model.Node;

public class VariableStatement {

    Node type;
    Node name;
    Boolean isStatic;
    Boolean isConstant;


    public VariableStatement(Node type, Node name, Boolean isStatic, Boolean isConstant) {
        this.type = type;
        this.name = name;
        this.isStatic = isStatic;
        this.isConstant = isConstant;
    }


    public Node getType() {
        return type;
    }

    public void setType(Node type) {
        this.type = type;
    }

    public Node getName() {
        return name;
    }

    public void setName(Node name) {
        this.name = name;
    }

    public Boolean getStatic() {
        return isStatic;
    }

    public void setStatic(Boolean aStatic) {
        isStatic = aStatic;
    }

    public Boolean getConstant() {
        return isConstant;
    }

    public void setConstant(Boolean constant) {
        isConstant = constant;
    }
}
