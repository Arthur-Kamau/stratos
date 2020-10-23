package com.araizen.com.LanguageData;

import java.util.Arrays;
import java.util.List;

public class LanguageData {
    public List<String> getKeyWords(){
        String[] array = {"double", "int", "string", "char", "alias" ,"class" , "then" , "or", "and","function", "some", "none", "return", "package", "import", "implements","extends"};
        List<String> list = Arrays.asList(array);
        return list;
    }
}
