package org.stratos.language;


import java.io.File;
import java.io.PrintWriter;
import java.util.ArrayList;
import java.util.List;

public class FileWalker {

    public List<String> walk(String path ,  PrintWriter outWriter,PrintWriter errWriter) {

        List<String> files  = new ArrayList<>();

        File root = new File( path );
        File[] list = root.listFiles();

        if (list == null) return new ArrayList<>();

        for ( File f : list ) {
            if ( f.isDirectory() ) {
                files.addAll(walk( f.getAbsolutePath() , outWriter,errWriter));
            }
            else {

                String fileName =f.getName();
                if(fileName.equals("app.conf") ){
                    outWriter.println("Found app conf file ");
                }else if (fileName.endsWith(".st") || fileName.endsWith(".sts") ) {

                    files.add(f.getAbsolutePath());
                }else{
                    errWriter.println("Skipping non-Stratos file: " + fileName);
                    continue;
                }
            }
        }
        return files;
    }




}