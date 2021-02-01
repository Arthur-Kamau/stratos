package model;

import model.Statement.Statement;

import java.util.List;

public class ExecutionTree {

    String filePath;
    boolean isStdLib;
    boolean isPackage;
    List<Statement> statements;

    public ExecutionTree(String filePath, boolean isStdLib, boolean isPackage, List<Statement> statements) {
        this.filePath = filePath;
        this.isStdLib = isStdLib;
        this.isPackage = isPackage;
        this.statements = statements;
    }


    public String getFilePath() {
        return filePath;
    }

    public void setFilePath(String filePath) {
        this.filePath = filePath;
    }

    public boolean isStdLib() {
        return isStdLib;
    }

    public void setStdLib(boolean stdLib) {
        isStdLib = stdLib;
    }

    public boolean isPackage() {
        return isPackage;
    }

    public void setPackage(boolean aPackage) {
        isPackage = aPackage;
    }

    public List<Statement> getStatements() {
        return statements;
    }

    public void setStatements(List<Statement> statements) {
        this.statements = statements;
    }
}
