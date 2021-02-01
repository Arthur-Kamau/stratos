package model.Statement;

import model.StatementType;
import model.VariableType;

public class IdentifierStatement extends Statement{
    String name;

    public IdentifierStatement(String name, StatementType type) {
        super(type);
        this.name = name;

    }

    public String getName() {
        return name;
    }

    public void setName(String name) {
        this.name = name;
    }
}
