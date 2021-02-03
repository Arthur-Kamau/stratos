//
// Created by kamau on 1/29/21.
//

#include "Parser.h"

parser::parser(){
    {
        if (s_keywords.empty()) {
            s_keywords["await"] = TokenType::Await;
            s_keywords["class"] = TokenType::Class;
            s_keywords["fun"] = TokenType::Function;
            s_keywords["var"] = TokenType::Variable;
            s_keywords["val"] = TokenType::Value;
            s_keywords["for"] = TokenType::For;

            s_keywords["while"] = TokenType::While;
            s_keywords["when"] = TokenType::When;
            s_keywords["true"] = TokenType::True;
            s_keywords["false"] = TokenType::False;

            s_keywords["break"] = TokenType::Break;
            s_keywords["continue"] = TokenType::Continue;

        }

        if (s_two_char_tokens.empty()) {
            s_two_char_tokens["=="] = TokenType::EqualsEquals;
            s_two_char_tokens["<="] = TokenType::LessThanEquals;
            s_two_char_tokens[">="] = TokenType::GreaterThanEquals;
            s_two_char_tokens["!="] = TokenType::ExclamationMarkEquals;
            s_two_char_tokens["--"] = TokenType::MinusMinus;
            s_two_char_tokens["++"] = TokenType::PlusPlus;
        }

        if (s_single_char_tokens.empty()) {
            s_single_char_tokens['&'] = TokenType::Ampersand;
            s_single_char_tokens['*'] = TokenType::Asterisk;
            s_single_char_tokens['['] = TokenType::BracketOpen;
            s_single_char_tokens[']'] = TokenType::BracketClose;
            s_single_char_tokens['^'] = TokenType::Caret;
            s_single_char_tokens[':'] = TokenType::Colon;
            s_single_char_tokens[']'] = TokenType::Comma;
            s_single_char_tokens['{'] = TokenType::CurlyOpen;
            s_single_char_tokens['}'] = TokenType::CurlyClose;
            s_single_char_tokens['='] = TokenType::Equals;
            s_single_char_tokens['!'] = TokenType::ExclamationMark;
            s_single_char_tokens['-'] = TokenType::Minus;
            s_single_char_tokens['('] = TokenType::ParenOpen;
            s_single_char_tokens[')'] = TokenType::ParenClose;
            s_single_char_tokens['%'] = TokenType::Percent;
            s_single_char_tokens['.'] = TokenType::Period;
            s_single_char_tokens['|'] = TokenType::Pipe;
            s_single_char_tokens['+'] = TokenType::Plus;
            s_single_char_tokens['?'] = TokenType::QuestionMark;
            s_single_char_tokens[';'] = TokenType::Semicolon;
            s_single_char_tokens['/'] = TokenType::Slash;
            s_single_char_tokens['~'] = TokenType::Tilde;
            s_single_char_tokens['<'] = TokenType::LessThan;
            s_single_char_tokens['>'] = TokenType::GreaterThan;
        }
        if (s_three_char_tokens.empty()) {
            s_three_char_tokens["..."] = TokenType::TripleDot;
        }
    }
}