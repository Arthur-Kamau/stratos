package com.stratos;

import com.fasterxml.jackson.databind.ObjectMapper;
import com.stratos.Config.AppConfigFile;
import com.stratos.Semantics.LanguageStatement;
import com.stratos.analysis.custom.AST.ASTGenerator;
import com.stratos.analysis.custom.Lexer.Lexer;
import com.stratos.analysis.custom.Parser.Parser;
import com.stratos.model.Diagnostics;
import com.stratos.model.Node;
import com.stratos.model.NodeList;
import com.stratos.model.Statement.Statement;
import com.stratos.model.Token;
import com.stratos.util.ProjectFiles.ProjectFiles;
import com.typesafe.config.Config;

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
        AppConfigFile confi=   new AppConfigFile();
        Config conf = confi.parse(projectPath);
        List<Diagnostics> dd = confi.validateConfigFile(conf);

//        for (Diagnostics dig : dd){
//            System.out.println(" diagonistics "+ dig.toString() );
//        }

        //String mainFile = projectPath+"/src/main.st";
        List<String> projectFiles  = new ProjectFiles().walk(projectPath+"/src");
        //read all the files

        for (String file : projectFiles){
            List<Node> nodesList = new Lexer().generateNodes(file);
            NodeList nodeList = new Parser().parse(nodesList);
//            for (List<Node> n: nodeList.getnodesGroup()) {
//                //Creating the ObjectMapper object
//                ObjectMapper mapper = new ObjectMapper();
//                //Converting the Object to JSONString
//                String jsonString = mapper.writeValueAsString(n);
//                System.out.println("node list ==>"+jsonString);
//            }


            List<Token> tokenList = new ASTGenerator(nodeList).generate();

            for (Token n: tokenList) {
                //Creating the ObjectMapper object
                ObjectMapper mapper = new ObjectMapper();
                //Converting the Object to JSONString
                String jsonString = mapper.writeValueAsString(n);
                System.out.println("Token list ==>"+jsonString);
            }



//            List<Statement> statements = new LanguageStatement().analysis(tokenList);


//            for (Statement n: statements) {
//                //Creating the ObjectMapper object
//                ObjectMapper mapper = new ObjectMapper();
//                //Converting the Object to JSONString
//                String jsonString = mapper.writeValueAsString(n);
//                System.out.println("Statement "+n.toString());
//            }


//            for (int i = 0; i < tokenList.getnodesGroup().size(); i++) {
//                System.out.println("  size  "+tokenList.getnodesGroup().size()+" index "+ i +"  Root node  "+ tokenList.getnodesGroup().get(i).toString());
//            }

//            OldParseTree parserTree = new OldParseTree();
//            List<TokenTree> tokenTrees =parserTree.parse(tokenList);
//            for (int i = 0; i < tokenTrees.size(); i++) {
//               Log.cyan(i+" Token trees "+ tokenTrees.get(i).toString());
//                if(tokenTrees.get(i).getNodesChildren() != null){
//                    int size= tokenTrees.get(i).getNodesChildren().getNodes().size();
//                    for (int j = 0; j < size ; j++) {
//                        Log.blue(" Child "+ tokenTrees.get(i).getNodesChildren().getNodes().get(j) );
//                    }
//                }
//            }


        }









//

//                for (int i = 0; i < tokenList.getnodesGroup().size(); i++) {
//                    System.out.println("Token  size  "+tokenList.getnodesGroup().size()+"  Root node  "+ tokenList.getnodesGroup().get(i).toString());
//                }
//                List<TokenTree> tokenTree = parserTree.parse(tokenList);
//
//                for (int i = 0; i < tokenTree.size(); i++) {
//                    System.out.println("  size  "+tokenTree.size()+" index "+ i +"  Root node  "+ tokenTree.get(i).toString());
//                }

//                SemanticAnalysis  semanticAnalysis = new SemanticAnalysis();
//                List<Statement> statements = semanticAnalysis.analysis(tokenList);
//
//
//                for (int i = 0; i < statements.size(); i++) {
//                    System.out.println(" statements  "+ statements.get(i).toString());
//                }
    }


}

// todo check antlr
// if(new File(mainFile).exists()){
//         try {
//         CharStream input = (CharStream) new ANTLRFileStream(mainFile);
//         StratosLexer lexer = new StratosLexer(input);
//         StratosParser parser = new StratosParser(new CommonTokenStream(lexer));
//         parser.addParseListener(new StratosCustomListener());
//         parser.sourceFile();
//         } catch (IOException ex) {
//         System.out.println("\n error  cause "+ex.getCause()+"  message  " +ex.getMessage()+"   \n trace "+ ex.getStackTrace()+"  \n");
//         Logger.getLogger(Compiler.class.getName()).log(Level.SEVERE, "Exception encoutered", ex);
//        }
//        }else{
//        throw  new Exception("Could not find main.st , main function");
//        }
