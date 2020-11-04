package com.stratos.Executor;


import com.stratos.model.ExecutionTree;
import com.stratos.model.Statement.Statement;
import com.typesafe.config.Config;
import com.typesafe.config.ConfigValue;

import java.util.List;
import java.util.Map;
import java.util.Set;

public class Executor {
    List<ExecutionTree> statementList;
    Config conf;
    String projectPath;
    public Executor(List<ExecutionTree> statementList, Config conf , String projectPath){
        this.statementList = statementList;
        this.conf= conf;
        this.projectPath = projectPath ;
    }
    public void execute(){

       String main = findMain();
        System.out.println("===== main   ====");
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
