package com.stratos.model;


public class Token {
     int characterStart ;
     int characterEnd ;
     int lineStart ;
     int lineEnd ;
     TokenType type;
     String lexeme;
     Object literal;

    public Token(int characterStart, int characterEnd, int lineStart, int lineEnd, TokenType type, String lexeme, Object literal) {
        this.characterStart = characterStart;
        this.characterEnd = characterEnd;
        this.lineStart = lineStart;
        this.lineEnd = lineEnd;
        this.type = type;
        this.lexeme = lexeme;
        this.literal = literal;
    }

    @Override
    public String toString() {
        return "Token{" +
                "characterStart=" + characterStart +
                ", characterEnd=" + characterEnd +
                ", lineStart=" + lineStart +
                ", lineEnd=" + lineEnd +
                ", type=" + type +
                ", lexeme='" + lexeme + '\'' +
                ", literal=" + literal +
                '}';
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

    public int getLineStart() {
        return lineStart;
    }

    public void setLineStart(int lineStart) {
        this.lineStart = lineStart;
    }

    public int getLineEnd() {
        return lineEnd;
    }

    public void setLineEnd(int lineEnd) {
        this.lineEnd = lineEnd;
    }

    public TokenType getType() {
        return type;
    }

    public void setType(TokenType type) {
        this.type = type;
    }

    public String getLexeme() {
        return lexeme;
    }

    public void setLexeme(String lexeme) {
        this.lexeme = lexeme;
    }

    public Object getLiteral() {
        return literal;
    }

    public void setLiteral(Object literal) {
        this.literal = literal;
    }
}
