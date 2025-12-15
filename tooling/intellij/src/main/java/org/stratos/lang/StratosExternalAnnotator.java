package org.stratos.lang;

import com.intellij.lang.annotation.AnnotationHolder;
import com.intellij.lang.annotation.ExternalAnnotator;
import com.intellij.lang.annotation.HighlightSeverity;
import com.intellij.openapi.util.TextRange;
import com.intellij.psi.PsiFile;
import org.jetbrains.annotations.NotNull;
import org.jetbrains.annotations.Nullable;

import java.io.File;
import java.io.IOException;
import java.nio.charset.StandardCharsets;
import java.nio.file.Files;
import java.util.ArrayList;
import java.util.List;
import java.util.concurrent.TimeUnit;
import java.util.regex.Matcher;
import java.util.regex.Pattern;

public class StratosExternalAnnotator extends ExternalAnnotator<StratosExternalAnnotator.Info, List<StratosExternalAnnotator.Issue>> {

    // Simple container for info needed to run the compiler
    public static class Info {
        String fileContent;
        String filePath;
        public Info(String content, String path) { this.fileContent = content; this.filePath = path; }
    }

    public static class Issue {
        int line;
        int col;
        String msg;
        public Issue(int line, int col, String msg) { this.line = line; this.col = col; this.msg = msg; }
    }

    @Nullable
    @Override
    public Info collectInformation(@NotNull PsiFile file) {
        return new Info(file.getText(), file.getVirtualFile().getPath());
    }

    @Nullable
    @Override
    public List<Issue> doAnnotate(Info info) {
        List<Issue> issues = new ArrayList<>();
        if (info == null) return issues;

        // Path to the compiler executable
        // In a real plugin, this should be configurable via settings
        String compilerPath = "C:\\Users\\ADMIN\\Desktop\\Development\\Projects\\stratos\\compiler\\C++\\build\\stratos.exe";

        File tempFile = null;
        try {
            tempFile = File.createTempFile("stratos_check", ".st");
            Files.write(tempFile.toPath(), info.fileContent.getBytes(StandardCharsets.UTF_8));

            ProcessBuilder pb = new ProcessBuilder(compilerPath, tempFile.getAbsolutePath());
            Process process = pb.start();
            
            // capture stderr
            String stderr = new String(process.getErrorStream().readAllBytes(), StandardCharsets.UTF_8);
            
            process.waitFor(5, TimeUnit.SECONDS);

            // Parse output: [Error] Line:Col: Message
            Pattern pattern = Pattern.compile("\[Error\] (\\d+):(\\d+): (.*)");
            Matcher matcher = pattern.matcher(stderr);

            while (matcher.find()) {
                int line = Integer.parseInt(matcher.group(1));
                int col = Integer.parseInt(matcher.group(2));
                String msg = matcher.group(3);
                issues.add(new Issue(line, col, msg));
            }

        } catch (IOException | InterruptedException e) {
            // e.printStackTrace();
        } finally {
            if (tempFile != null) {
                tempFile.delete();
            }
        }

        return issues;
    }

    @Override
    public void apply(@NotNull PsiFile file, List<Issue> issues, @NotNull AnnotationHolder holder) {
        if (issues == null) return;

        // Map line/col to TextRange
        // Note: Compiler uses 1-based indexing, usually. 
        // Need to convert to 0-based char offset.
        
        String text = file.getText();
        String[] lines = text.split("\n");

        for (Issue issue : issues) {
            int lineIdx = issue.line - 1;
            int colIdx = issue.col - 1;

            if (lineIdx >= 0 && lineIdx < lines.length) {
                // Calculate offset
                int offset = 0;
                for (int i = 0; i < lineIdx; i++) {
                    offset += lines[i].length() + 1; // +1 for newline
                }
                offset += colIdx;

                if (offset < text.length()) {
                    // Highlight until end of token or line, rough heuristic
                    int endOffset = offset + 1;
                    while (endOffset < text.length() && Character.isJavaIdentifierPart(text.charAt(endOffset))) {
                        endOffset++;
                    }
                    
                    TextRange range = new TextRange(offset, Math.min(endOffset, text.length()));
                    holder.createAnnotation(HighlightSeverity.ERROR, range, issue.msg);
                }
            }
        }
    }
}