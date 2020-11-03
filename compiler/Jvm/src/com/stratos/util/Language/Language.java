package com.stratos.util.Language;

import com.stratos.model.NodeType;

import java.util.Arrays;
import java.util.List;

public class Language {
    public List<String> getAppConfigKeyDetails() {
        String[] array = {"name", "version", "description","author", "dependencies","keywords"};
        List<String> list = Arrays.asList(array);
        return list;
    }

    public List<String> getKeyWords() {
        String[] array = {"val","let","var","list" , "if", "else", "when","enum","struct","double", "int", "private" ,"string", "char", "alias", "class", "then", "or", "and", "function", "some", "none", "return", "package", "import", "implements", "extends"};
        List<String> list = Arrays.asList(array);
        return list;
    }



    public NodeType getKeyWordNodeType(String text) {
        switch (text) {

            case "var":
                return NodeType.VarNode;
            case "val":
                return NodeType.ValNode;
            case "when":
                return NodeType.WhenNode;
            case "let":
                return NodeType.LetNode;
            case "list":
                return NodeType.ListNode;
            case "if":
                return NodeType.IfNode;
            case "else":
                return NodeType.ElseNode;
            case "enum":
                return NodeType.EnumNode;
            case "strut":
                return NodeType.StructNode;
            case "double":
                return NodeType.DoubleNode;
            case "int":
                return NodeType.IntegerNode;
            case "string":
                return NodeType.StringNode;
            case "char":
                return NodeType.CharacterNode;
            case "alias":
                return NodeType.AliasNode;
            case "class":
                return NodeType.ClassNode;
            case "then":
                return NodeType.ThenNode;
            case "or":
                return NodeType.OrNode;
            case "and":
                return NodeType.AndNode;
            case "function":
                return NodeType.FunctionNode;
            case "some":
                return NodeType.SomeNode;
            case "none":
                return NodeType.NoneNode;
            case "return":
                return NodeType.ReturnNode;
            case "package":
                return NodeType.PackageNode;
            case "import":
                return NodeType.ImportNode;
            case "implements":
                return NodeType.ImplementsNode;
            case "extends":
                return NodeType.ExtendsNode;
            case "private":
                return NodeType.PrivateNode;

            default:
                return NodeType.AlphaNumericNode;
        }


    }
}
