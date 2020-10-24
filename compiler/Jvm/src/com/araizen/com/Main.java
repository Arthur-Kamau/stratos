package com.araizen.com;

import com.araizen.com.Config.AppConfigFile;
import com.araizen.com.Lexer.Lexer;
import com.araizen.com.Parser.Parser;
import com.araizen.com.model.Diagnostics;
import com.araizen.com.model.Node;
import com.araizen.com.model.ProjectConfigOptions;
import com.araizen.com.model.Token;
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

                Token tokenList = p.parse(nodesList);
                List<Diagnostics>  diagnostics =  p.getDiagnostics();

                for (int i = 0; i < tokenList.getnodesGroup().size(); i++) {
                    System.out.println("  size  "+tokenList.getnodesGroup().size()+"  Root node  "+ tokenList.getnodesGroup().get(i).toString());
                }

                for (int i = 0; i < tokenList.getChildren().size(); i++) {
                    System.out.println(" Root Children  "+ tokenList.getChildren().get(i).toString());
                }
//
//                for (int i = 0; i < diagnostics.size(); i++) {
//                    System.out.println(" diagnostics  "+ diagnostics.get(i).toString());
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
