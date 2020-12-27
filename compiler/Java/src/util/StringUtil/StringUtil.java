package util.StringUtil;

import java.util.regex.Matcher;
import java.util.regex.Pattern;

public class StringUtil {
    public boolean isAlphaNumeric(String text){
        String regex = "^[a-zA-Z0-9]+$";
        Pattern pattern = Pattern.compile(regex);
        Matcher matcher = pattern.matcher(text);
        return matcher.matches();
    }
    public boolean isNumeric(String text){
        String regex = "^[0-9]+$";
        Pattern pattern = Pattern.compile(regex);
        Matcher matcher = pattern.matcher(text);
        return matcher.matches();
    }
    public boolean isNegativeDouble(String text){
        return text.matches("^-?[0-9][0-9,\\.]+$");
    }
}
