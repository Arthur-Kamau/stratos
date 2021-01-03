package model;

import model.Statement.Statement;

import java.util.List;

public class Ast {

    String type;
    int start;
    int end;
    List<Statement> body;


    public Ast(String type, int start, int end, List<Statement> body) {
        this.type = type;
        this.start = start;
        this.end = end;
        this.body = body;
    }

    public String getType() {
        return type;
    }

    public void setType(String type) {
        this.type = type;
    }

    public int getStart() {
        return start;
    }

    public void setStart(int start) {
        this.start = start;
    }

    public int getEnd() {
        return end;
    }

    public void setEnd(int end) {
        this.end = end;
    }

    public List<Statement> getBody() {
        return body;
    }

    public void setBody(List<Statement> body) {
        this.body = body;
    }
}
