#ifndef TOKEN_H
#define TOKEN_H

#include <string>

enum TokenType {
    SCENE,
    ACTOR,
    IF,
    GOTO,
    END,
    GIVE,
    TAKE,
    LABEL,
    TOKEN,
    NUMBER,
    STRING,
    DIALOGUE_ID,
    DIALOGUE_TEXT
};

struct Token {
    TokenType type;
    std::string value;
    int line;
    int column;
};

#endif // TOKEN_H
