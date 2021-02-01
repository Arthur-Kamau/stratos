package model.Statement;

import model.Node;
import model.StatementType;

public class PackageStatement extends  Statement{
    String name;


    public PackageStatement(StatementType statementType, String name) {
        super(statementType);
        this.name = name;
    }

    public String getName() {
        return name;
    }

    public void setName(String name) {
        this.name = name;
    }
}
