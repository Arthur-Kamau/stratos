package com.stratos.util;

import java.util.regex.Matcher;
import java.util.regex.Pattern;

public class Util {
    public boolean isNumeric(String text){
        String regex = "^[0-9]+$";
        Pattern pattern = Pattern.compile(regex);
        Matcher matcher = pattern.matcher(text);
        return matcher.matches();
    }
}
