package com.stratos.analysis.Parser;
import com.stratos.model.Expression;
import com.stratos.model.Statement;
import com.stratos.model.Token;
import com.stratos.model.TokenType;



import java.util.ArrayList;
import java.util.Arrays;
import java.util.List;

import static  com.stratos.model.TokenType.*;


public class Parser {

    private static class ParseError extends RuntimeException {}
    private final List<Token> tokens;
    private int current = 0;

    public Parser(List<Token> tokens) {
        this.tokens = tokens;
    }
    public List<Statement> parse(){
        List<Statement> statements = new ArrayList<>();
        while (!isAtEnd()) {
/* Statements and State parse < Statements and State parse-declaration
      statements.add(statement());
*/
//> parse-declaration
            statements.add(declaration());
//< parse-declaration
        }

        return statements;
    }






    //< Statements and State parse
//> expression
    private Expression expression() {
/* Parsing Expressionessions expression < Statements and State expression
    return equality();
*/
//> Statements and State expression
        return assignment();
//< Statements and State expression
    }
    //< expression
//> Statements and State declaration
    private Statement declaration() {
        try {
//> Classes match-class
            if (match(CLASS)) return classDeclaration();
//< Classes match-class
//> Functions match-fun
            if (match(FUNCTION)) return function("function");
//< Functions match-fun
            if (match(VAR)) return varDeclaration();

            return statement();
        } catch (Parser.ParseError error) {
            synchronize();
            return null;
        }
    }

    //< Statements and State declaration
//> Classes parse-class-declaration
    private Statement classDeclaration() {
        Token name = consume(IDENTIFIER, "Expect class name.");
//> Inheritance parse-superclass

        Expression.Variable superclass = null;
        if (match(LESS)) {
            consume(IDENTIFIER, "Expect superclass name.");
            superclass = new Expression.Variable(previous());
        }

//< Inheritance parse-superclass
        consume(LEFT_BRACE, "Expect '{' before class body.");

        List<Statement.Function> methods = new ArrayList<>();
        while (!check(RIGHT_BRACE) && !isAtEnd()) {
            methods.add(function("method"));
        }

        consume(RIGHT_BRACE, "Expect '}' after class body.");

/* Classes parse-class-declaration < Inheritance construct-class-ast
    return new Statement.Class(name, methods);
*/
//> Inheritance construct-class-ast
        return new Statement.Class(name, superclass, methods);
//< Inheritance construct-class-ast
    }
    //< Classes parse-class-declaration
//> Statements and State parse-statement
    private Statement statement() {
//> Control Flow match-for
        if (match(FOR)) return forStatement();
//< Control Flow match-for
//> Control Flow match-if
        if (match(IF)) return ifStatement();
//< Control Flow match-if
        if (match(PRINT)) return printStatement();
//> Functions match-return
        if (match(RETURN)) return returnStatement();
//< Functions match-return
//> Control Flow match-while
        if (match(WHILE)) return whileStatement();
//< Control Flow match-while
//> parse-block
        if (match(LEFT_BRACE)) return new Statement.Block(block());
//< parse-block

        return expressionStatement();
    }
    //< Statements and State parse-statement
//> Control Flow for-statement
    private Statement forStatement() {
        consume(LEFT_PAREN, "Expect '(' after 'for'.");

/* Control Flow for-statement < Control Flow for-initializer
    // More here...
*/
//> for-initializer
        Statement initializer;
        if (match(SEMICOLON)) {
            initializer = null;
        } else if (match(VAR)) {
            initializer = varDeclaration();
        } else {
            initializer = expressionStatement();
        }
//< for-initializer
//> for-condition

        Expression condition = null;
        if (!check(SEMICOLON)) {
            condition = expression();
        }
        consume(SEMICOLON, "Expect ';' after loop condition.");
//< for-condition
//> for-increment

        Expression increment = null;
        if (!check(RIGHT_PAREN)) {
            increment = expression();
        }
        consume(RIGHT_PAREN, "Expect ')' after for clauses.");
//< for-increment
//> for-body
        Statement body = statement();

//> for-desugar-increment
        if (increment != null) {
            body = new Statement.Block(
                    Arrays.asList(
                            body,
                            new Statement.Expressionession(increment)));
        }

//< for-desugar-increment
//> for-desugar-condition
        if (condition == null) condition = new Expression.Literal(true);
        body = new Statement.While(condition, body);

//< for-desugar-condition
//> for-desugar-initializer
        if (initializer != null) {
            body = new Statement.Block(Arrays.asList(initializer, body));
        }

//< for-desugar-initializer
        return body;
//< for-body
    }
    //< Control Flow for-statement
//> Control Flow if-statement
    private Statement ifStatement() {
        consume(LEFT_PAREN, "Expect '(' after 'if'.");
        Expression condition = expression();
        consume(RIGHT_PAREN, "Expect ')' after if condition."); // [parens]

        Statement thenBranch = statement();
        Statement elseBranch = null;
        if (match(ELSE)) {
            elseBranch = statement();
        }

        return new Statement.If(condition, thenBranch, elseBranch);
    }
    //< Control Flow if-statement
//> Statements and State parse-print-statement
    private Statement printStatement() {
        Expression value = expression();
        consume(SEMICOLON, "Expect ';' after value.");
        return new Statement.Print(value);
    }
    //< Statements and State parse-print-statement
//> Functions parse-return-statement
    private Statement returnStatement() {
        Token keyword = previous();
        Expression value = null;
        if (!check(SEMICOLON)) {
            value = expression();
        }

        consume(SEMICOLON, "Expect ';' after return value.");
        return new Statement.Return(keyword, value);
    }
    //< Functions parse-return-statement
//> Statements and State parse-var-declaration
    private Statement varDeclaration() {
        Token name = consume(IDENTIFIER, "Expect variable name.");

        Expression initializer = null;
        if (match(EQUAL)) {
            initializer = expression();
        }

        consume(SEMICOLON, "Expect ';' after variable declaration.");
        return new Statement.Var(name, initializer);
    }
    //< Statements and State parse-var-declaration
//> Control Flow while-statement
    private Statement whileStatement() {
        consume(LEFT_PAREN, "Expect '(' after 'while'.");
        Expression condition = expression();
        consume(RIGHT_PAREN, "Expect ')' after condition.");
        Statement body = statement();

        return new Statement.While(condition, body);
    }
    //< Control Flow while-statement
//> Statements and State parse-expression-statement
    private Statement expressionStatement() {
        Expression expr = expression();
        consume(SEMICOLON, "Expect ';' after expression.");
        return new Statement.Expressionession(expr);
    }
    //< Statements and State parse-expression-statement
//> Functions parse-function
    private Statement.Function function(String kind) {
        Token name = consume(IDENTIFIER, "Expect " + kind + " name.");
//> parse-parameters
        consume(LEFT_PAREN, "Expect '(' after " + kind + " name.");
        List<Token> parameters = new ArrayList<>();
        if (!check(RIGHT_PAREN)) {
            do {
                if (parameters.size() >= 255) {
                    error(peek(), "Can't have more than 255 parameters.");
                }

                parameters.add(
                        consume(IDENTIFIER, "Expect parameter name."));
            } while (match(COMMA));
        }
        consume(RIGHT_PAREN, "Expect ')' after parameters.");
//< parse-parameters
//> parse-body

        consume(LEFT_BRACE, "Expect '{' before " + kind + " body.");
        List<Statement> body = block();
        return new Statement.Function(name, parameters, body);
//< parse-body
    }
    //< Functions parse-function
//> Statements and State block
    private List<Statement> block() {
        List<Statement> statements = new ArrayList<>();

        while (!check(RIGHT_BRACE) && !isAtEnd()) {
            statements.add(declaration());
        }

        consume(RIGHT_BRACE, "Expect '}' after block.");
        return statements;
    }
    //< Statements and State block
//> Statements and State parse-assignment
    private Expression assignment() {
/* Statements and State parse-assignment < Control Flow or-in-assignment
    Expression expr = equality();
*/
//> Control Flow or-in-assignment
        Expression expr = or();
//< Control Flow or-in-assignment

        if (match(EQUAL)) {
            Token equals = previous();
            Expression value = assignment();

            if (expr instanceof Expression.Variable) {
                Token name = ((Expression.Variable)expr).name;
                return new Expression.Assign(name, value);
//> Classes assign-set
            } else if (expr instanceof Expression.Get) {
                Expression.Get get = (Expression.Get)expr;
                return new Expression.Set(get.object, get.name, value);
//< Classes assign-set
            }

            error(equals, "Invalid assignment target."); // [no-throw]
        }

        return expr;
    }
    //< Statements and State parse-assignment
//> Control Flow or
    private Expression or() {
        Expression expr = and();

        while (match(OR)) {
            Token operator = previous();
            Expression right = and();
            expr = new Expression.Logical(expr, operator, right);
        }

        return expr;
    }
    //< Control Flow or
//> Control Flow and
    private Expression and() {
        Expression expr = equality();

        while (match(AND)) {
            Token operator = previous();
            Expression right = equality();
            expr = new Expression.Logical(expr, operator, right);
        }

        return expr;
    }
    //< Control Flow and
//> equality
    private Expression equality() {
        Expression expr = comparison();

        while (match(BANG_EQUAL, EQUAL_EQUAL)) {
            Token operator = previous();
            Expression right = comparison();
            expr = new Expression.Binary(expr, operator, right);
        }

        return expr;
    }
    //< equality
//> comparison
    private Expression comparison() {
        Expression expr = term();

        while (match(GREATER, GREATER_EQUAL, LESS, LESS_EQUAL)) {
            Token operator = previous();
            Expression right = term();
            expr = new Expression.Binary(expr, operator, right);
        }

        return expr;
    }
    //< comparison
//> term
    private Expression term() {
        Expression expr = factor();

        while (match(MINUS, PLUS)) {
            Token operator = previous();
            Expression right = factor();
            expr = new Expression.Binary(expr, operator, right);
        }

        return expr;
    }
    //< term
//> factor
    private Expression factor() {
        Expression expr = unary();

        while (match(SLASH, STAR)) {
            Token operator = previous();
            Expression right = unary();
            expr = new Expression.Binary(expr, operator, right);
        }

        return expr;
    }
    //< factor
//> unary
    private Expression unary() {
        if (match(BANG, MINUS)) {
            Token operator = previous();
            Expression right = unary();
            return new Expression.Unary(operator, right);
        }

/* Parsing Expressionessions unary < Functions unary-call
    return primary();
*/
//> Functions unary-call
        return call();
//< Functions unary-call
    }
    //< unary
//> Functions finish-call
    private Expression finishCall(Expression callee) {
        List<Expression> arguments = new ArrayList<>();
        if (!check(RIGHT_PAREN)) {
            do {
//> check-max-arity
                if (arguments.size() >= 255) {
                    error(peek(), "Can't have more than 255 arguments.");
                }
//< check-max-arity
                arguments.add(expression());
            } while (match(COMMA));
        }

        Token paren = consume(RIGHT_PAREN,
                "Expect ')' after arguments.");

        return new Expression.Call(callee, paren, arguments);
    }
    //< Functions finish-call
//> Functions call
    private Expression call() {
        Expression expr = primary();

        while (true) { // [while-true]
            if (match(LEFT_PAREN)) {
                expr = finishCall(expr);
//> Classes parse-property
            } else if (match(DOT)) {
                Token name = consume(IDENTIFIER,
                        "Expect property name after '.'.");
                expr = new Expression.Get(expr, name);
//< Classes parse-property
            } else {
                break;
            }
        }

        return expr;
    }
//< Functions call
//> primary

    private Expression primary() {
        if (match(FALSE)) return new Expression.Literal(false);
        if (match(TRUE)) return new Expression.Literal(true);
        if (match(NIL)) return new Expression.Literal(null);

        if (match(NUMBER, STRING)) {
            return new Expression.Literal(previous().literal);
        }
//> Inheritance parse-super

        if (match(SUPER)) {
            Token keyword = previous();
            consume(DOT, "Expect '.' after 'super'.");
            Token method = consume(IDENTIFIER,
                    "Expect superclass method name.");
            return new Expression.Super(keyword, method);
        }
//< Inheritance parse-super
//> Classes parse-this

        if (match(THIS)) return new Expression.This(previous());
//< Classes parse-this
//> Statements and State parse-identifier

        if (match(IDENTIFIER)) {
            return new Expression.Variable(previous());
        }
//< Statements and State parse-identifier

        if (match(LEFT_PAREN)) {
            Expression expr = expression();
            consume(RIGHT_PAREN, "Expect ')' after expression.");
            return new Expression.Grouping(expr);
        }
//> primary-error

        throw error(peek(), "Expect expression.");
//< primary-error
    }
//< primary
//> match
    
    
    
    
    
    
    
    
    
    
    
    



    //> match
    private boolean match(TokenType... types) {
        for (TokenType type : types) {
            if (check(type)) {
                advance();
                return true;
            }
        }

        return false;
    }
    //< match
//> consume
    private Token consume(TokenType type, String message) {
        if (check(type)) return advance();

        throw error(peek(), message);
    }
    //< consume
//> check
    private boolean check(TokenType type) {
        if (isAtEnd()) return false;
        return peek().getType() == type;
    }
    //< check
//> advance
    private Token advance() {
        if (!isAtEnd()) current++;
        return previous();
    }
    //< advance
//> utils
    private boolean isAtEnd() {
        return peek().getType() == EOF;
    }

    private Token peek() {
        return tokens.get(current);
    }

    private Token previous() {
        return tokens.get(current - 1);
    }
    //< utils
//> error
    private ParseError error(Token token, String message) {
        error(token, message);
        return new ParseError();
    }
    //< error
//> synchronize
    private void synchronize() {
        advance();

        while (!isAtEnd()) {
            if (previous().getType() == SEMICOLON) return;

            switch (peek().getType()) {
                case CLASS:
                case FUNCTION:
                case VAR:
                case FOR:
                case IF:
                case WHILE:
                case PRINT:
                case RETURN:
                    return;
            }

            advance();
        }
    }
//< synchronize
}
