package model.Statement;

import model.Node;
import model.Statement.ParameterStatement;

import java.util.List;

public class FunctionStatement extends  Statement{
    Node name;
    List<ParameterStatement> parameterStatementList;
    boolean isPrivate;
}
