#include "stratos/Parser.h"
#include "stratos/Lexer.h"
#include <iostream>
#include <sstream>

namespace stratos {

Parser::Parser(const std::vector<Token>& tokens) : tokens(tokens) {}

std::vector<std::unique_ptr<Stmt>> Parser::parse() {
    std::vector<std::unique_ptr<Stmt>> statements;
    while (!isAtEnd()) {
        try {
            auto stmt = declaration();
            statements.push_back(std::move(stmt));
        } catch (ParseError& error) {
            std::cerr << "[Error] " << error.line << ":" << error.column << ": " << error.what() << std::endl;
            synchronize();
        }
    }
    return statements;
}

std::unique_ptr<Expr> Parser::parseExpression() {
    return expression();
}

// --- Declarations ---

std::unique_ptr<Stmt> Parser::declaration() {
    try {
        // Check for doc comment before declaration
        consumeDocComment();

        // std::cout << "[Parser::declaration] Current token: " << peek().lexeme << " (type " << static_cast<int>(peek().type) << ")" << std::endl;

        // Check for common mistake: using 'function' instead of 'fn'
        if (check(TokenType::IDENTIFIER) && peek().lexeme == "function") {
            if (current + 1 < tokens.size() && tokens[current + 1].type == TokenType::IDENTIFIER) {
                throw ParseError("Stratos uses 'fn' to declare functions, not 'function'.", peek().line, peek().column);
            }
        }

        bool isPublic = match({TokenType::PUB});

        if (match({TokenType::VAR, TokenType::VAL})) {
            // std::cout << "[Parser::declaration] Matched VAR/VAL, calling varDeclaration()" << std::endl;
            return varDeclaration(isPublic);
        }
        if (match({TokenType::ASYNC})) {
            consume(TokenType::FN, "Expect 'fn' after 'async'.");
            return fnDeclaration("function", true, true);
        }
        if (match({TokenType::FN})) return fnDeclaration("function", true); // Top level functions are public by default (exported)
        if (match({TokenType::CLASS, TokenType::INTERFACE})) return classDeclaration();
        if (match({TokenType::STRUCT})) return structDeclaration();
        if (match({TokenType::ENUM})) return enumDeclaration();
        if (match({TokenType::TYPE})) return typeAliasDeclaration();
        if (match({TokenType::PACKAGE})) return packageDeclaration();
        if (match({TokenType::USE})) return useStatement();

        // Clear pending doc comment if not used
        pendingDocComment.reset();

        return statement();
    } catch (ParseError& error) {
        // Silently recover from parse errors (e.g., generic type syntax issues)
        synchronize();
        return nullptr;
    }
}

std::unique_ptr<Stmt> Parser::varDeclaration(bool isPublic) {
    bool isMutable = (previous().type == TokenType::VAR);

    // Check for destructuring declaration: val (a, b, c) = expr
    if (match({TokenType::LEFT_PAREN})) {
        std::vector<Token> names;

        // Parse the list of names
        do {
            Token varName = consume(TokenType::IDENTIFIER, "Expect variable name in destructuring.");
            names.push_back(varName);
        } while (match({TokenType::COMMA}));

        consume(TokenType::RIGHT_PAREN, "Expect ')' after destructuring names.");
        consume(TokenType::EQUAL, "Expect '=' after destructuring declaration.");

        std::unique_ptr<Expr> initializer = expression();
        consume(TokenType::SEMICOLON, "Expect ';' after destructuring declaration.");

        return std::make_unique<DestructuringDecl>(isMutable, std::move(names), std::move(initializer), isPublic);
    }

    // Regular variable declaration
    Token name = consume(TokenType::IDENTIFIER, "Expect variable name.");

    std::string typeName = "";
    if (match({TokenType::COLON})) {
        typeName = parseType();
    }

    std::unique_ptr<Expr> initializer = nullptr;
    if (match({TokenType::EQUAL})) {
        initializer = expression();
    }

    consume(TokenType::SEMICOLON, "Expect ';' after variable declaration.");
    auto var = std::make_unique<VarDecl>(name, typeName, std::move(initializer), isMutable, isPublic);
    var->documentation = takePendingDoc();
    return var;
}

std::unique_ptr<Stmt> Parser::fnDeclaration(const std::string& kind, bool isPublic, bool isAsync) {
    // For methods, allow keywords like 'get', 'set', 'use', 'delete' as method names
    Token name;
    if (kind == "method") {
        // Accept either an identifier or certain keywords as method names
        if (check(TokenType::IDENTIFIER)) {
            name = advance();
        } else if (check(TokenType::USE) || check(TokenType::FOR) || check(TokenType::IF) ||
                   check(TokenType::WHILE) || check(TokenType::RETURN) || check(TokenType::VAR) ||
                   check(TokenType::VAL) || check(TokenType::TRUE) || check(TokenType::FALSE)) {
            // Convert the keyword token to act as an identifier for method name
            name = advance();
            name.type = TokenType::IDENTIFIER;  // Treat it as identifier for AST purposes
        } else {
            name = consume(TokenType::IDENTIFIER, "Expect method name.");
        }
    } else {
        name = consume(TokenType::IDENTIFIER, "Expect " + kind + " name.");
    }

    // Skip generic type parameters if present (e.g., <T, E>)
    if (match({TokenType::LESS})) {
        int depth = 1;
        while (depth > 0 && !isAtEnd()) {
            if (match({TokenType::LESS})) {
                depth++;
            } else if (match({TokenType::GREATER})) {
                depth--;
            } else {
                advance();
            }
        }
    }

    consume(TokenType::LEFT_PAREN, "Expect '(' after " + kind + " name.");

    std::vector<Parameter> parameters;

    if (!check(TokenType::RIGHT_PAREN)) {
        do {
            bool isVariadic = false;
            if (match({TokenType::DOT_DOT_DOT})) {
                isVariadic = true;
            }

            Token paramName = consume(TokenType::IDENTIFIER, "Expect parameter name.");
            
            std::string type = "any";
            if (match({TokenType::COLON})) {
                type = parseType();
            }

            std::unique_ptr<Expr> defaultValue = nullptr;
            if (match({TokenType::EQUAL})) {
                defaultValue = expression();
            }

            parameters.push_back(Parameter(paramName, type, std::move(defaultValue), isVariadic));
        } while (match({TokenType::COMMA}));
    }
    consume(TokenType::RIGHT_PAREN, "Expect ')' after parameters.");

    std::string returnType = "void";
    if (check(TokenType::IDENTIFIER) || check(TokenType::INT) || check(TokenType::DOUBLE) ||
        check(TokenType::STRING) || check(TokenType::BOOL) || check(TokenType::VOID) ||
        check(TokenType::ASYNC)) {
        // Optional return type parsing without colon if it's just the type
        if (!check(TokenType::LEFT_BRACE) && !check(TokenType::SEMICOLON)) {
             returnType = parseType();
        }
    }

    // Check for interface method (no body, just semicolon)
    if (check(TokenType::SEMICOLON)) {
        consume(TokenType::SEMICOLON, "Expect ';' after interface method signature.");
        // Return function declaration with null body (interface method)
        auto func = std::make_unique<FunctionDecl>(name, std::move(parameters), returnType, nullptr, isPublic, isAsync);
        func->documentation = takePendingDoc();
        return func;
    }

    consume(TokenType::LEFT_BRACE, "Expect '{' before " + kind + " body.");
    std::vector<std::unique_ptr<Stmt>> body;
    while (!check(TokenType::RIGHT_BRACE) && !isAtEnd()) {
        body.push_back(declaration());
    }
    consume(TokenType::RIGHT_BRACE, "Expect '}' after " + kind + " body.");

    auto bodyPtr = std::make_unique<std::vector<std::unique_ptr<Stmt>>>(std::move(body));
    auto func = std::make_unique<FunctionDecl>(name, std::move(parameters), returnType, std::move(bodyPtr), isPublic, isAsync);
    func->documentation = takePendingDoc();
    return func;
}

std::unique_ptr<Stmt> Parser::classDeclaration() {
    Token name = consume(TokenType::IDENTIFIER, "Expect class name.");

    // Skip generic type parameters if present (e.g., <T, E>)
    if (match({TokenType::LESS})) {
        int depth = 1;
        while (depth > 0 && !isAtEnd()) {
            if (match({TokenType::LESS})) {
                depth++;
            } else if (match({TokenType::GREATER})) {
                depth--;
            } else {
                advance();
            }
        }
    }

    std::unique_ptr<VariableExpr> superclass = nullptr;
    if (match({TokenType::COLON})) {
        consume(TokenType::IDENTIFIER, "Expect superclass name.");
        superclass = std::make_unique<VariableExpr>(previous());
    }

    consume(TokenType::LEFT_BRACE, "Expect '{' before class body.");

    std::vector<std::unique_ptr<Stmt>> methods;
    while (!check(TokenType::RIGHT_BRACE) && !isAtEnd()) {
        bool isPublic = false;
        if (match({TokenType::PUB})) {
            isPublic = true;
        }

        if (match({TokenType::VAR, TokenType::VAL})) {
            methods.push_back(varDeclaration(isPublic));
        } else if (match({TokenType::FN})) {
             methods.push_back(fnDeclaration("method", isPublic));
        } else if (match({TokenType::CONSTRUCTOR})) {
            // Parse constructor as a special function named "constructor"
            Token ctorName = previous(); // "constructor" keyword
            consume(TokenType::LEFT_PAREN, "Expect '(' after constructor.");

            std::vector<Parameter> parameters;

            if (!check(TokenType::RIGHT_PAREN)) {
                do {
                    Token param = consume(TokenType::IDENTIFIER, "Expect parameter name.");
                    
                    std::string paramType = "any";
                    if (match({TokenType::COLON})) {
                        paramType = parseType();
                    }
                    
                    parameters.push_back(Parameter(param, paramType, nullptr, false));
                } while (match({TokenType::COMMA}));
            }

            consume(TokenType::RIGHT_PAREN, "Expect ')' after parameters.");

            // Constructors don't have return types
            std::string returnType = "void";

            // Parse constructor body
            consume(TokenType::LEFT_BRACE, "Expect '{' before constructor body.");
            auto body = std::make_unique<std::vector<std::unique_ptr<Stmt>>>();
            while (!check(TokenType::RIGHT_BRACE) && !isAtEnd()) {
                body->push_back(declaration());
            }
            consume(TokenType::RIGHT_BRACE, "Expect '}' after constructor body.");

            // Create a FunctionDecl with name "constructor"
            methods.push_back(std::make_unique<FunctionDecl>(ctorName, std::move(parameters), returnType, std::move(body), true));
        } else {
             // Skip unknown things inside class for now to avoid infinite loops
             advance();
        }
    }
    consume(TokenType::RIGHT_BRACE, "Expect '}' after class body.");

    auto cls = std::make_unique<ClassDecl>(name, std::move(superclass), std::move(methods));
    cls->documentation = takePendingDoc();
    return cls;
}

std::unique_ptr<Stmt> Parser::structDeclaration() {
    Token name = consume(TokenType::IDENTIFIER, "Expect struct name.");
    consume(TokenType::LEFT_BRACE, "Expect '{' before struct body.");

    std::vector<std::unique_ptr<Stmt>> methods; // Structs fields are VarDecls

    while (!check(TokenType::RIGHT_BRACE) && !isAtEnd()) {
        bool isPublic = true; // Struct fields are public by default
        if (match({TokenType::PUB})) {
            isPublic = true;
        }

        Token fieldName = consume(TokenType::IDENTIFIER, "Expect field name.");
        consume(TokenType::COLON, "Expect ':' after field name.");
        std::string typeName = parseType();
        consume(TokenType::SEMICOLON, "Expect ';' after field declaration.");

        // Create a VarDecl for the field
        // Fields in structs are mutable by default in this implementation context
        auto varDecl = std::make_unique<VarDecl>(fieldName, typeName, nullptr, true, isPublic); 
        methods.push_back(std::move(varDecl));
    }

    consume(TokenType::RIGHT_BRACE, "Expect '}' after struct body.");

    // Reuse ClassDecl for structs as they are handled similarly in runtime
    return std::make_unique<ClassDecl>(name, nullptr, std::move(methods));
}

std::unique_ptr<Stmt> Parser::enumDeclaration() {
    Token name = consume(TokenType::IDENTIFIER, "Expect enum name.");

    consume(TokenType::LEFT_BRACE, "Expect '{' before enum body.");

    std::vector<Token> values;
    while (!check(TokenType::RIGHT_BRACE) && !isAtEnd()) {
        Token valueName = consume(TokenType::IDENTIFIER, "Expect enum value name.");
        values.push_back(valueName);

        // Allow optional comma after each value
        if (!check(TokenType::RIGHT_BRACE)) {
            if (!match({TokenType::COMMA})) {
                // Comma is optional before closing brace or between values
                // If next token is not }, it's an error
                if (!check(TokenType::RIGHT_BRACE)) {
                    throw ParseError("Expect ',' or '}' after enum value.", peek().line, peek().column);
                }
            }
        }
    }

    consume(TokenType::RIGHT_BRACE, "Expect '}' after enum body.");

    auto enumDecl = std::make_unique<EnumDecl>(name, std::move(values));
    enumDecl->documentation = takePendingDoc();
    return enumDecl;
}

std::unique_ptr<Stmt> Parser::typeAliasDeclaration() {
    // type Handler = Function<Request, Response, void>;
    Token name = consume(TokenType::IDENTIFIER, "Expect type alias name.");
    consume(TokenType::EQUAL, "Expect '=' after type alias name.");
    std::string aliasedType = parseType();
    consume(TokenType::SEMICOLON, "Expect ';' after type alias.");

    auto typeAlias = std::make_unique<TypeAliasDecl>(name, aliasedType);
    typeAlias->documentation = takePendingDoc();
    return typeAlias;
}

std::unique_ptr<Stmt> Parser::packageDeclaration() {
    Token name = consume(TokenType::IDENTIFIER, "Expect package name.");
    // Package declarations don't use braces in Go-style
    // Just consume semicolon or newline
    // For now, keep compatibility with old block-style
    if (check(TokenType::LEFT_BRACE)) {
        consume(TokenType::LEFT_BRACE, "Expect '{' before package body.");
        std::vector<std::unique_ptr<Stmt>> body;
        while (!check(TokenType::RIGHT_BRACE) && !isAtEnd()) {
            body.push_back(declaration());
        }
        consume(TokenType::RIGHT_BRACE, "Expect '}' after package body.");
        auto pkg = std::make_unique<PackageDecl>(name, std::move(body));
        pkg->documentation = takePendingDoc();
        return pkg;
    }
    // Go-style: package declaration at top, no braces
    // Consume the semicolon if present
    if (check(TokenType::SEMICOLON)) {
        advance(); // consume the semicolon
    }
    
    // Parse the rest of the file as the package body
    std::vector<std::unique_ptr<Stmt>> body;
    while (!isAtEnd()) {
        body.push_back(declaration());
    }

    auto pkg = std::make_unique<PackageDecl>(name, std::move(body));
    pkg->documentation = takePendingDoc();
    return pkg;
}

std::unique_ptr<Stmt> Parser::useStatement() {
    Token moduleName;
    // Allow 'async' keyword as a module name
    if (check(TokenType::ASYNC)) {
        moduleName = advance();
    } else {
        moduleName = consume(TokenType::IDENTIFIER, "Expect module name after 'use'.");
    }

    // Consume the semicolon if present (Go-style allows omitting it)
    if (check(TokenType::SEMICOLON)) {
        advance();
    }

    return std::make_unique<UseStmt>(moduleName);
}

// --- Statements ---

std::unique_ptr<Stmt> Parser::statement() {
    if (match({TokenType::IF})) return ifStatement();
    if (match({TokenType::WHILE})) return whileStatement();
    if (match({TokenType::FOR})) return forStatement();
    if (match({TokenType::RETURN})) return returnStatement();
    if (match({TokenType::BREAK})) return breakStatement();
    if (match({TokenType::CONTINUE})) return continueStatement();
    if (match({TokenType::DEFER})) return deferStatement();
    // if (match({TokenType::WHEN})) return whenStatement(); // Handled as expression statement now
    if (match({TokenType::LEFT_BRACE})) return block();

    return expressionStatement();
}

std::unique_ptr<Expr> Parser::whenExpression() {
    consume(TokenType::LEFT_PAREN, "Expect '(' after 'when'.");
    
    std::unique_ptr<Expr> condition = nullptr;
    // Check if we have a condition or if it's 'when ()' (which acts like switch true)
    // Or maybe 'when ( expr )'
    // Stratos syntax: when (x) { ... } or when { ... } (if we supported no parens)
    // But typical is when (x) { case 1 -> ... }
    
    if (!check(TokenType::RIGHT_PAREN)) {
        condition = expression();
    }
    consume(TokenType::RIGHT_PAREN, "Expect ')' after when condition.");
    
    consume(TokenType::LEFT_BRACE, "Expect '{' before when body.");
    
    std::vector<WhenCase> cases;
    
    while (!check(TokenType::RIGHT_BRACE) && !isAtEnd()) {
        // Parse case
        // Case syntax: 
        // 1. value -> statement/block
        // 2. else -> statement/block
        // 3. value1, value2 -> ...
        
        std::vector<std::unique_ptr<Expr>> caseConditions;
        bool isElse = false;
        
        if (match({TokenType::ELSE})) {
            isElse = true;
            consume(TokenType::ARROW, "Expect '->' after 'else'.");
        } else {
            // Parse conditions
            do {
                caseConditions.push_back(expression());
            } while (match({TokenType::COMMA}));
            
            consume(TokenType::ARROW, "Expect '->' after case conditions.");
        }
        
        // Parse body
        std::unique_ptr<Stmt> body;
        if (match({TokenType::LEFT_BRACE})) {
            body = block();
        } else {
            // Single statement/expression
            // If it's an expression, wrap in return/expression stmt?
            // "when" is an expression, so the body should evaluate to something.
            // If we parse as statement(), it covers ExpressionStmt.
            // But we need to capture the value if it's being assigned.
            // For now, let's parse as statement. Interpreter handles returning value from block/stmt.
            body = statement();
        }
        
        cases.push_back(WhenCase(std::move(caseConditions), std::move(body), isElse));
    }
    
    consume(TokenType::RIGHT_BRACE, "Expect '}' after when body.");
    
    return std::make_unique<WhenExpr>(std::move(condition), std::move(cases));
}

std::unique_ptr<Expr> Parser::parseInterpolatedString(const std::string& template_str) {
    std::vector<InterpolatedPart> parts;
    std::string currentLiteral;
    size_t i = 0;

    while (i < template_str.size()) {
        if (template_str[i] == '$' && i + 1 < template_str.size()) {
            // Check for interpolation
            if (template_str[i + 1] == '{') {
                // ${expression} syntax
                if (!currentLiteral.empty()) {
                    parts.push_back(InterpolatedPart(currentLiteral));
                    currentLiteral.clear();
                }

                i += 2; // Skip "${"
                std::string exprStr;
                int braceCount = 1;
                while (i < template_str.size() && braceCount > 0) {
                    if (template_str[i] == '{') braceCount++;
                    else if (template_str[i] == '}') braceCount--;
                    if (braceCount > 0) exprStr += template_str[i];
                    i++;
                }

                // Parse the expression string
                Lexer exprLexer(exprStr, "<interpolation>");
                auto exprTokens = exprLexer.scanTokens();
                Parser exprParser(exprTokens);
                auto expr = exprParser.parseExpression();
                if (expr) {
                    parts.push_back(InterpolatedPart(std::move(expr)));
                }
            } else if (isalpha(template_str[i + 1]) || template_str[i + 1] == '_') {
                // $identifier syntax
                if (!currentLiteral.empty()) {
                    parts.push_back(InterpolatedPart(currentLiteral));
                    currentLiteral.clear();
                }

                i++; // Skip "$"
                std::string identifier;
                while (i < template_str.size() && (isalnum(template_str[i]) || template_str[i] == '_')) {
                    identifier += template_str[i];
                    i++;
                }

                // Create a VariableExpr for the identifier
                Token varToken;
                varToken.type = TokenType::IDENTIFIER;
                varToken.lexeme = identifier;
                varToken.line = previous().line;
                varToken.column = previous().column;
                varToken.file = previous().file;
                parts.push_back(InterpolatedPart(std::make_unique<VariableExpr>(varToken)));
            } else {
                currentLiteral += template_str[i];
                i++;
            }
        } else {
            currentLiteral += template_str[i];
            i++;
        }
    }

    if (!currentLiteral.empty()) {
        parts.push_back(InterpolatedPart(currentLiteral));
    }

    return std::make_unique<InterpolatedStringExpr>(std::move(parts));
}

std::unique_ptr<Stmt> Parser::ifStatement() {
    std::unique_ptr<Expr> condition = expression();

    std::unique_ptr<Stmt> thenBranch = statement();
    std::unique_ptr<Stmt> elseBranch = nullptr;

    if (match({TokenType::ELSE})) {
        elseBranch = statement();
    }

    return std::make_unique<IfStmt>(std::move(condition), std::move(thenBranch), std::move(elseBranch));
}

std::unique_ptr<Stmt> Parser::whileStatement() {
    std::unique_ptr<Expr> condition = expression();
    std::unique_ptr<Stmt> body = statement();
    return std::make_unique<WhileStmt>(std::move(condition), std::move(body));
}

std::unique_ptr<Stmt> Parser::forStatement() {
    // Parse: for (val|var) variable (: type)? in iterable { body }

    // Check for val or var
    bool isMutable = false;
    if (match({TokenType::VAR})) {
        isMutable = true;
    } else if (match({TokenType::VAL})) {
        isMutable = false;
    } else {
        throw ParseError("Expect 'val' or 'var' after 'for'", peek().line, peek().column);
    }

    // Get variable name
    Token variable = consume(TokenType::IDENTIFIER, "Expect variable name in for loop.");

    // Optional type annotation
    std::string varType = "";
    if (match({TokenType::COLON})) {
        varType = parseType();
    }

    // Expect 'in' keyword
    if (!match({TokenType::IDENTIFIER})) {
        throw ParseError("Expect 'in' keyword in for loop", peek().line, peek().column);
    }
    if (previous().lexeme != "in") {
        throw ParseError("Expect 'in' keyword in for loop, got '" + previous().lexeme + "'", previous().line, previous().column);
    }

    // Parse iterable expression
    std::unique_ptr<Expr> iterable = expression();

    // Parse loop body
    std::unique_ptr<Stmt> body = statement();

    return std::make_unique<ForStmt>(variable, isMutable, varType, std::move(iterable), std::move(body));
}

std::unique_ptr<Stmt> Parser::returnStatement() {
    Token keyword = previous();
    std::unique_ptr<Expr> value = nullptr;
    if (!check(TokenType::SEMICOLON)) {
        value = expression();
    }
    consume(TokenType::SEMICOLON, "Expect ';' after return value.");
    return std::make_unique<ReturnStmt>(keyword, std::move(value));
}

std::unique_ptr<Stmt> Parser::breakStatement() {
    Token keyword = previous();
    consume(TokenType::SEMICOLON, "Expect ';' after 'break'.");
    return std::make_unique<BreakStmt>(keyword);
}

std::unique_ptr<Stmt> Parser::continueStatement() {
    Token keyword = previous();
    consume(TokenType::SEMICOLON, "Expect ';' after 'continue'.");
    return std::make_unique<ContinueStmt>(keyword);
}

std::unique_ptr<Stmt> Parser::deferStatement() {
    Token keyword = previous();
    // Parse the deferred statement (typically a function call)
    std::unique_ptr<Stmt> stmt = statement();
    return std::make_unique<DeferStmt>(keyword, std::move(stmt));
}

std::unique_ptr<Stmt> Parser::block() {
    std::vector<std::unique_ptr<Stmt>> statements;
    while (!check(TokenType::RIGHT_BRACE) && !isAtEnd()) {
        statements.push_back(declaration());
    }
    consume(TokenType::RIGHT_BRACE, "Expect '}' after block.");
    return std::make_unique<BlockStmt>(std::move(statements));
}

std::unique_ptr<Stmt> Parser::expressionStatement() {
    std::unique_ptr<Expr> expr = expression();
    consume(TokenType::SEMICOLON, "Expect ';' after expression.");
    return std::make_unique<ExpressionStmt>(std::move(expr));
}

// --- Expressions ---

std::unique_ptr<Expr> Parser::expression() {
    return assignment();
}

std::unique_ptr<Expr> Parser::assignment() {
    std::unique_ptr<Expr> expr = range(); 

    if (match({TokenType::EQUAL, TokenType::PLUS_EQUAL, TokenType::MINUS_EQUAL})) {
        Token equals = previous();
        std::unique_ptr<Expr> value = assignment(); // Right-associative

        // In a full compiler, we'd check if 'expr' is a valid assignment target (VariableExpr)
        // For AST generation, we can just return a BinaryExpr or specialized AssignExpr
        // For now, treat as BinaryExpr to keep it simple, or cast.
        return std::make_unique<BinaryExpr>(std::move(expr), equals, std::move(value));
    }
    return expr;
}

std::unique_ptr<Expr> Parser::range() {
    std::unique_ptr<Expr> expr = pipe();
    if (match({TokenType::DOT_DOT})) {
        Token op = previous();
        std::unique_ptr<Expr> right = pipe();
        expr = std::make_unique<BinaryExpr>(std::move(expr), op, std::move(right));
    }
    return expr;
}

std::unique_ptr<Expr> Parser::pipe() {
    std::unique_ptr<Expr> expr = logicOr();
    while (match({TokenType::PIPE})) {
        Token op = previous();
        std::unique_ptr<Expr> right = logicOr();
        expr = std::make_unique<BinaryExpr>(std::move(expr), op, std::move(right));
    }
    return expr;
}

std::unique_ptr<Expr> Parser::logicOr() {
    std::unique_ptr<Expr> expr = logicAnd();
    while (match({TokenType::OR})) {
        Token op = previous();
        std::unique_ptr<Expr> right = logicAnd();
        expr = std::make_unique<BinaryExpr>(std::move(expr), op, std::move(right));
    }
    return expr;
}

std::unique_ptr<Expr> Parser::logicAnd() {
    std::unique_ptr<Expr> expr = bitwiseOr();
    while (match({TokenType::AND})) {
        Token op = previous();
        std::unique_ptr<Expr> right = bitwiseOr();
        expr = std::make_unique<BinaryExpr>(std::move(expr), op, std::move(right));
    }
    return expr;
}

std::unique_ptr<Expr> Parser::bitwiseOr() {
    std::unique_ptr<Expr> expr = bitwiseXor();
    while (match({TokenType::BITWISE_OR})) {
        Token op = previous();
        std::unique_ptr<Expr> right = bitwiseXor();
        expr = std::make_unique<BinaryExpr>(std::move(expr), op, std::move(right));
    }
    return expr;
}

std::unique_ptr<Expr> Parser::bitwiseXor() {
    std::unique_ptr<Expr> expr = bitwiseAnd();
    while (match({TokenType::BITWISE_XOR})) {
        Token op = previous();
        std::unique_ptr<Expr> right = bitwiseAnd();
        expr = std::make_unique<BinaryExpr>(std::move(expr), op, std::move(right));
    }
    return expr;
}

std::unique_ptr<Expr> Parser::bitwiseAnd() {
    std::unique_ptr<Expr> expr = equality();
    while (match({TokenType::BITWISE_AND})) {
        Token op = previous();
        std::unique_ptr<Expr> right = equality();
        expr = std::make_unique<BinaryExpr>(std::move(expr), op, std::move(right));
    }
    return expr;
}

std::unique_ptr<Expr> Parser::equality() {
    std::unique_ptr<Expr> expr = comparison();
    while (match({TokenType::BANG_EQUAL, TokenType::EQUAL_EQUAL})) {
        Token op = previous();
        std::unique_ptr<Expr> right = comparison();
        expr = std::make_unique<BinaryExpr>(std::move(expr), op, std::move(right));
    }
    return expr;
}

std::unique_ptr<Expr> Parser::comparison() {
    std::unique_ptr<Expr> expr = bitwiseShift();
    while (match({TokenType::GREATER, TokenType::GREATER_EQUAL, TokenType::LESS, TokenType::LESS_EQUAL})) {
        Token op = previous();
        std::unique_ptr<Expr> right = bitwiseShift();
        expr = std::make_unique<BinaryExpr>(std::move(expr), op, std::move(right));
    }
    return expr;
}

std::unique_ptr<Expr> Parser::bitwiseShift() {
    std::unique_ptr<Expr> expr = term();
    while (match({TokenType::LEFT_SHIFT, TokenType::RIGHT_SHIFT})) {
        Token op = previous();
        std::unique_ptr<Expr> right = term();
        expr = std::make_unique<BinaryExpr>(std::move(expr), op, std::move(right));
    }
    return expr;
}

std::unique_ptr<Expr> Parser::term() {
    std::unique_ptr<Expr> expr = factor();
    while (match({TokenType::MINUS, TokenType::PLUS})) {
        Token op = previous();
        std::unique_ptr<Expr> right = factor();
        expr = std::make_unique<BinaryExpr>(std::move(expr), op, std::move(right));
    }
    return expr;
}

std::unique_ptr<Expr> Parser::factor() {
    std::unique_ptr<Expr> expr = cast();
    while (match({TokenType::SLASH, TokenType::STAR, TokenType::PERCENT})) {
        Token op = previous();
        std::unique_ptr<Expr> right = cast();
        expr = std::make_unique<BinaryExpr>(std::move(expr), op, std::move(right));
    }
    return expr;
}

std::unique_ptr<Expr> Parser::unary() {
    if (match({TokenType::BANG, TokenType::MINUS, TokenType::BITWISE_NOT})) {
        Token op = previous();
        std::unique_ptr<Expr> right = unary();
        return std::make_unique<UnaryExpr>(op, std::move(right));
    }

    if (match({TokenType::AWAIT})) {
        Token keyword = previous();
        std::unique_ptr<Expr> expr = unary();
        return std::make_unique<AwaitExpr>(keyword, std::move(expr));
    }

    return call();
}

std::unique_ptr<Expr> Parser::call() {
    std::unique_ptr<Expr> expr = primary();

    // Handle generic type parameters for constructors like Array<T>()
    // Check if we have an identifier followed by <
    // IMPORTANT: Only treat < as generics if followed by a ( after the closing >
    // This prevents false positive with comparisons like: x < 10
    if (auto* varExpr = dynamic_cast<VariableExpr*>(expr.get())) {
        if (check(TokenType::LESS)) {
            // Look ahead to see if this is actually generics (has matching > followed by ()
            // Save current position
            int savedCurrent = current;

            // Try to find matching >
            advance(); // consume <
            int depth = 1;
            bool foundMatchingBracket = false;
            while (depth > 0 && !isAtEnd()) {
                if (check(TokenType::LESS)) {
                    depth++;
                    advance();
                } else if (check(TokenType::GREATER)) {
                    depth--;
                    advance();
                    if (depth == 0) {
                        foundMatchingBracket = true;
                        break;
                    }
                } else {
                    advance();
                }
            }

            // Check if this looks like a generic constructor call
            // (should have ( after the closing >)
            bool isGenericCall = foundMatchingBracket && check(TokenType::LEFT_PAREN);

            if (!isGenericCall) {
                // Not a generic call, restore position
                // This is likely a comparison operator like x < 10
                current = savedCurrent;
            }
            // If it is a generic call, we've already consumed the tokens, which is what we want
        }
    }



    // Check for Struct Initialization: Name { field: value, ... }
    if (auto* varExpr = dynamic_cast<VariableExpr*>(expr.get())) {
        if (check(TokenType::LEFT_BRACE)) {
            // Check if it looks like a struct init (identifiers followed by colon)
            // Or just a block? Block is statement, here we are in expression.
            // Map literal starts with {.
            // But Map literal is handled in primary().
            
            // We need to verify if this is a struct init.
            // Lookahead: { identifier :
            int savedCurrent = current;
            advance(); // {
            
            bool isStructInit = false;
            if (check(TokenType::IDENTIFIER)) {
                 advance(); // identifier
                 if (check(TokenType::COLON)) {
                     isStructInit = true;
                 }
            } else if (check(TokenType::RIGHT_BRACE)) {
                // Empty struct init? Name {}
                isStructInit = true;
            }
            
            // Restore
            current = savedCurrent;
            
            if (isStructInit) {
                consume(TokenType::LEFT_BRACE, "Expect '{' after struct name.");
                std::vector<std::pair<std::string, std::unique_ptr<Expr>>> fields;
                
                if (!check(TokenType::RIGHT_BRACE)) {
                    do {
                        Token fieldName = consume(TokenType::IDENTIFIER, "Expect field name.");
                        consume(TokenType::COLON, "Expect ':' after field name.");
                        std::unique_ptr<Expr> value = expression();
                        fields.push_back({fieldName.lexeme, std::move(value)});
                    } while (match({TokenType::COMMA}));
                }
                
                consume(TokenType::RIGHT_BRACE, "Expect '}' after struct initialization.");
                
                return std::make_unique<StructInitExpr>(varExpr->name, std::move(fields));
            }
        }
    }

    while (true) {
        if (match({TokenType::LEFT_PAREN})) {
            expr = finishCall(std::move(expr));
        } else if (check(TokenType::LESS)) {
            // Could be generic function call like func<Type>(args) or comparison
            // Save position BEFORE consuming < for backtracking
            int savedCurrent = current;
            advance(); // consume <

            // Try to parse as generic type arguments
            std::vector<std::string> typeArgs;
            bool isGenericCall = true;

            // Try to parse type arguments
            do {
                try {
                    typeArgs.push_back(parseType());
                } catch (...) {
                    isGenericCall = false;
                    break;
                }
            } while (match({TokenType::COMMA}));

            // Check for closing > and following (
            if (isGenericCall && match({TokenType::GREATER}) && check(TokenType::LEFT_PAREN)) {
                // It's a generic function call - consume ( and parse as call
                advance(); // consume (
                expr = finishCall(std::move(expr));
                // Store type args in the CallExpr (we'll need to extend CallExpr for this)
                // For now, type args are parsed but not stored - the callee name carries them
            } else {
                // Not a generic call - backtrack and treat < as comparison
                current = savedCurrent;
                // Let the comparison parsing handle this by breaking out
                break;
            }
        } else if (match({TokenType::LEFT_BRACKET})) {
            std::unique_ptr<Expr> index = expression();
            Token bracket = consume(TokenType::RIGHT_BRACKET, "Expect ']' after array index.");
            expr = std::make_unique<IndexExpr>(std::move(expr), std::move(index), bracket);
        } else if (match({TokenType::DOT})) {
             Token dot = previous();
             Token name = consume(TokenType::IDENTIFIER, "Expect property name after '.'.");
             // Transform into property access expr (omitted for brevity, simpler to assume variable expr in this context implies access if not call)
             // But strictly we need a GetExpr. Reusing VariableExpr logic or generic BinaryExpr for dot?
             // Let's cheat and make it a BinaryExpr with DOT op
             // Right side needs to be an Expr.
             expr = std::make_unique<BinaryExpr>(std::move(expr), dot, std::make_unique<VariableExpr>(name));
        } else {
            break;
        }
    }
    return expr;
}



std::unique_ptr<Expr> Parser::finishCall(std::unique_ptr<Expr> callee) {
    std::vector<std::unique_ptr<Expr>> arguments;
    if (!check(TokenType::RIGHT_PAREN)) {
        do {
            // Handle named arguments: identifier '=' expression
            // Simplified: just parse expression.
            arguments.push_back(expression());
        } while (match({TokenType::COMMA}));
    }
    Token paren = consume(TokenType::RIGHT_PAREN, "Expect ')' after arguments.");
    return std::make_unique<CallExpr>(std::move(callee), paren, std::move(arguments));
}

std::unique_ptr<Expr> Parser::primary() {
    if (match({TokenType::FALSE})) return std::make_unique<LiteralExpr>("false", TokenType::FALSE);
    if (match({TokenType::TRUE})) return std::make_unique<LiteralExpr>("true", TokenType::TRUE);
    if (match({TokenType::NONE})) return std::make_unique<LiteralExpr>("None", TokenType::NONE);

    if (match({TokenType::NUMBER, TokenType::STRING, TokenType::CHAR})) {
        return std::make_unique<LiteralExpr>(previous().lexeme, previous().type);
    }

    // Handle interpolated strings like "Hello $name" or "Value is ${x + 1}"
    if (match({TokenType::INTERPOLATED_STRING})) {
        return parseInterpolatedString(previous().lexeme);
    }

    if (match({TokenType::IDENTIFIER})) {
        return std::make_unique<VariableExpr>(previous());
    }

    // Handle 'this'
    if (match({TokenType::THIS})) {
        return std::make_unique<VariableExpr>(previous());
    }

    if (match({TokenType::WHEN})) {
        return whenExpression(); // Parse as expression
    }

    // Allow 'async' as an identifier (for module names like async.delay)
    // OR as async lambda: async () => { ... }
    if (match({TokenType::ASYNC})) {
        // Check if this is an async lambda
        if (check(TokenType::LEFT_PAREN)) {
            advance(); // consume '('

            // Parse async lambda
            // 1. Check for empty lambda: async () =>
            if (check(TokenType::RIGHT_PAREN)) {
                if (current + 1 < tokens.size() && tokens[current + 1].type == TokenType::ARROW) {
                    advance(); // consume )
                    advance(); // consume =>
                    std::unique_ptr<Stmt> body = parseLambdaBody();
                    return std::make_unique<LambdaExpr>(std::vector<Token>{}, std::move(body), true); // isAsync = true
                }
            }

            std::unique_ptr<Expr> expr = expression();

            // 2. Check for multi-param lambda: async (a, b) =>
            if (match({TokenType::COMMA})) {
                std::vector<Token> params;

                // First param
                if (auto* v = dynamic_cast<VariableExpr*>(expr.get())) {
                    params.push_back(v->name);
                } else {
                    throw ParseError("Expect parameter name.", peek().line, peek().column);
                }

                // Remaining params
                do {
                    Token param = consume(TokenType::IDENTIFIER, "Expect parameter name.");
                    params.push_back(param);
                } while (match({TokenType::COMMA}));

                consume(TokenType::RIGHT_PAREN, "Expect ')' after parameters.");
                consume(TokenType::ARROW, "Expect '=>' after lambda parameters.");
                std::unique_ptr<Stmt> body = parseLambdaBody();
                return std::make_unique<LambdaExpr>(std::move(params), std::move(body), true); // isAsync = true
            }

            consume(TokenType::RIGHT_PAREN, "Expect ')' after expression.");

            // 3. Check for single-param lambda: async (a) =>
            if (match({TokenType::ARROW})) {
                std::vector<Token> params;
                if (auto* v = dynamic_cast<VariableExpr*>(expr.get())) {
                    params.push_back(v->name);
                } else {
                    throw ParseError("Expect parameter name.", peek().line, peek().column);
                }
                std::unique_ptr<Stmt> body = parseLambdaBody();
                return std::make_unique<LambdaExpr>(std::move(params), std::move(body), true); // isAsync = true
            }

            // Not a lambda, error - we consumed ASYNC and LEFT_PAREN but it's not a lambda
            throw ParseError("Expected lambda expression after 'async ('.", peek().line, peek().column);
        }

        // Just 'async' as identifier (for async.delay, etc.)
        return std::make_unique<VariableExpr>(previous());
    }

    // Handle If-Expression (Ternary)
    if (match({TokenType::IF})) {
        consume(TokenType::LEFT_PAREN, "Expect '(' after 'if'.");
        std::unique_ptr<Expr> condition = expression();
        consume(TokenType::RIGHT_PAREN, "Expect ')' after condition.");
        std::unique_ptr<Expr> thenExpr = expression();
        consume(TokenType::ELSE, "Expect 'else' for if-expression.");
        std::unique_ptr<Expr> elseExpr = expression();
        
        // Map to a specialized TernaryExpr or (mis)use CallExpr/IfStmt?
        // Ideally we need TernaryExpr in AST.
        // For now, let's treat it as a Call to a special intrinsic "if_expr(cond, then, else)"
        // This avoids changing AST structure mid-flight.
        std::vector<std::unique_ptr<Expr>> args;
        args.push_back(std::move(condition));
        args.push_back(std::move(thenExpr));
        args.push_back(std::move(elseExpr));
        return std::make_unique<CallExpr>(
            std::make_unique<VariableExpr>(Token{TokenType::IDENTIFIER, "__if_expr", 0, 0}), 
            Token{TokenType::RIGHT_PAREN, ")", 0, 0}, 
            std::move(args)
        );
    }

    if (match({TokenType::LEFT_PAREN})) {
        // 1. Check for empty lambda: () =>
        if (check(TokenType::RIGHT_PAREN)) {
            // Need to peek next token to see if it is ARROW
            if (current + 1 < tokens.size() && tokens[current + 1].type == TokenType::ARROW) {
                advance(); // consume )
                advance(); // consume =>
                std::unique_ptr<Stmt> body = parseLambdaBody();
                return std::make_unique<LambdaExpr>(std::vector<Token>{}, std::move(body));
            }
        }

        std::unique_ptr<Expr> expr = expression();

        // 2. Check for multi-param lambda: (a, b) =>
        if (match({TokenType::COMMA})) {
            std::vector<Token> params;
            
            // First param
            if (auto* v = dynamic_cast<VariableExpr*>(expr.get())) {
                params.push_back(v->name);
            } else {
                throw ParseError("Expect parameter name.", peek().line, peek().column);
            }

            // Remaining params
            do {
                Token param = consume(TokenType::IDENTIFIER, "Expect parameter name.");
                params.push_back(param);
            } while (match({TokenType::COMMA}));

            consume(TokenType::RIGHT_PAREN, "Expect ')' after parameters.");
            consume(TokenType::ARROW, "Expect '=>' after lambda parameters.");
            std::unique_ptr<Stmt> body = parseLambdaBody();
            return std::make_unique<LambdaExpr>(std::move(params), std::move(body));
        }

        consume(TokenType::RIGHT_PAREN, "Expect ')' after expression.");

        // 3. Check for single-param lambda: (a) =>
        if (match({TokenType::ARROW})) {
            std::vector<Token> params;
            if (auto* v = dynamic_cast<VariableExpr*>(expr.get())) {
                params.push_back(v->name);
            } else {
                throw ParseError("Expect parameter name.", peek().line, peek().column);
            }
            std::unique_ptr<Stmt> body = parseLambdaBody();
            return std::make_unique<LambdaExpr>(std::move(params), std::move(body));
        }

        return std::make_unique<GroupingExpr>(std::move(expr));
    }

    // Handle array literals: [expr, expr, ...]
    if (match({TokenType::LEFT_BRACKET})) {
        std::vector<std::unique_ptr<Expr>> elements;
        if (!check(TokenType::RIGHT_BRACKET)) {
            do {
                elements.push_back(expression());
            } while (match({TokenType::COMMA}));
        }
        consume(TokenType::RIGHT_BRACKET, "Expect ']' after array elements.");
        return std::make_unique<ArrayLiteralExpr>(std::move(elements));
    }

    // Handle map/object literals: { key: value, ... }
    if (match({TokenType::LEFT_BRACE})) {
        std::vector<std::pair<std::string, std::unique_ptr<Expr>>> entries;
        if (!check(TokenType::RIGHT_BRACE)) {
            do {
                Token key = consume(TokenType::IDENTIFIER, "Expect key in map literal.");
                consume(TokenType::COLON, "Expect ':' after key in map literal.");
                std::unique_ptr<Expr> value = expression();
                entries.push_back({key.lexeme, std::move(value)});
            } while (match({TokenType::COMMA}));
        }
        consume(TokenType::RIGHT_BRACE, "Expect '}' after map literal.");
        return std::make_unique<MapLiteralExpr>(std::move(entries));
    }

    throw ParseError("Expect expression. Found: " + peek().lexeme, peek().line, peek().column);
}

// New cast() function
std::unique_ptr<Expr> Parser::cast() {
    std::unique_ptr<Expr> expr = unary(); // Start with a unary expression
    if (match({TokenType::AS})) {
        Token asToken = previous(); // The 'as' keyword
        
        // Check for optional '?' for safe cast
        bool isSafe = false;
        if (match({TokenType::QUESTION})) {
            isSafe = true;
        }

        std::string typeName = parseType(); // Parse the target type (e.g., int, double)
        
        // Convert typeName string back to TokenType for CastExpr
        TokenType targetTypeToken = TokenType::IDENTIFIER; // Default to IDENTIFIER
        if (typeName == "int") targetTypeToken = TokenType::INT;
        else if (typeName == "double") targetTypeToken = TokenType::DOUBLE;
        else if (typeName == "string") targetTypeToken = TokenType::STRING;
        else if (typeName == "bool") targetTypeToken = TokenType::BOOL;
        else if (typeName == "char") targetTypeToken = TokenType::CHAR;
        else if (typeName == "void") targetTypeToken = TokenType::VOID;
        else if (typeName == "unit") targetTypeToken = TokenType::UNIT;
        else if (typeName.rfind("Optional<", 0) == 0) targetTypeToken = TokenType::OPTIONAL; // Handle Optional<T>
        else {
            // For custom types, IDENTIFIER is fine for now
        }
        
        // Create a CastExpr node
        return std::make_unique<CastExpr>(std::move(expr), Token{targetTypeToken, typeName, asToken.line, asToken.column}, isSafe);
    }
    return expr;
}

// --- Helpers ---

std::string Parser::parseType() {
    if (match({TokenType::OPTIONAL})) {
        consume(TokenType::LESS, "Expect <");
        std::string inner = parseType();
        consume(TokenType::GREATER, "Expect >");
        return "Optional<" + inner + ">";
    }
    // Include ASYNC to support async.Promise<T> type syntax
    if (match({TokenType::IDENTIFIER, TokenType::INT, TokenType::STRING, TokenType::BOOL, TokenType::DOUBLE, TokenType::VOID,
                TokenType::I8, TokenType::I16, TokenType::I32, TokenType::I64,
                TokenType::U8, TokenType::U16, TokenType::U32, TokenType::U64,
                TokenType::F32, TokenType::F64, TokenType::USIZE, TokenType::ISIZE,
                TokenType::ASYNC})) {
        std::string typeName = previous().lexeme;

        // Handle qualified type names (e.g., io.Error, module.Type)
        while (match({TokenType::DOT})) {
            Token name = consume(TokenType::IDENTIFIER, "Expect type name after '.'");
            typeName += "." + name.lexeme;
        }

        // Handle generic type parameters (e.g., Result<string, Error>, Array<int>)
        if (match({TokenType::LESS})) {
            typeName += "<";
            int depth = 1;
            while (depth > 0 && !isAtEnd()) {
                Token token = advance();
                typeName += token.lexeme;
                if (token.type == TokenType::LESS) {
                    depth++;
                } else if (token.type == TokenType::GREATER) {
                    depth--;
                }
            }
        }

        // Handle pointer type suffix '*' (e.g., i32*, u8*)
        while (match({TokenType::STAR})) {
            typeName += "*";
        }

        // Handle optional type suffix '?' (e.g., String?)
        if (match({TokenType::QUESTION})) {
             typeName += "?";
        }

        return typeName;
    }
    Token t = peek();
    throw ParseError("Expect type. Found: " + t.lexeme, t.line, t.column);
}

bool Parser::match(const std::vector<TokenType>& types) {
    for (TokenType type : types) {
        if (check(type)) {
            advance();
            return true;
        }
    }
    return false;
}

bool Parser::check(TokenType type) {
    if (isAtEnd()) return false;
    return peek().type == type;
}

Token Parser::advance() {
    if (!isAtEnd()) current++;
    return previous();
}

bool Parser::isAtEnd() {
    return peek().type == TokenType::END_OF_FILE;
}

Token Parser::peek() {
    return tokens[current];
}

Token Parser::previous() {
    return tokens[current - 1];
}

Token Parser::consume(TokenType type, std::string message) {
    if (check(type)) return advance();
    Token t = peek();
    throw ParseError(message + " Found: " + t.lexeme, t.line, t.column);
}

std::unique_ptr<Stmt> Parser::parseLambdaBody() {
    if (match({TokenType::LEFT_BRACE})) {
        return block();
    } else {
        std::unique_ptr<Expr> expr = expression();
        // Wrap expression in a ReturnStmt for implicit return
        // We wrap that in a BlockStmt to match LambdaExpr signature expectation if needed? 
        // LambdaExpr takes Stmt, so ReturnStmt is fine. But execution of Lambda usually expects a Block.
        // Let's return a Block containing the ReturnStmt.
        std::vector<std::unique_ptr<Stmt>> stmts;
        stmts.push_back(std::make_unique<ReturnStmt>(Token{TokenType::RETURN, "return", 0, 0}, std::move(expr)));
        return std::make_unique<BlockStmt>(std::move(stmts));
    }
}

void Parser::synchronize() {
    advance();
    while (!isAtEnd()) {
        if (previous().type == TokenType::SEMICOLON) return;
        switch (peek().type) {
            case TokenType::CLASS:
            case TokenType::FN:
            case TokenType::VAR:
            case TokenType::VAL:
            case TokenType::FOR:
            case TokenType::IF:
            case TokenType::WHILE:
            case TokenType::RETURN:
                return;
            default: // Ignore
                break;
        }
        advance();
    }
}

// --- Documentation Parsing ---

void Parser::consumeDocComment() {
    // Check if current token is a doc comment
    if (check(TokenType::DOC_COMMENT)) {
        Token docToken = advance();
        pendingDocComment = parseDocComment(docToken.docText);
        pendingDocComment->line = docToken.line;
        pendingDocComment->rawText = docToken.docText;
    }
}

std::unique_ptr<DocComment> Parser::takePendingDoc() {
    return std::move(pendingDocComment);
}

std::unique_ptr<DocComment> Parser::parseDocComment(const std::string& rawText) {
    auto doc = std::make_unique<DocComment>();

    std::istringstream stream(rawText);
    std::string line;
    std::string currentSection;
    bool inDescription = true;
    std::string currentTag;
    std::string currentTagContent;

    while (std::getline(stream, line)) {
        // Trim whitespace
        size_t start = line.find_first_not_of(" \t");
        if (start == std::string::npos) {
            // Empty line
            if (inDescription && !currentSection.empty()) {
                currentSection += "\n\n";
            } else if (!currentTag.empty()) {
                currentTagContent += "\n";
            }
            continue;
        }

        line = line.substr(start);

        // Check for tags
        if (line[0] == '@') {
            // Save previous tag if any
            if (!currentTag.empty()) {
                saveTag(doc, currentTag, currentTagContent);
                currentTag = "";
                currentTagContent = "";
            }

            inDescription = false;

            // Parse tag
            size_t spacePos = line.find(' ');
            std::string tagName = (spacePos != std::string::npos)
                ? line.substr(1, spacePos - 1)
                : line.substr(1);
            std::string tagContent = (spacePos != std::string::npos)
                ? line.substr(spacePos + 1)
                : "";

            currentTag = tagName;
            currentTagContent = tagContent;
        } else {
            // Regular text
            if (inDescription) {
                currentSection += line + " ";
            } else if (!currentTag.empty()) {
                // Continuation of a tag (e.g., multi-line @example)
                if (!currentTagContent.empty()) {
                    currentTagContent += "\n";
                }
                currentTagContent += line;
            }
        }
    }

    // Save last tag if any
    if (!currentTag.empty()) {
        saveTag(doc, currentTag, currentTagContent);
    }

    // Finalize description
    if (!currentSection.empty()) {
        // Split summary from description at first paragraph break
        size_t doubleNewline = currentSection.find("\n\n");
        if (doubleNewline != std::string::npos) {
            doc->summary = currentSection.substr(0, doubleNewline);
            doc->description = currentSection;
        } else {
            doc->summary = currentSection;
            doc->description = currentSection;
        }

        // Trim trailing whitespace
        while (!doc->summary.empty() && std::isspace(doc->summary.back())) {
            doc->summary.pop_back();
        }
    }

    return doc;
}

void Parser::saveTag(std::unique_ptr<DocComment>& doc, const std::string& tagName, const std::string& tagContent) {
    if (tagName == "param") {
        // Format: @param name description
        ParamDoc param;
        std::istringstream paramStream(tagContent);
        paramStream >> param.name;

        // Rest is description
        std::getline(paramStream, param.description);
        if (!param.description.empty() && param.description[0] == ' ') {
            param.description = param.description.substr(1);
        }

        doc->params.push_back(param);
    } else if (tagName == "return") {
        doc->returnDoc = tagContent;
    } else if (tagName == "throws") {
        doc->throws.push_back(tagContent);
    } else if (tagName == "example") {
        doc->examples.push_back(tagContent);
    } else if (tagName == "since") {
        doc->since = tagContent;
    } else if (tagName == "deprecated") {
        doc->deprecated = tagContent;
    } else {
        doc->customTags[tagName] = tagContent;
    }
}

} // namespace stratos