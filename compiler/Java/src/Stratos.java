
import analysis.custom.Lexer.Lexer;
import analysis.custom.Parser.Parser;
import com.fasterxml.jackson.databind.ObjectMapper;
import model.Node;
import model.NodeList;
import model.Statement.Statement;

import java.io.*;
import java.util.List;
import java.util.Objects;

/***
 * Main file
 * checks if the compiler is invoked to run a file or  run a project
 * a project must contain a src folder and app.conf file
 * if a file default project  configurations are generated
 * then invokes the compiler run
 */
public class Stratos {

    public static void main(String[] args) throws IOException {
        // write your code here
        System.out.println("Stratos compiler");


        String projePath = System.getProperty("user.dir");
        String path = projePath + "/example/basic_example/src/main.st";

        File file = new File(path);
     if(file.exists()){
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
     }else{
         System.out.println("File Or Folder does not exist \n "+path);
     }


    }

    private static void runFile(String path) throws Exception {
        List<Node> _nodesList = new Lexer().generateNodes(path);

        for (Node n : _nodesList) {
            ObjectMapper mapper = new ObjectMapper();
            //Converting the Object to JSONString
            String jsonString = mapper.writeValueAsString(n);
            System.out.println("node list ==>" + jsonString);

        }


//     List<Statement>    statementList = new Parser().parse(_nodesList);
//        for (Statement n : statementList) {
//            //Creating the ObjectMapper object
//            ObjectMapper mapper = new ObjectMapper();
//            //Converting the Object to JSONString
//            String jsonString = mapper.writeValueAsString(n);
//            System.out.println("node list ==>" + jsonString);
//        }
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

                new Compiler().RunProject(path);


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
