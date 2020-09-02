package com.araizen.com;

import com.araizen.com.util.print.Log;

import java.io.IOException;
import java.nio.charset.Charset;
import java.nio.file.Files;
import java.nio.file.Paths;
import java.util.List;

public class Main {

    public static void main(String[] args) throws IOException {
        // write your code here
        System.out.println("Stratos compiler");
        String f = "/home/arthur-kamau/Development/Stratos/compiler/example/basic_example/src/main.st";
        runFile(f);
//        if (args.length > 1) {
//            System.out.println("more than one arg Error");
//            System.exit(64);
//        } else if (args.length == 1) {
//            System.out.println("One Command Line Arg fount than one arg Error");
//            // runFile(args[0]);
//            String f = "/home/arthur-kamau/Development/Stratos/compiler/example/basic_example/src/main.st";
//            runFile(f);
//        } else {
//            runPrompt();
//        }
    }

    private static void runFile(String path) throws IOException {

        byte[] byt = Files.readAllBytes(Paths.get(path));
        run(new String(byt, Charset.defaultCharset()));
    }

    private static void run(String src) {
        Scanner scn = new Scanner(src);

        List<Token> tokens = scn.scanTokens();
        /* Scanning run < Parsing Expressions print-ast

            // For now, just print the tokens.

        */
        for (Token token : tokens) {
            Log.process(token.toString());
        }
    }

    private static void runPrompt() {

    }
}
