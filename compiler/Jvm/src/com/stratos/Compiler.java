package com.stratos;

import com.fasterxml.jackson.databind.ObjectMapper;
import com.stratos.Config.AppConfigFile;
import com.stratos.Executor.Executor;
import com.stratos.Syntax.StratosSyntax;
import com.stratos.Validators.Validators;
import com.stratos.analysis.custom.AST.ASTGenerator;
import com.stratos.analysis.custom.Lexer.Lexer;
import com.stratos.analysis.custom.Parser.Parser;
import com.stratos.model.*;

import com.stratos.model.Statement.Statement;
import com.stratos.util.ProjectFiles.ProjectFiles;
import com.typesafe.config.Config;

import java.util.ArrayList;
import java.util.List;


/***
 *  First read the project app.config
 * In the project there are two analysis ie Lexer and parser implementations
 * 1. is custom parser and lexer
 * 2. is Antlr parser and lexer
 *
 */
public class Compiler {
    public void RunProject(String projectPath) throws Exception {
        AppConfigFile config = new AppConfigFile();
        Config conf = config.parse(projectPath);
        List<Diagnostics> diagnostics = config.validateConfigFile(conf);

//        for (Diagnostics dig : dd){
//            System.out.println(" diagonistics "+ dig.toString() );
//        }

        //String mainFile = projectPath+"/src/main.st";
        List<String> projectFiles = new ProjectFiles().walk(projectPath + "/src");
        //read all the files

        List<ExecutionTree> executionTrees = new ArrayList<>();
        for (String file : projectFiles) {
            List<Node> nodesList = new Lexer().generateNodes(file);
            NodeList nodeList = new Parser().parse(nodesList);
            for (List<Node> n: nodeList.getnodesGroup()) {
                //Creating the ObjectMapper object
                ObjectMapper mapper = new ObjectMapper();
                //Converting the Object to JSONString
                String jsonString = mapper.writeValueAsString(n);
                System.out.println("node list ==>"+jsonString);
            }


            System.out.println("\n\n ================= \n\n ");

            List<Token> tokenList = new ASTGenerator(nodeList).generate();

            for (Token n: tokenList) {
                //Creating the ObjectMapper object
                ObjectMapper mapper = new ObjectMapper();
                //Converting the Object to JSONString
                String jsonString = mapper.writeValueAsString(n);
                System.out.println("Token list ==>"+jsonString);
            }


            List<Statement> statements = new StratosSyntax().createStatements(tokenList);


//            for (Statement n : statements) {
//                //Creating the ObjectMapper object
//                ObjectMapper mapper = new ObjectMapper();
//                //Converting the Object to JSONString
//                String jsonString = mapper.writeValueAsString(n);
////                System.out.println("Statement " + n.toString());
//                System.out.println("Statement " + jsonString);
//
//            }

            Validators validators = new Validators();
            List<Statement> validatedStatements = validators.validate(statements);

            List<Diagnostics> validationDiagonistics = validators.getDiagnostics();
            diagnostics.addAll(validationDiagonistics);


            executionTrees.add(new ExecutionTree(
                    file,
                    false,
                    false,
                    validatedStatements
            ));


        }

        Executor executor =  new Executor(executionTrees,  conf, projectPath);
        executor.execute();

    }





}

