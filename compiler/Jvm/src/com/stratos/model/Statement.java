package com.stratos.model;



import java.util.List;

public abstract class Statement {


    interface Visitor<R> {
        R visitBlockStmt(Block stmt);
        R visitClassStmt(Class stmt);
        R visitExpressionStmt(ExpressionStatement stmt);
        R visitFunctionStmt(Function stmt);
        R visitIfStmt(If stmt);
        R visitPrintStmt(Print stmt);
        R visitReturnStmt(Return stmt);
        R visitVarStmt(Var stmt);
        R visitWhileStmt(While stmt);
    }

    //> stmt-block
    static class Block extends Statement {
        Block(List<Statement> statements) {
            this.statements = statements;
        }

        @Override
        <R> R accept(Statement.Visitor<R> visitor) {
            return visitor.visitBlockStmt(this);
        }

        @Override
        public String toString() {
            return "Block{" +
                    "statements=" + statements +
                    '}';
        }

        final List<Statement> statements;
    }
//< stmt-block


    //> stmt-class
    static class Class extends Statement {
        Class(Token name,
              Expression.Variable superclass,
              List<Function> methods) {
            this.name = name;
            this.superclass = superclass;
            this.methods = methods;
        }

        @Override
        <R> R accept(Visitor<R> visitor) {
            return visitor.visitClassStmt(this);
        }

        final Token name;
        final Expression.Variable superclass;
        final List<Function> methods;
    }
//< stmt-class

    //> stmt-expression
    static class ExpressionStatement extends Statement {
        ExpressionStatement(Expression expression) {
            this.expression = expression;
        }

        @Override
        <R> R accept(Visitor<R> visitor) {
            return visitor.visitExpressionStmt(this);
        }

        final Expression expression;
    }
    //< stmt-expression
//> stmt-function
    static class Function extends Statement {
        Function(Token name, List<Token> params, List<Statement> body) {
            this.name = name;
            this.params = params;
            this.body = body;
        }

        @Override
        <R> R accept(Visitor<R> visitor) {
            return visitor.visitFunctionStmt(this);
        }

        final Token name;
        final List<Token> params;
        final List<Statement> body;
    }
    //< stmt-function

    //> stmt-if
    static class If extends Statement {
        If(Expression condition, Statement thenBranch, Statement elseBranch) {
            this.condition = condition;
            this.thenBranch = thenBranch;
            this.elseBranch = elseBranch;
        }

        @Override
        <R> R accept(Visitor<R> visitor) {
            return visitor.visitIfStmt(this);
        }

        final Expression condition;
        final Statement thenBranch;
        final Statement elseBranch;
    }
    //< stmt-if

    //> stmt-print
    static class Print extends Statement {
        Print(Expression expression) {
            this.expression = expression;
        }

        @Override
        <R> R accept(Visitor<R> visitor) {
            return visitor.visitPrintStmt(this);
        }

        final Expression expression;
    }
    //< stmt-print
//> stmt-return
    static class Return extends Statement {
        Return(Token keyword, Expression value) {
            this.keyword = keyword;
            this.value = value;
        }

        @Override
        public String toString() {
            return "Return{" +
                    "keyword=" + keyword +
                    ", value=" + value +
                    '}';
        }

        @Override
        <R> R accept(Visitor<R> visitor) {
            return visitor.visitReturnStmt(this);
        }

        final Token keyword;
        final Expression value;
    }
    //< stmt-return
//> stmt-var
    static class Var extends Statement {
        Var(Token name, Expression initializer) {
            this.name = name;
            this.initializer = initializer;
        }

        @Override
        public String toString() {
            return "Var{" +
                    "name=" + name +
                    ", initializer=" + initializer +
                    '}';
        }

        @Override
        <R> R accept(Visitor<R> visitor) {
            return visitor.visitVarStmt(this);
        }

        final Token name;
        final Expression initializer;
    }
    //< stmt-var
//> stmt-while
    static class While extends Statement {
        While(Expression condition, Statement body) {
            this.condition = condition;
            this.body = body;
        }

        @Override
        <R> R accept(Visitor<R> visitor) {
            return visitor.visitWhileStmt(this);
        }

        final Expression condition;
        final Statement body;
    }
//< stmt-while

    abstract <R> R accept(Visitor<R> visitor);
}
