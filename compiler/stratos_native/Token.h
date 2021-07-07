//
// Created by kamau on 2/1/21.
//
#pragma once
#ifndef STRATOS_NATIVE_TOKEN_H
#define STRATOS_NATIVE_TOKEN_H
#include <iostream>
#include <vector>
#include "Node.cpp"

class Token {

public:
    virtual const char*  name() const;

protected:
    Token(){}

};
static std::ostream &operator<<(std::ostream &os, Token const &m) {
    return os << "Token ";
}

class ScopeNode : public Token {
private:
    virtual const char*  name() const override{return "Scope";};
public:
    const std::vector<Token>& children()const{return _children ;}

    template<typename T, typename... Args>
    T& append_child(Args&&... args){
        auto  child =  make<T>(forward<Args>(args)...);
        _children.push_back(move(child));
        return _children.back();
    }

protected:
    ScopeNode(){}

private:
    std::vector<Token> _children;
};



class TokenProgram : public Token {

public:
    explicit TokenProgram(ScopeNode body): _program_body(std::move(body)){

    }
    const ScopeNode& program_body() const{ std::cerr << "boom " << std::endl; return _program_body;}

private:
    virtual const char*  name() const override{return "program";};
    ScopeNode _program_body;
};

class TokenReturnStatement : public Token {
public:
    explicit TokenReturnStatement(Token argument): _argument(std::move(argument)){
    }
    const Token*  argument()const{ return &_argument;}
private:
    virtual const char*  name() const override{return "return";};
    Token _argument;
};

class TokenBlockStatement : public ScopeNode {
public:
    TokenBlockStatement(){}
private:
    virtual const char*  name() const override{return "block statement";};
};

class TokenBinaryExpressionStatement : public  Token {
private:
    virtual const char*  name() const override{return "binary expression";};
};
class TokenFunctionDeclaration : public Token {
public:
    TokenFunctionDeclaration(std::string  name, ScopeNode body):_function_name(std::move(name)), _function_body(std::move(body)){

    }
    std::string function_name() const{ return _function_name;}
    const ScopeNode& function_body() const{ std::cerr << "boom " << std::endl; return _function_body;}


private:
    virtual const char*  name() const override{return "function declaration";};
    std::string _function_name;
    ScopeNode _function_body;
};

class TokenFunctionCall : public Token {
public:
    TokenFunctionCall(std::string  name ):_function_name(std::move(name)){

    }
    std::string function_name() const{ return _function_name;}
private:
    virtual const char*  name() const override{return "function call";};
    std::string _function_name;
};

#endif //STRATOS_NATIVE_TOKEN_H
