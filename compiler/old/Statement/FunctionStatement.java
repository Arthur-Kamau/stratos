package com.stratos.old.Statement;

import com.stratos.model.Node;

import java.util.List;

public class FunctionStatement extends  Statement{
    Node name;
    List<ParameterStatement> parameterStatementList;
    boolean isPrivate;
}
