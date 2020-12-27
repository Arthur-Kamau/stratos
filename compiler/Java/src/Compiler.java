//import analysis.custom.AST.ASTGenerator;
import analysis.custom.Lexer.Lexer;
import analysis.custom.Parser.Parser;
import com.fasterxml.jackson.databind.ObjectMapper;
import config.AppConfigFile;
import model.Diagnostics;
import model.ExecutionTree;
//import old.stratos.Config.AppConfigFile;
//import old.stratos.Executor.Executor;
//import old.stratos.Syntax.StratosSyntax;
//import old.stratos.Validators.Validators;
//import old.stratos.analysis.custom.AST.ASTGenerator;
//import old.stratos.analysis.custom.Lexer.Lexer;
//import old.stratos.analysis.custom.Parser.Parser;

import model.*;
//import model.Statement.Statement;
//import old.stratos.util.ProjectFiles.ProjectFiles;
import com.typesafe.config.Config;
import model.Statement.Statement;
import util.ProjectFiles.ProjectFiles;

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
            List<Statement> statementList = new Parser().parse(nodesList);

            for (Statement statement : statementList) {
                //Creating the ObjectMapper object
                ObjectMapper mapper = new ObjectMapper();
                //Converting the Object to JSONString
                String jsonString = mapper.writeValueAsString(statement);
                System.out.println("node list ==>" + jsonString);
            }


            System.out.println("\n\n ================= \n\n ");


        }
    }

//            List<Statement> statements = new StratosSyntax().createStatements(tokenList);


//            for (Statement n : statements) {
//                //Creating the ObjectMapper object
//                ObjectMapper mapper = new ObjectMapper();
//                //Converting the Object to JSONString
//                String jsonString = mapper.writeValueAsString(n);
////                System.out.println("Statement " + n.toString());
//                System.out.println("Statement " + jsonString);
//
//            }

//            Validators validators = new Validators();
//            List<Statement> validatedStatements = validators.validate(statements);
//
//            List<Diagnostics> validationDiagonistics = validators.getDiagnostics();
//            diagnostics.addAll(validationDiagonistics);
//
//
//            executionTrees.add(new ExecutionTree(
//                    file,
//                    false,
//                    false,
//                    validatedStatements
//            ));


//        }
//
//        Executor executor =  new Executor(executionTrees,  conf, projectPath);
//        executor.execute();

//    }





}

