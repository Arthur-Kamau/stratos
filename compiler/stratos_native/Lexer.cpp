//
// Created by kamau on 1/29/21.
//

#include "Lexer.h"

Lexer::Lexer(std::string source) {
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
    s_three_char_tokens["..."] =  TokenType::TripleDot;
}
}



//        s_keywords.insert("await", TokenType::Await);
//        s_keywords.insert("break", TokenType::Break);
//        s_keywords.insert("case", TokenType::Case);
//        s_keywords.insert("catch", TokenType::Catch);
//        s_keywords.insert("class", TokenType::Class);
//        s_keywords.insert("const", TokenType::Const);
//        s_keywords.insert("continue", TokenType::Continue);
//        s_keywords.insert("debugger", TokenType::Debugger);
//        s_keywords.insert("default", TokenType::Default);
//        s_keywords.insert("delete", TokenType::Delete);
//        s_keywords.insert("do", TokenType::Do);
//        s_keywords.insert("else", TokenType::Else);
//        s_keywords.insert("enum", TokenType::Enum);
//        s_keywords.insert("export", TokenType::Export);
//        s_keywords.insert("extends", TokenType::Extends);
//        s_keywords.insert("false", TokenType::BoolLiteral);
//        s_keywords.insert("finally", TokenType::Finally);
//        s_keywords.insert("for", TokenType::For);
//        s_keywords.insert("function", TokenType::Function);
//        s_keywords.insert("if", TokenType::If);
//        s_keywords.insert("import", TokenType::Import);
//        s_keywords.insert("in", TokenType::In);
//        s_keywords.insert("instanceof", TokenType::Instanceof);
//        s_keywords.insert("let", TokenType::Let);
//        s_keywords.insert("new", TokenType::New);
//        s_keywords.insert("null", TokenType::NullLiteral);
//        s_keywords.insert("return", TokenType::Return);
//        s_keywords.insert("super", TokenType::Super);
//        s_keywords.insert("switch", TokenType::Switch);
//        s_keywords.insert("this", TokenType::This);
//        s_keywords.insert("throw", TokenType::Throw);
//        s_keywords.insert("true", TokenType::BoolLiteral);
//        s_keywords.insert("try", TokenType::Try);
//        s_keywords.insert("typeof", TokenType::Typeof);
//        s_keywords.insert("var", TokenType::Var);
//        s_keywords.insert("void", TokenType::Void);
//        s_keywords.insert("while", TokenType::While);
//        s_keywords.insert("with", TokenType::With);
//        s_keywords.insert("yield", TokenType::Yield);
//    }
//
//    if (s_three_char_tokens.empty()) {
//        s_three_char_tokens.insert("===", TokenType::EqualsEqualsEquals);
//        s_three_char_tokens.insert("!==", TokenType::ExclamationMarkEqualsEquals);
//        s_three_char_tokens.insert("**=", TokenType::DoubleAsteriskEquals);
//        s_three_char_tokens.insert("<<=", TokenType::ShiftLeftEquals);
//        s_three_char_tokens.insert(">>=", TokenType::ShiftRightEquals);
//        s_three_char_tokens.insert("&&=", TokenType::DoubleAmpersandEquals);
//        s_three_char_tokens.insert("||=", TokenType::DoublePipeEquals);
//        s_three_char_tokens.insert("\?\?=", TokenType::DoubleQuestionMarkEquals);
//        s_three_char_tokens.insert(">>>", TokenType::UnsignedShiftRight);
//        s_three_char_tokens.insert("...", TokenType::TripleDot);
//    }
//
//    if (s_two_char_tokens.empty()) {
//        s_two_char_tokens.insert("=>", TokenType::Arrow);
//        s_two_char_tokens.insert("+=", TokenType::PlusEquals);
//        s_two_char_tokens.insert("-=", TokenType::MinusEquals);
//        s_two_char_tokens.insert("*=", TokenType::AsteriskEquals);
//        s_two_char_tokens.insert("/=", TokenType::SlashEquals);
//        s_two_char_tokens.insert("%=", TokenType::PercentEquals);
//        s_two_char_tokens.insert("&=", TokenType::AmpersandEquals);
//        s_two_char_tokens.insert("|=", TokenType::PipeEquals);
//        s_two_char_tokens.insert("^=", TokenType::CaretEquals);
//        s_two_char_tokens.insert("&&", TokenType::DoubleAmpersand);
//        s_two_char_tokens.insert("||", TokenType::DoublePipe);
//        s_two_char_tokens.insert("??", TokenType::DoubleQuestionMark);
//        s_two_char_tokens.insert("**", TokenType::DoubleAsterisk);
//        s_two_char_tokens.insert("==", TokenType::EqualsEquals);
//        s_two_char_tokens.insert("<=", TokenType::LessThanEquals);
//        s_two_char_tokens.insert(">=", TokenType::GreaterThanEquals);
//        s_two_char_tokens.insert("!=", TokenType::ExclamationMarkEquals);
//        s_two_char_tokens.insert("--", TokenType::MinusMinus);
//        s_two_char_tokens.insert("++", TokenType::PlusPlus);
//        s_two_char_tokens.insert("<<", TokenType::ShiftLeft);
//        s_two_char_tokens.insert(">>", TokenType::ShiftRight);
//        s_two_char_tokens.insert("?.", TokenType::QuestionMarkPeriod);
//    }
//

//    consume();
//}

