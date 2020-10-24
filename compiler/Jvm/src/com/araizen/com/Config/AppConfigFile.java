package com.araizen.com.Config;

import com.araizen.com.model.ProjectConfigOptions;

import java.io.*;
import java.util.Arrays;
import java.util.Enumeration;
import java.util.List;
import java.util.Properties;

public class AppConfigFile {
    private List<String> mandatoryFields = Arrays.asList("name", "version");

    public ProjectConfigOptions parse(String filePath) throws Exception {
        ProjectConfigOptions projectConfigOptions = new ProjectConfigOptions();
        // read conf file
        Properties prop = new Properties();

        String filename = filePath + File.separatorChar + "app.conf";
        try {
            InputStream is = new FileInputStream(filename);
            prop.load(is);
            Enumeration<?> propertNames =  prop.propertyNames();
//            for ( Object v:propertNames
//                 ) {
//
//            }
//            propertNames.removeAll(mandatoryFields);
//            if (propertNames.contains("name")) {
//                throw new Exception("Name not exists in config file");
//            }

        } catch (FileNotFoundException ex) {
            System.out.println("File error \n Cause " + ex.getCause() + "\n Message " + ex.getMessage() + " \n Stack trace" + ex.getStackTrace());
        }

        return  projectConfigOptions;
    }
}
