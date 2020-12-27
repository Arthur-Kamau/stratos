package model.Statement;

import model.StatementType;

public class ImportStatement extends  Statement{
    String importPath;

    public ImportStatement(StatementType statementType, String importPath) {
        super(statementType);
        this.importPath = importPath;
    }

    public String getImportPath() {
        return importPath;
    }

    public void setImportPath(String importPath) {
        this.importPath = importPath;
    }
}
