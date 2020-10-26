package com.araizen.com;

import com.araizen.com.Analysis.SemanticAnalysis;
import com.araizen.com.Config.AppConfigFile;
import com.araizen.com.Lexer.Lexer;
import com.araizen.com.Parser.Parser;
import com.araizen.com.Parser.ParseTree;
import com.araizen.com.model.*;
import com.araizen.com.model.Statement.Statement;
import com.araizen.com.util.StringUtil.StringUtil;

import java.io.*;
import java.nio.charset.Charset;
import java.nio.file.Files;
import java.nio.file.Path;
import java.nio.file.Paths;
import java.util.List;
import java.util.Objects;
import java.util.Properties;

public class Main {

    public static void main(String[] args) throws IOException {
        // write your code here
        System.out.println("Stratos compiler");


        String projePath = System.getProperty("user.dir");
        String path = projePath + "/example/basic_example";

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

                ProjectConfigOptions conf = new AppConfigFile().parse(path);

                List<Node> nodesList = new Lexer().generateNodes(path+"/src/main.st");

//                for (int i = 0; i < nodesList.size(); i++) {
//                    System.out.println(i+" Node "+ nodesList.get(i).toString());
//                }

                Parser p = new Parser();
                ParseTree parserTree = new ParseTree();
                Token tokenList = p.parse(nodesList);

//                for (int i = 0; i < tokenList.getnodesGroup().size(); i++) {
//                    System.out.println("Token  size  "+tokenList.getnodesGroup().size()+"  Root node  "+ tokenList.getnodesGroup().get(i).toString());
//                }
                List<TokenTree> tokenTree = parserTree.parse(tokenList);

                for (int i = 0; i < tokenTree.size(); i++) {
                    System.out.println("  size  "+tokenTree.size()+" index "+ i +"  Root node  "+ tokenTree.get(i).toString());
                }

//                SemanticAnalysis  semanticAnalysis = new SemanticAnalysis();
//                List<Statement> statements = semanticAnalysis.analysis(tokenList);
//
//
//                for (int i = 0; i < statements.size(); i++) {
//                    System.out.println(" statements  "+ statements.get(i).toString());
//                }


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
