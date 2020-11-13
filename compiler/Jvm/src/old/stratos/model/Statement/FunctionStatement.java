package old.stratos.model.Statement;

import old.stratos.model.Node;

import java.util.List;

public class FunctionStatement extends  Statement{
    Node name;
    List<ParameterStatement> parameterStatementList;
    boolean isPrivate;
}
