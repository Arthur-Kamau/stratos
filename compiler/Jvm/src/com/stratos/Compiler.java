package com.stratos;

import com.stratos.Config.AppConfigFile;
import com.stratos.model.Diagnostics;
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

        for (Diagnostics dig : dd){
            System.out.println(" diagonistics "+ dig.toString() );
        }

        //String mainFile = projectPath+"/src/main.st";
//        List<String> projectFiles  = walk(projectPath);
        //read all the files

//        for (String file : projectFiles){
//            List<Node> nodesList = new Lexer().generateNodes(file);
//            Parser p = new Parser();
//            Token tokenList = p.parse(nodesList);
//
//
//        }




//


//        ParseTree parserTree = new ParseTree();

//                for (int i = 0; i < nodesList.size(); i++) {
//                    System.out.println(i+" Node "+ nodesList.get(i).toString());
//                }


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
