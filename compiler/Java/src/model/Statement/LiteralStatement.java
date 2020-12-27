package model.Statement;

import model.StatementType;

public class LiteralStatement  extends Statement{

    String value;

    public LiteralStatement(String value, StatementType statementType) {
        super(statementType);
        this.value = value;
    }

    public String getValue() {
        return value;
    }

    public void setValue(String value) {
        this.value = value;
    }
}
