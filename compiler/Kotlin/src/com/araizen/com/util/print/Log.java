package com.araizen.com.util.print;

public class Log {

    public static void process(String str) {
        System.out.println(ConsoleColors.GREEN + "Process :: \t"+  str + ConsoleColors.RESET + " \n");
    }

    public static void error(String str ) {
        System.out.println(ConsoleColors.RED + "Error ::  \t"+  str + ConsoleColors.RESET + " \n");

    }
    public static void error(String str, String line ) {
        System.out.println(ConsoleColors.RED + "Error ::  \t"+  str  +"\t At line "+ line + ConsoleColors.RESET + " \n");
    }
    public static void error(String str, int line ) {
        System.out.println(ConsoleColors.RED + "Error ::  \t"+  str  +"\t At line "+ line + ConsoleColors.RESET + " \n");
    }

    public static void warning(String str) {
        System.out.println(ConsoleColors.YELLOW + "Android :: \t"+  str + ConsoleColors.RESET + " \n");
    }

    public static void purple(String str) {
        System.out.println(ConsoleColors.PURPLE + "Android \t"+  str + ConsoleColors.RESET + " \n");
    }

    public static void cyan(String str) {
        System.out.println(ConsoleColors.CYAN + "Android :: \t"+  str + ConsoleColors.RESET + " \n");
    }

    public static void blue(String str) {
        System.out.println(ConsoleColors.BLUE + "Android :: \t"+ str + ConsoleColors.RESET + " \n");
    }
}
