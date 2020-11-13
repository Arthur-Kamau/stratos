package com.stratos.model;


public class Token {
    final int characterStart ;
    final int characterEnd ;
    final int lineStart ;
    final int lineEnd ;
    final TokenType type;
    final String lexeme;
    final Object literal;

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
}
