package com.stratos.util.LanguageData;

import com.stratos.model.NodeType;

import java.util.Arrays;
import java.util.List;

public class LanguageData {
    public List<String> getKeyWords() {
        String[] array = {"val","let","var","list" ,"enum","struct","double", "int", "private" ,"string", "char", "alias", "class", "then", "or", "and", "function", "some", "none", "return", "package", "import", "implements", "extends"};
        List<String> list = Arrays.asList(array);
        return list;
    }

    public NodeType getKeyWordNodeType(String text) {
        switch (text) {

            case "var":
                return NodeType.VarNode;
            case "val":
                return NodeType.ValNode;
            case "let":
                return NodeType.LetNode;
            case "list":
                return NodeType.ListNode;
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
                return NodeType.ImplimentsNode;
            case "extends":
                return NodeType.ExtendsNode;
            case "private":
                return NodeType.PrivateNode;

            default:
                return NodeType.AlphaNumericNode;
        }


    }
}
