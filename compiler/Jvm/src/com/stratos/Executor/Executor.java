package com.stratos.Executor;


import com.stratos.model.ExecutionTree;
import com.stratos.model.Statement.ExpressionStatement;
import com.stratos.model.Statement.OperationStatement;
import com.stratos.model.Statement.Statement;
import com.typesafe.config.Config;
import com.typesafe.config.ConfigValue;

import java.util.List;
import java.util.Map;
import java.util.Set;

public class Executor {
    List<ExecutionTree> executionTree;
    Config conf;
    String projectPath;
    public Executor(List<ExecutionTree> statementList, Config conf , String projectPath){
        this.executionTree = statementList;
        this.conf= conf;
        this.projectPath = projectPath ;
    }
    public void execute(){

//        String main = findMain();


        for (ExecutionTree tree : executionTree ){
            for (Statement stm:
          tree.getStatements()  ) {
              if(stm instanceof OperationStatement){
                  if(((OperationStatement) stm).isComplexStatement()){
                      System.out.println("complex stement");
                  }else{

                      System.out.println("simple ");
                  }
              }
            }
        }
    }

   private String  findMain( ){
        String main = "";
       for (Map.Entry<String, ConfigValue> entry: conf.root().entrySet()) {

           if(entry.getKey().equals("main")){
               main = entry.getValue().render();
           }

       }

       if(!main.isEmpty()){
           // find the package  and method exist

       }else{
           // scann the excution tree for main  function
           
       }

       return  main;
    }


}
