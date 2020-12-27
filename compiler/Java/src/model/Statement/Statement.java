package model.Statement;

import java.util.List;

public class Statement {
    Statement statement;
    List<Statement> children;


    public Statement getStatement() {
        return statement;
    }

    public void setStatement(Statement statement) {
        this.statement = statement;
    }

    public List<Statement> getChildren() {
        return children;
    }

    public void setChildren(List<Statement> children) {
        this.children = children;
    }

    @Override
    public String toString() {
        return "Statement{" +
                "statement=" + statement +
                ", children=" + children +
                '}';
    }
}
