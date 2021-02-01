
import analysis.custom.Lexer.Lexer;
import analysis.custom.Parser.Parser;
import com.fasterxml.jackson.databind.ObjectMapper;
import model.Node;
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


        //todo check args passed in

        try {
            new    Compiler(path).run();
        } catch (Exception e) {
            e.printStackTrace();
            System.out.println("Failed .............");
        }
    }
}
//
//        File file = new File(path);
//     if(file.exists()){
//         if (file.isDirectory()) {
//             try {
//                 runProject(path);
//             } catch (Exception ex) {
//                 System.out.println("File error \n Cause " + ex.getCause() + "\n Message " + ex.getMessage() + " \n Stack trace" + ex.getStackTrace());
//                 ex.printStackTrace();
//             }
//         } else {
//
//             try {
//
//                 runFile(path);
//             } catch (Exception ex) {
//                 System.out.println("File error \n Cause " + ex.getCause() + "\n Message " + ex.getMessage() + " \n Stack trace" + ex.getStackTrace());
//                 ex.printStackTrace();
//             }
//         }
//     }else{
//         System.out.println("File Or Folder does not exist \n "+path);
//     }
//
//
//    }
//
//    private static void runFile(String path) throws Exception {
//        List<Node> _nodesList = new Lexer().generateNodes(path);
//
////        for (Node n : _nodesList) {
////            ObjectMapper mapper = new ObjectMapper();
////            //Converting the Object to JSONString
////            String jsonString = mapper.writeValueAsString(n);
////            System.out.println("node list ==>" + jsonString);
////
////        }
//
//
//     List<Statement>    statementList = new Parser().parse(_nodesList);
//        for (Statement n : statementList) {
//            //Creating the ObjectMapper object
//            ObjectMapper mapper = new ObjectMapper();
//            //Converting the Object to JSONString
//            String jsonString = mapper.writeValueAsString(n);
//            System.out.println("node list ==>" + jsonString);
//        }
//    }
//
//    private static void runProject(String path) throws Exception {
//
//    }
//
//
//}
