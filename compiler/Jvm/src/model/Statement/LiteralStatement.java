package model.Statement;

import model.StatementType;
import model.VariableType;

public class LiteralStatement  extends Statement{

    String value;
    VariableType type;

    public LiteralStatement(StatementType statementType, String value, VariableType type) {
        super(statementType);
        this.value = value;
        this.type = type;
    }

    public String getValue() {
        return value;
    }

    public void setValue(String value) {
        this.value = value;
    }

    public VariableType getType() {
        return type;
    }

    public void setType(VariableType type) {
        this.type = type;
    }
}
