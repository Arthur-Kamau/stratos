//
// Created by kamau on 1/29/21.
//

#include "Lexer.h"


Lexer::Lexer() {
    {
        if (s_keywords.empty()) {


            s_keywords["package"] = NodeType::Package;
            s_keywords["import"] = NodeType::Import;
            s_keywords["class"] = NodeType::Class;

            s_keywords["fun"] = NodeType::Function;
            s_keywords["await"] = NodeType::Await;
            s_keywords["async"] = NodeType::Async;
            s_keywords["return"] = NodeType::Return;
            s_keywords["yield"] = NodeType::Yeild;
            s_keywords["stream"] = NodeType::Stream;

            s_keywords["var"] = NodeType::Variable;
            s_keywords["val"] = NodeType::Value;
            s_keywords["let"] = NodeType::Let;

            s_keywords["int"] = NodeType::Int;
            s_keywords["string"] = NodeType::String;
            s_keywords["double"] = NodeType::Double;
            s_keywords["char"] = NodeType::Char;

            s_keywords["for"] = NodeType::For;
            s_keywords["is"] = NodeType::Is;
            s_keywords["as"] = NodeType::As;
            s_keywords["range"] = NodeType::Range;

            s_keywords["list"] = NodeType::List;
            s_keywords["set"] = NodeType::Set;
            s_keywords["map"] = NodeType::Map;

            s_keywords["with"] = NodeType::With;
            s_keywords["while"] = NodeType::While;
            s_keywords["when"] = NodeType::When;
            s_keywords["else"] = NodeType::Else;
            s_keywords["true"] = NodeType::True;
            s_keywords["false"] = NodeType::False;

            s_keywords["break"] = NodeType::Break;
            s_keywords["continue"] = NodeType::Continue;

            s_keywords["private"] = NodeType::Private;
            s_keywords["public"] = NodeType::Public;


        }

        if (s_two_char_tokens.empty()) {
            s_two_char_tokens["=="] = NodeType::EqualsEquals;
            s_two_char_tokens["<="] = NodeType::LessThanEquals;
            s_two_char_tokens[">="] = NodeType::GreaterThanEquals;
            s_two_char_tokens["!="] = NodeType::ExclamationMarkEquals;
            s_two_char_tokens["--"] = NodeType::MinusMinus;
            s_two_char_tokens["++"] = NodeType::PlusPlus;
        }

        if (s_single_char_tokens.empty()) {
            s_single_char_tokens['&'] = NodeType::Ampersand;
            s_single_char_tokens['*'] = NodeType::Asterisk;
            s_single_char_tokens['['] = NodeType::SquareBracketsOpen;
            s_single_char_tokens[']'] = NodeType::SquareBracketsClose;
            s_single_char_tokens['^'] = NodeType::Caret;
            s_single_char_tokens[':'] = NodeType::Colon;
            s_single_char_tokens[']'] = NodeType::Comma;
            s_single_char_tokens['{'] = NodeType::CurlyBracketsOpen;
            s_single_char_tokens['}'] = NodeType::CurlyBracketsClose;
            s_single_char_tokens['='] = NodeType::Equals;
            s_single_char_tokens['!'] = NodeType::ExclamationMark;
            s_single_char_tokens['-'] = NodeType::Minus;
            s_single_char_tokens['('] = NodeType::RoundBracketsOpen;
            s_single_char_tokens[')'] = NodeType::RoundBracketsClose;
            s_single_char_tokens['%'] = NodeType::Percent;
            s_single_char_tokens['.'] = NodeType::Period;
            s_single_char_tokens['|'] = NodeType::Pipe;
            s_single_char_tokens['+'] = NodeType::Plus;
            s_single_char_tokens['?'] = NodeType::QuestionMark;
            s_single_char_tokens[';'] = NodeType::Semicolon;
            s_single_char_tokens['/'] = NodeType::Slash;
            s_single_char_tokens['~'] = NodeType::Tilde;
            s_single_char_tokens['<'] = NodeType::LessThan;
            s_single_char_tokens['>'] = NodeType::GreaterThan;
        }
        if (s_three_char_tokens.empty()) {
            s_three_char_tokens["..."] = NodeType::TripleDot;
        }
    }
}

std::vector<Node> Lexer::lex_text(std::string source) {

    source_length = source.size();
    char_array.assign(source.begin(), source.end());

    while (!is_eof()) {
        lex();

    }
    Node var;
    var.start = current_character_index;
    var.end = current_character_index + 1;
    var.line = line_number;
    var.literal = "";
    var.type = NodeType::Eof;

    nodes.push_back(var);

    return nodes;
}

void Lexer::add_three_char_node(std::string item, NodeType type) {
    Node var;
    var.start = current_character_index;
    var.end = current_character_index + 3;
    var.line = line_number;
    var.literal = item;
    var.type = type;
    nodes.push_back(
            var
    );
}

void Lexer::add_two_char_node(std::string item, NodeType type) {
    Node var;
    var.start = current_character_index;
    var.end = current_character_index + 2;
    var.line = line_number;
    var.literal = item;
    var.type = type;
    nodes.push_back(
            var
    );
}

void Lexer::add_one_char_node(char item, NodeType type) {
    Node var;
    var.start = current_character_index;
    var.end = current_character_index + 1;
    var.line = line_number;
    var.literal = item;
    var.type = type;
    nodes.push_back(
            var
    );
}

void Lexer::lex() {
    int start_character_index;
    char peek_char;
    char double_peek_char;
    char start = current();
    switch (start) {

        case '"':
            peek_char = peek();
            if (peek_char == '"') {
                Node var;
                var.start =current_character_index ;
                var.end = current_character_index+1;
                var.line = line_number;
                var.literal = "";
                var.type = NodeType::UserDefinedString;
                nodes.push_back(
                        var
                );
                //consume the closing "
                advance();
            } else {
                std::string text_string;
                start_character_index = current_character_index;
                text_string.push_back(start);
                while (!is_eof()) {
                    char advanced = advance();
                    if (current() == '\n') {

                        line_number++;


                    }

                    if (current() == '"') {
                        text_string.push_back(advanced);
                        break;
                    } else {

                        text_string.push_back(advanced);
                    }
                }

                Node var;
                var.start = start_character_index;
                var.end = current_character_index;
                var.line = line_number;
                var.literal = text_string;
                var.type = NodeType::UserDefinedString;
                nodes.push_back(
                        var
                );
            }

            break;
        case '\'':
            peek_char = peek();
            if (peek_char == '\'') {
                Node var;
                var.start =current_character_index ;
                var.end = current_character_index+1;
                var.line = line_number;
                var.literal = "";
                var.type = NodeType::UserDefinedChar;
                nodes.push_back(
                        var
                );
                advance();
            } else {
                std::string text_string;
                start_character_index = current_character_index;
                text_string.push_back(start);
                while (!is_eof()) {
                    char advanced = advance();
                    if (current() == '\n') {

                        line_number++;


                    }

                    if (current() == '"') {
                        text_string.push_back(advanced);
                        break;
                    } else {

                        text_string.push_back(advanced);
                    }
                }

                Node var;
                var.start = start_character_index;
                var.end = current_character_index;
                var.line = line_number;
                var.literal = text_string;
                var.type = NodeType::UserDefinedChar;
                nodes.push_back(
                        var
                );
            }

            //consume the closing '
            advance();
            break;
        case '/':
            peek_char = peek();
            if (peek_char == '*') {

                std::string text_string;
                start_character_index = current_character_index;
                text_string.push_back(start);
                while (!is_eof()) {

                    char advanced = advance();

                    if (current() == '*' && peek() == '/') {

                        if (current() == '\n') {
                            line_number++;
                        }

                        text_string.push_back(current());
                        text_string.push_back(advance());
                        break;
                    } else {

                        text_string.push_back(advanced);
                    }
                }
                Node var;
                var.start = start_character_index;
                var.end = current_character_index;
                var.line = line_number;
                var.literal = text_string;
                var.type = NodeType::CommentBlock;
                nodes.push_back(
                        var
                );

            } else if (peek_char == '/') {
                std::string text_string;
                start_character_index = current_character_index;
                text_string.push_back(start);
                while (!is_eof()) {

                    char advanced = advance();
                    if (current() == '\n') {

                        line_number++;

                        break;
                    } else {

                        text_string.push_back(advanced);
                    }
                }
                Node var;
                var.start = start_character_index;
                var.end = current_character_index;
                var.line = line_number;
                var.literal = text_string;
                var.type = NodeType::LineComment;
                nodes.push_back(
                        var
                );
            } else {
                std::cerr << "Error , expected '/' or '*'  but got " << current() << std::endl;
                std::exit(1);
            }
            break;
        case '&':
            peek_char = peek();
            if (peek_char == '&') {
                add_two_char_node("&&", NodeType::And);
                advance();//consume the = character
            } else {
                add_one_char_node(start, NodeType::XAnd);
            }
            break;
        case '|':
            peek_char = peek();
            if (peek_char == '|') {
                add_two_char_node("||", NodeType::Or);
                advance();//consume the = character
            } else {
                add_one_char_node(start, NodeType::XOr);
            }
            break;
        case '+':
            peek_char = peek();
            if (peek_char == '+') {
                add_two_char_node("++", NodeType::PlusPlus);
                advance();//consume the = character
            } else {
                add_one_char_node(start, NodeType::Plus);
            }
            break;
        case ':':
            add_one_char_node(start, NodeType::Colon);
            break;
        case ',':
            add_one_char_node(start, NodeType::Comma);
            break;
        case '.':
            peek_char = peek();
            double_peek_char = double_peek();
            if(peek_char=='.' && double_peek_char=='.'){
                add_three_char_node("...", NodeType::Range);
                advance(); // consume the .
                advance();// consume the .
            }else {
                add_one_char_node(start, NodeType::Period);
            }
            break;
        case '<':
            add_one_char_node(start, NodeType::LessThan);
            break;
        case '>':
            add_one_char_node(start, NodeType::GreaterThan);
            break;
        case '*':
            add_one_char_node(start, NodeType::Multiply);
            break;
        case '~':
            add_one_char_node(start, NodeType::Tilde);
            break;
        case '{':
            add_one_char_node(start, NodeType::CurlyBracketsOpen);
            break;
        case '}':
            add_one_char_node(start, NodeType::CurlyBracketsClose);
            break;
        case '(':
            add_one_char_node(start, NodeType::RoundBracketsOpen);
            break;
        case ')':
            add_one_char_node(start, NodeType::RoundBracketsClose);
            break;
        case '[':
            add_one_char_node(start, NodeType::SquareBracketsOpen);
            break;
        case ']':
            add_one_char_node(start, NodeType::SquareBracketsClose);
            break;
        case '_':
            add_one_char_node(start, NodeType::UnderScore);
            break;

        case '!':
            peek_char = peek();

            if (peek_char == '=') {
                add_two_char_node("!=", NodeType::NotEquals);
                advance();//consume the = character
            } else {
                add_one_char_node(start, NodeType::ExclamationMark);
            }
            break;
        case '=' :
            // peek if next char equal =
            peek_char = peek();

            if (peek_char == '=') {
                add_two_char_node("==", NodeType::EqualsEquals);
                advance();//consume the = character
            } else {
                add_one_char_node(start, NodeType::Equals);
            }
            break;

        default:
            if (non_digit(start) || digit(start)) {
                std::string text_string;
                start_character_index = current_character_index;
                text_string.push_back(start);

                while (true) {

                    char advanced = advance();

                    if (current() == ' ' || current() == '{' || current() == '}' || current() == '(' ||
                        current() == ')' || current() == '[' || current() == ']' || current() == '.' ||
                        current() == ':' || current() == ';' || current() == ',' || current() == '\n') {

                        if (current() == '\n') {
                            line_number++;
                        }
                        break;
                    } else {

                        text_string.push_back(advanced);
                    }
                }
                Node var;
                var.start = start_character_index;
                var.end = current_character_index;
                var.line = line_number;
                var.literal = text_string;
                if (non_digit(start)) {
                    if (key_word(text_string)) {

                        var.type = s_keywords[text_string];

                    } else {
                        var.type = NodeType::UserDefinedName;
                    }
                } else {
                    //todo long
                    var.type = NodeType::UserDefinedInt;
                }

                nodes.push_back(
                        var
                );


            } else {


                if (current() == ' ' || current() == '\n') {
                    if (current() == '\n') {
                        line_number++;
                    }

                } else {
                    std::cerr << "Unknown " << current() << "is non digit " << non_digit(current()) << std::endl;
                }
            }
    }
    // proceed to next character
    // will be gotten with current at start of function
    // function is in loop
    advance();
}


char Lexer::current() {
    return char_array[current_character_index];
}

bool Lexer::is_eof() const {
    return current_character_index >= source_length;
}

char Lexer::advance() {


    current_character_index++;


    return char_array[current_character_index];
}

char Lexer::double_peek() {
    int next = current_character_index + 2;
    return char_array[next];
}
char Lexer::peek() {

    int next = current_character_index + 1;
    return char_array[next];

}


bool Lexer::key_word(const std::string &ch) {

    return s_keywords.count(ch) > 0;
}

/*
symbol : one of
  == , >= , <= ,
*/
bool Lexer::double_symbol(const std::string &ch) {

    return s_two_char_tokens.count(ch) > 0;
}


/*
symbol : one of
  ! % ^ ~ & * ( ) - + = [ ] { } | : ; < > , . / \ ' " @ # ` ?
*/
bool Lexer::single_symbol(char ch) {

    return s_single_char_tokens.count(ch) > 0;
}

/*
digit : one of
    0 1 2 3 4 5 6 7 8 9
*/
bool Lexer::digit(char ch) {
    return ((ch - '0' >= 0) && (ch - '0' <= 9));
}

/*
nonzero-digit : one of
	1 2 3 4 5 6 7 8 9
*/
bool Lexer::nonzero_digit(char ch) {
    return ((ch - '0' >= 1) && (ch - '0' <= 9));
}

/*
octal-digit : one of
  0 1 2 3 4 5 6 7
*/
bool Lexer::octal_digit(char ch) {
    return ((ch - '0' >= 0) && (ch - '0' <= 7));
}

/*
hexadecimal-digit : one of
	0 1 2 3 4 5 6 7 8 9
	a b c d e f
	A B C D E F
*/
bool Lexer::hexadecimal_digit(char ch) {
    return (((ch - '0' >= 0) && (ch - '0' <= 9)) ||
            (ch >= 'a' && ch <= 'f') ||
            (ch >= 'A' && ch <= 'F'));
}

/*
non-digit : one of
  _ $ a b c d e f g h i j k l m n o p q r s t u v w x y z
  A B C D E F G H I J K L M N O P Q R S T U V W X Y Z
*/
bool Lexer::non_digit(char ch) {
    return (ch == '_' || ch == '$'
            || (ch >= 'a' && ch <= 'z')
            || (ch >= 'A' && ch <= 'Z'));
}





