package com.stratos.util.ProjectFiles;

import java.io.File;
import java.util.ArrayList;
import java.util.List;

public class ProjectFiles {
    public List<String> walk(String path ) {
        List<String> files =  new ArrayList<>();
        File root = new File( path );
        File[] list = root.listFiles();

        if (list == null) {
            return  files;
        }

        for ( File f : list ) {
            if ( f.isDirectory() ) {
                walk( f.getAbsolutePath() );
                System.out.println( "Dir:" + f.getAbsoluteFile() );
            }
            else {
                System.out.println( "File:" + f.getAbsoluteFile() );
                files.add(f.getAbsolutePath());
            }
        }
        return  files;
    }
}
