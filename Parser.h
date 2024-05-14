#ifndef PARSER_H
#define PARSER_H

#include "Token.h"
#include <vector>
#include <string>
#include <stdexcept>
#include <sstream>

struct Command {
    std::string type;
    std::vector<std::string> args;
};

struct Scene {
    int scene_id;
    std::vector<Command> commands;
};

class Parser {
public:
    Parser(const std::vector<Token> &tokens) : tokens(tokens), pos(0) {}

    std::vector<Scene> parse() {
        std::vector<Scene> scenes;
        while (pos < tokens.size()) {
            if (tokens[pos].type == SCENE) {
                scenes.push_back(parse_scene());
            } else {
                pos++;
            }
        }
        return scenes;
    }

private:
    Scene parse_scene() {
        pos++;  // Skip 'SCENE'
        int scene_id = std::stoi(tokens[pos++].value);
        std::vector<Command> commands;
        while (pos < tokens.size() && tokens[pos].type != END) {
            Command command = parse_command();
            if (!command.type.empty()) {
                commands.push_back(command);
            }
        }
        pos++;  // Skip 'END'
        return {scene_id, commands};
    }

    Command parse_command() {
        Token token = tokens[pos];
        pos++;
        if (token.type == ACTOR) {
            return parse_actor();
        } else if (token.type == IF) {
            return parse_if();
        } else if (token.type == GOTO) {
            return parse_goto();
        } else if (token.type == GIVE) {
            return parse_give();
        } else if (token.type == TAKE) {
            return parse_take();
        } else if (token.type == DIALOGUE_ID && tokens[pos].type == DIALOGUE_TEXT) {
            return parse_dialogue(token);
        } else if (token.type == LABEL) {
            return {"LABEL", {token.value}};
        } else {
            throw make_error("Unexpected token", token);
        }
    }

    Command parse_actor() {
        std::string id = tokens[pos++].value;
        std::string name = tokens[pos++].value;
        std::string skin = tokens[pos++].value;
        std::string pose = tokens[pos++].value;
        std::string x = tokens[pos++].value;
        std::string y = tokens[pos++].value;
        return {"ACTOR", {id, name, skin, pose, x, y}};
    }

    Command parse_if() {
        std::string condition = tokens[pos++].value;
        std::string label = tokens[pos++].value;
        return {"IF", {condition, label}};
    }

    Command parse_goto() {
        std::string destination = tokens[pos++].value;
        return {"GOTO", {destination}};
    }

    Command parse_give() {
        std::string token = tokens[pos++].value;
        return {"GIVE", {token}};
    }

    Command parse_take() {
        std::string token = tokens[pos++].value;
        return {"TAKE", {token}};
    }

    Command parse_dialogue(const Token &token) {
        std::string actor_id = token.value.substr(0, token.value.size() - 1); // Remove colon
        std::string dialogue = tokens[pos++].value;
        return {"DIALOGUE", {actor_id, dialogue}};
    }

    std::runtime_error make_error(const std::string &message, const Token &token) const {
        std::ostringstream oss;
        oss << message << ": " << token.value << " at line " << token.line << ", column " << token.column;
        return std::runtime_error(oss.str());
    }

    const std::vector<Token> &tokens;
    size_t pos;
};

#endif // PARSER_H
