//import analysis.custom.AST.ASTGenerator;
import analysis.antlr.StratosLexer;
import analysis.antlr.StratosParser;
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
import org.antlr.v4.runtime.CharStreams;
import org.antlr.v4.runtime.CommonTokenStream;
import org.antlr.v4.runtime.tree.ParseTree;
import org.antlr.v4.runtime.tree.ParseTreeWalker;
import util.ProjectFiles.ProjectFiles;

import java.io.File;
import java.nio.charset.Charset;
import java.nio.file.Files;
import java.nio.file.Paths;
import java.util.ArrayList;
import java.util.List;
import java.util.Objects;


/***
 *  First read the project app.config
 * In the project there are two analysis ie Lexer and parser implementations
 * 1. is custom parser and lexer
 * 2. is Antlr parser and lexer
 *
 */
public class Compiler {
    private String projectPath;
  public   Compiler(String projectPath){
      this.projectPath =projectPath;
  }

  public void run() throws Exception {

      File file = new File(projectPath);
      boolean hasConfFile = false;
      boolean hasSrcFolder = false;
      if (file.exists()) {
          if(file.isDirectory()){
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

                  //new Compiler().RunProject(path);
                  throw new Exception("No implemented");

              } else {
                  if (!hasConfFile) {
                      System.out.println("app.conf file not found in " + projectPath);
                  } else {
                      System.out.println("src folder not found");
                  }
              }
          }else{
//              new AntlrFrontEnd().runFile(projectPath);
              new StratosFrontEnd().runFile(projectPath);
          }
      } else {
          System.out.println("Project does not exist");
      }
  }


}
class AntlrFrontEnd{

    public void  runFile(String filePath) throws Exception{
        System.out.println("Running Script");
        byte[] bytes = Files.readAllBytes(Paths.get(filePath));
       String sourceCode = new String(bytes, Charset.defaultCharset());
        StratosLexer lexer = new StratosLexer(CharStreams.fromString(sourceCode) );
        CommonTokenStream tokens = new CommonTokenStream(lexer);
StratosParser parser = new StratosParser(tokens);
        System.out.println("hey \n "+parser.expressionList().toString());

    }
}

class StratosFrontEnd{
    public void runProject(String projectPath) throws Exception {
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
//
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

    public void  runFile(String projectPath) throws Exception{
        System.out.println("Run Script");
        List<Node> nodesList = new Lexer().generateNodes(projectPath);
        List<Statement> statementList = new Parser().parse(nodesList);
//
        for (Statement statement : statementList) {
            //Creating the ObjectMapper object
            ObjectMapper mapper = new ObjectMapper();
            //Converting the Object to JSONString
            String jsonString = mapper.writeValueAsString(statement);
            System.out.println("node list ==>" + jsonString);
        }
    }
}
