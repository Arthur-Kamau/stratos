package model.Statement;

import model.Node;

public class PackageStatement extends  Statement{
    Node name;
    Node packageKeyWord;

    public PackageStatement(Node name, Node packageKeyWord) {
        this.name = name;
        this.packageKeyWord = packageKeyWord;
    }

    @Override
    public String toString() {
        return "PackageStatement {" +
                "name=" + name.getValue() +
                ", packageKeyWord=" + packageKeyWord.getValue() +
                '}';
    }

    public Node getName() {
        return name;
    }

    public void setName(Node name) {
        this.name = name;
    }

    public Node getPackageKeyWord() {
        return packageKeyWord;
    }

    public void setPackageKeyWord(Node packageKeyWord) {
        this.packageKeyWord = packageKeyWord;
    }
}
