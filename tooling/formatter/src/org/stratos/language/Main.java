package org.stratos.language;

import java.io.*;
import java.nio.file.Files;
import java.nio.file.Path;
import java.nio.file.Paths;
import java.util.ArrayList;
import java.util.LinkedHashMap;
import java.util.List;
import java.util.Map;
import java.util.concurrent.ExecutionException;
import java.util.concurrent.ExecutorService;
import java.util.concurrent.Executors;
import java.util.concurrent.Future;

import static java.nio.charset.StandardCharsets.UTF_8;

public class Main {


    private final PrintWriter outWriter;
    private final PrintWriter errWriter;
    private final InputStream inStream;

    public Main(PrintWriter outWriter, PrintWriter errWriter, InputStream inStream) {
        this.outWriter = outWriter;
        this.errWriter = errWriter;
        this.inStream = inStream;
    }

    public static void main(String[] args) {
	// write your code here
        System.out.println("Formatter");


        int result;
        PrintWriter out = new PrintWriter(new OutputStreamWriter(System.out, UTF_8));
        PrintWriter err = new PrintWriter(new OutputStreamWriter(System.err, UTF_8));
        try {
            Main formatter = new Main(out, err, System.in);
            result = formatter.format(args);
        } catch (Exception e) {
            err.print(e.getMessage());
            result = 0;
        } finally {
            err.flush();
            out.flush();
        }
        System.exit(result);
    }


    /**
     * The main entry point for the formatter, with some number of file names to format. We process
     * them in parallel, but we must be careful; if multiple file names refer to the same file (which
     * is hard to determine), we must serialize their update.
     *
     * @param args the command-line arguments
     */
    public int format(String... args) throws  Exception {
        List<String> files = new ArrayList<>();

        if(args.length == 0){
            throw new  Exception("No path to project specified");
        }else{
            File file = new File(args[0]);
            if(file.exists()){
                     files.addAll(  new FileWalker().walk(file.getAbsolutePath(),outWriter, errWriter));

            }else{
                throw new  Exception("Specified path does not exist");
            }
        }


        Map<Path, String> inputs = new LinkedHashMap<>();


        for (String f:files
             ) {
            System.out.println("fmt file "+f);


            Path path = Paths.get(f);
            String input;
            try {
                input = new String(Files.readAllBytes(path), UTF_8);
                inputs.put(path, input);

            } catch (IOException e) {
                errWriter.println(path.getFileName() + ": could not read file: " + e.getMessage());

            }
        }

        return 0;


    }

     private int formatFiles(Map<Path, String> inputs){

         Map<Path, Future<String>> results = new LinkedHashMap<>();


        return  0;
     }

//    private int formatFiles(CommandLineOptions parameters, JavaFormatterOptions options) {
//        int numThreads = Math.min(MAX_THREADS, parameters.files().size());
//        ExecutorService executorService = Executors.newFixedThreadPool(numThreads);
//
//        Map<Path, String> inputs = new LinkedHashMap<>();
//        Map<Path, Future<String>> results = new LinkedHashMap<>();
//        boolean allOk = true;
//
//        for (String fileName : parameters.files()) {
//            if (!fileName.endsWith(".st") || !fileName.endsWith(".sts") ) {
//                errWriter.println("Skipping non-Java file: " + fileName);
//                continue;
//            }
//            Path path = Paths.get(fileName);
//            String input;
//            try {
//                input = new String(Files.readAllBytes(path), UTF_8);
//                inputs.put(path, input);
//                results.put(
//                        path, executorService.submit(new FormatFileCallable(parameters, input, options)));
//            } catch (IOException e) {
//                errWriter.println(fileName + ": could not read file: " + e.getMessage());
//                allOk = false;
//            }
//        }
//
//        for (Map.Entry<Path, Future<String>> result : results.entrySet()) {
//            Path path = result.getKey();
//            String formatted;
//            try {
//                formatted = result.getValue().get();
//            } catch (InterruptedException e) {
//                errWriter.println(e.getMessage());
//                allOk = false;
//                continue;
//            } catch (ExecutionException e) {
//                if (e.getCause() instanceof FormatterException) {
//                    for (FormatterDiagnostic diagnostic : ((FormatterException) e.getCause()).diagnostics()) {
//                        errWriter.println(path + ":" + diagnostic.toString());
//                    }
//                } else {
//                    errWriter.println(path + ": error: " + e.getCause().getMessage());
//                    e.getCause().printStackTrace(errWriter);
//                }
//                allOk = false;
//                continue;
//            }
//            boolean changed = !formatted.equals(inputs.get(path));
//            if (changed && parameters.setExitIfChanged()) {
//                allOk = false;
//            }
//            if (parameters.inPlace()) {
//                if (!changed) {
//                    continue; // preserve original file
//                }
//                try {
//                    Files.write(path, formatted.getBytes(UTF_8));
//                } catch (IOException e) {
//                    errWriter.println(path + ": could not write file: " + e.getMessage());
//                    allOk = false;
//                    continue;
//                }
//            } else if (parameters.dryRun()) {
//                if (changed) {
//                    outWriter.println(path);
//                }
//            } else {
//                outWriter.write(formatted);
//            }
//        }
//        return allOk ? 0 : 1;
//    }

}
