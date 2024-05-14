#ifndef LEXER_H
#define LEXER_H

#include "Token.h"
#include <vector>
#include <regex>
#include <stdexcept>
#include <sstream>

class Lexer {
public:
    Lexer(const std::string &script) : script(script), line(1), line_start(0) {}

    std::vector<Token> tokenize() {
        std::vector<Token> tokens;
        std::regex token_specification(
            "(SCENE|ACTOR|IF|GOTO|END|GIVE|TAKE)|"  // Keywords
            "(@[A-Za-z_][A-Za-z0-9_]*)|"  // Labels
            "(T\\d+)|"  // Tokens
            "(\\d+):|"  // Dialogue actor ID
            "(\"[^\"]*\")|"  // Strings
            "([^:\\n]+)|"  // Dialogue text or other content
            "(\\n)|"  // Newlines
            "([ \t]+)|"  // Skip
            "(.)"  // Mismatch
            );
        auto words_begin = std::sregex_iterator(script.begin(), script.end(), token_specification);
        auto words_end = std::sregex_iterator();

        for (std::sregex_iterator i = words_begin; i != words_end; ++i) {
            std::smatch match = *i;
            if (match[1].matched) tokens.push_back(make_token(keyword_to_token_type(match[1].str()), match[1]));
            else if (match[2].matched) tokens.push_back(make_token(LABEL, match[2]));
            else if (match[3].matched) tokens.push_back(make_token(TOKEN, match[3]));
            else if (match[4].matched) tokens.push_back(make_token(DIALOGUE_ID, match[4]));
            else if (match[5].matched) tokens.push_back(make_token(STRING, match[5]));
            else if (match[6].matched) tokens.push_back(make_token(DIALOGUE_TEXT, match[6]));
            else if (match[7].matched) {
                line++;
                line_start = static_cast<int>(match.position(7) + match.length(7));
            } else if (match[8].matched) continue;
            else if (match[9].matched) throw make_error("Unexpected token", match[9].str(), match.position(9));
        }
        return tokens;
    }

private:
    Token make_token(TokenType type, const std::string &value) {
        return Token{type, value, line, static_cast<int>(value.size() + 1)};
    }

    std::runtime_error make_error(const std::string &message, const std::string &token, std::ptrdiff_t position) const {
        std::ostringstream oss;
        oss << message << ": " << token << " at line " << line << ", column " << line_start << "\n";
        oss << "Context: \"" << get_context(position) << "\"";
        return std::runtime_error(oss.str());
    }

    std::string get_context(std::ptrdiff_t position) const {
        const int context_length = 20;
        std::ptrdiff_t start = std::max(position - context_length, std::ptrdiff_t(0));
        std::ptrdiff_t end = std::min(position + context_length, static_cast<std::ptrdiff_t>(script.size()));
        return script.substr(start, end - start);
    }

    TokenType keyword_to_token_type(const std::string &keyword) {
        if (keyword == "SCENE") return SCENE;
        if (keyword == "ACTOR") return ACTOR;
        if (keyword == "IF") return IF;
        if (keyword == "GOTO") return GOTO;
        if (keyword == "END") return END;
        if (keyword == "GIVE") return GIVE;
        if (keyword == "TAKE") return TAKE;
        throw make_error("Unknown keyword", keyword, 0);
    }

    std::string script;
    int line;
    int line_start;
};

#endif // LEXER_H
