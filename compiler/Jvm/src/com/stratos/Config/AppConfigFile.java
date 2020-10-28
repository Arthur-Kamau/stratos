package com.stratos.Config;

import com.stratos.model.Diagnostics;
import com.stratos.model.SeverityLevel;
import com.stratos.util.Language.Language;
import com.stratos.util.ProjectFiles.ProjectFiles;
import com.typesafe.config.Config;
import com.typesafe.config.ConfigFactory;
import com.typesafe.config.ConfigValue;

import java.io.*;
import java.util.*;

public class AppConfigFile {


    public Config  parse(String filePath) throws Exception {


        String filename = filePath + File.separatorChar + "app.conf";
        try {
           String content = new ProjectFiles().readFile(filename);
            Config config = ConfigFactory.parseString(content);


            return  config;


        } catch (FileNotFoundException ex) {
            System.out.println("File error \n Cause " + ex.getCause() + "\n Message " + ex.getMessage() + " \n Stack trace" + ex.getStackTrace());
        }

        return  null;
    }

   public List<Diagnostics> validateConfigFile(Config data){
        List<Diagnostics>  diagnostics = new ArrayList<>();

        List<String> reqFields = new ArrayList<String>();
       reqFields.addAll(new Language().getAppConfigKeyDetails());

        for (Map.Entry<String, ConfigValue> entry: data.root().entrySet()) {
//            System.out.println("Root key = " + entry.getKey() + " value "+ entry.getValue());
            reqFields.remove(entry.getKey());

        }

        if(!reqFields.isEmpty()){
            for (String field :  reqFields){
                diagnostics.add(new Diagnostics(
                        field + " is missing resulting to default",
                        SeverityLevel.Information,
                        0, 0,0,0
                ));
            }
        }
        return diagnostics;
    }

}
