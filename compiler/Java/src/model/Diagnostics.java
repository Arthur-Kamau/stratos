package model;

import model.SeverityLevel;

public class Diagnostics {
    String message;
    SeverityLevel severityLevel;
    int lineNumberStart;
    int lineNumberEnd;
    int characterStart;
    int characterEnd;

    @Override
    public String toString() {
        return "Diagnostics{" +
                "message='" + message + '\'' +
                ", severityLevel=" + severityLevel +
                ", lineNumberStart=" + lineNumberStart +
                ", lineNumberEnd=" + lineNumberEnd +
                ", characterStart=" + characterStart +
                ", characterEnd=" + characterEnd +
                '}';
    }

    public Diagnostics(String message, SeverityLevel severityLevel, int lineNumberStart, int lineNumberEnd, int characterStart, int characterEnd) {
        this.message = message;
        this.severityLevel = severityLevel;
        this.lineNumberStart = lineNumberStart;
        this.lineNumberEnd = lineNumberEnd;
        this.characterStart = characterStart;
        this.characterEnd = characterEnd;
    }

    public String getMessage() {
        return message;
    }

    public void setMessage(String message) {
        this.message = message;
    }

    public SeverityLevel getSeverityLevel() {
        return severityLevel;
    }

    public void setSeverityLevel(SeverityLevel severityLevel) {
        this.severityLevel = severityLevel;
    }

    public int getLineNumberStart() {
        return lineNumberStart;
    }

    public void setLineNumberStart(int lineNumberStart) {
        this.lineNumberStart = lineNumberStart;
    }

    public int getLineNumberEnd() {
        return lineNumberEnd;
    }

    public void setLineNumberEnd(int lineNumberEnd) {
        this.lineNumberEnd = lineNumberEnd;
    }

    public int getCharacterStart() {
        return characterStart;
    }

    public void setCharacterStart(int characterStart) {
        this.characterStart = characterStart;
    }

    public int getCharacterEnd() {
        return characterEnd;
    }

    public void setCharacterEnd(int characterEnd) {
        this.characterEnd = characterEnd;
    }
}
