package com.stratos;


import com.stratos.analysis.Lexer.Lexer;
import com.stratos.analysis.Parser.Statement;
import com.stratos.core.Interpreter;
import com.stratos.core.Resolver;
import com.stratos.model.Token;
import com.stratos.analysis.Parser.Parser;



import java.io.File;
import java.io.IOException;
import java.nio.file.Files;
import java.nio.file.Paths;
import java.util.List;
import java.util.Objects;

public class Stratos {

    private static final Interpreter interpreter = new Interpreter();

    public static void main(String[] args) throws IOException {
        // write your code here
        System.out.println("Stratos compiler");


        String projePath = System.getProperty("user.dir");
        String path = projePath + "/example/basic_example/src/main.st";

        File file = new File(path);
        if (file.isDirectory()) {
            try {
                runProject(path);
            } catch (Exception ex) {
                System.out.println("File error \n Cause " + ex.getCause() + "\n Message " + ex.getMessage() + " \n Stack trace" + ex.getStackTrace());
                ex.printStackTrace();
            }
        } else {

            try {
                runFile(path);
            } catch (Exception ex) {
                System.out.println("File error \n Cause " + ex.getCause() + "\n Message " + ex.getMessage() + " \n Stack trace" + ex.getStackTrace());
                ex.printStackTrace();
            }
        }
    }

    private static void runFile(String path) throws Exception {
        byte[] bytes = Files.readAllBytes(Paths.get(path));
        String s = new String(bytes);

        Lexer scanner = new Lexer(s);
        List<Token> tokens = scanner.scanTokens();

//        for (Token item: tokens ) {
//            System.out.println("item "+item.toString());
//        }

        Parser parser = new Parser(tokens);


        List<Statement> statements = parser.parse();
        for (Statement item: statements ) {

            if (item == null) {
                System.out.println("null .... ");
            }else{
                System.out.println(item.toString());
            }
        }




        Resolver resolver = new Resolver(interpreter);
        resolver.resolve(statements);



        //> Statements and State interpret-statements
        interpreter.interpret(statements);
//< Statements and State interpret-statements

    }

    private static void runProject(String path) throws Exception {

        File file = new File(path);
        boolean hasConfFile = false;
        boolean hasSrcFolder = false;
        if (file.exists()) {
            // check if the run folder has a conf file
            for (final File fileEntry : Objects.requireNonNull(file.listFiles())) {
                if (fileEntry.isDirectory()) {

                    if (fileEntry.getName().equals("src")) {
                        hasSrcFolder = true;
                    }
                } else {

                    if (fileEntry.getName().equals("app.conf")) {

                        hasConfFile = true;
                    }

                }
            }

            if (hasConfFile && hasSrcFolder) {

                // todo call the compiler
                System.out.println("TODO");

            } else {
                if (!hasConfFile) {
                    System.out.println("app.conf file not found in " + path);
                } else {
                    System.out.println("src folder not found");
                }
            }
        } else {
            System.out.println("Project does not exist");
        }
    }


}
