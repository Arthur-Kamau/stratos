package com.araizen.com;

import java.io.IOException;
import java.nio.charset.Charset;
import java.nio.file.Files;
import java.nio.file.Paths;

public class Main {

    public static void main(String[] args) {
	// write your code here
        System.out.println("Stratos comipler");

        if(args.length > 1){
            System.out.println("more than one arg Error");
            System.exit(64);
        }else if(args.length ==1){
            runFile(args[0]);
        }else{
            runPrompt();
        }
    }

    private  static void runFile(String path) throws IOException {

byte[]  byt = Files.readAllBytes(Paths.get(path));
run (new String(byt, Charset.defaultCharset()));
    }

    private static void run (String src){
        Scanner scn =  new Scanner(src)
    }


    private static void runPrompt (){

    }
}
