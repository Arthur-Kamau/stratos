package com.stratos.old.Statement;

import com.stratos.model.Node;

public class ImportStatement extends  Statement{
    Node path;
    Node importKeyWord;

    public ImportStatement(Node path, Node importKeyWord) {
        this.path = path;
        this.importKeyWord = importKeyWord;
    }

    public Node getPath() {
        return path;
    }

    public void setPath(Node path) {
        this.path = path;
    }

    public Node getImportKeyWord() {
        return importKeyWord;
    }

    public void setImportKeyWord(Node importKeyWord) {
        this.importKeyWord = importKeyWord;
    }
}
