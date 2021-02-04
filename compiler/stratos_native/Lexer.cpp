//
// Created by kamau on 1/29/21.
//

#include "Lexer.h"


Lexer::Lexer() {
    {
        if (s_keywords.empty()) {
            s_keywords["await"] = NodeType::Await;
            s_keywords["package"] = NodeType::Package;
            s_keywords["import"] = NodeType::Import;
            s_keywords["class"] = NodeType::Class;
            s_keywords["fun"] = NodeType::Function;
            s_keywords["var"] = NodeType::Variable;
            s_keywords["val"] = NodeType::Value;
            s_keywords["for"] = NodeType::For;

            s_keywords["while"] = NodeType::While;
            s_keywords["when"] = NodeType::When;
            s_keywords["true"] = NodeType::True;
            s_keywords["false"] = NodeType::False;

            s_keywords["break"] = NodeType::Break;
            s_keywords["continue"] = NodeType::Continue;

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
            s_single_char_tokens['['] = NodeType::BracketOpen;
            s_single_char_tokens[']'] = NodeType::BracketClose;
            s_single_char_tokens['^'] = NodeType::Caret;
            s_single_char_tokens[':'] = NodeType::Colon;
            s_single_char_tokens[']'] = NodeType::Comma;
            s_single_char_tokens['{'] = NodeType::CurlyOpen;
            s_single_char_tokens['}'] = NodeType::CurlyClose;
            s_single_char_tokens['='] = NodeType::Equals;
            s_single_char_tokens['!'] = NodeType::ExclamationMark;
            s_single_char_tokens['-'] = NodeType::Minus;
            s_single_char_tokens['('] = NodeType::ParenOpen;
            s_single_char_tokens[')'] = NodeType::ParenClose;
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
    return nodes;
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
    char peek_char;
    char start = current();
    switch (start) {
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
            if (non_digit(current())) {
                std::string text_string;
                int start_character_index = current_character_index;
                text_string.push_back(start);

                while (true) {

                    char advanced = advance();
                    std::cerr << "Start  " << start << " advance " << advanced << std::endl;

                    if (current() == ' ' || current() == '\n') {
                        std::cerr << "Start  " << std::endl;
                        if (current() == '\n') {
                            line_number++;
                        }
                        break;
                    } else {
                        std::cerr << "====  " << std::endl;
                        text_string.push_back(advanced);
                    }
                }
                Node var;
                var.start = start_character_index;
                var.end = current_character_index;
                var.line = line_number;
                var.literal = text_string;
                if (key_word(text_string)) {
                    var.type = s_keywords[text_string];

                } else {
                    var.type = NodeType::UserDefinedName;
                }

                nodes.push_back(
                        var
                );


            } else {


                if (current() == ' ' || current() == '\n') {
                    if (current() == '\n') {
                        line_number++;
                    }

                }else{
                    std::cerr << "Unknown " << current() << "is non digit " << non_digit(current())  << std::endl;
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

char Lexer::peek() {
//    if (!is_eof()) {
    int next = current_character_index + 1;
    return char_array[next];
//    }
//    return char_array[current_character_index];
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





