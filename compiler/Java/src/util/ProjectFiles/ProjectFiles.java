package util.ProjectFiles;

import java.io.*;
import java.util.ArrayList;
import java.util.List;

public class ProjectFiles {
    public String readFile(String path) throws IOException {
        //Creating an InputStream object
        InputStream inputStream = new FileInputStream(path);
        //creating an InputStreamReader object
        InputStreamReader isReader = new InputStreamReader(inputStream);
        //Creating a BufferedReader object
        BufferedReader reader = new BufferedReader(isReader);
        StringBuffer sb = new StringBuffer();
        String str;
        while ((str = reader.readLine()) != null) {
            sb.append(str);
            sb.append("\n");
        }

        return sb.toString();
    }

    public List<String> walk(String path) {
        List<String> files = new ArrayList<>();
        File root = new File(path);
        File[] list = root.listFiles();

        if (list == null) {
            return files;
        }

        for (File f : list) {
            if (f.isDirectory()) {
                walk(f.getAbsolutePath());
                System.out.println("Dir:" + f.getAbsoluteFile());
            } else {
                System.out.println("File:" + f.getAbsoluteFile());
                files.add(f.getAbsolutePath());
            }
        }
        return files;
    }
}
