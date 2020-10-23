package com.araizen.com.util.StringUtil;

public class StringUtil {
    public boolean isAlphaNumeric(String text){
        return   text.matches("^.*[^a-zA-Z0-9 ]+$");
    }
    public boolean isNumeric(String text){
        return   text.matches("^.*[^0-9 ]+$");
    }
    public boolean isNegativeDouble(String text){
        return text.matches("^-?[0-9][0-9,\\.]+$");
    }
}
